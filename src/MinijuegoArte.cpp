#include "MinijuegoArte.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstdint>

MinijuegoArte::MinijuegoArte(GestorEstados* g, sf::RenderWindow& w, Personaje& p)
: Estado(g, p), m_window(w), m_personaje(p),
  m_sprFondo(m_texFondo), m_sprDialog(m_texDialog), m_sprGreninja(m_texGreninja), m_sprShuriken(m_texShuriken),
  m_txtA(m_font), m_txtB(m_font), m_txtC(m_font), m_txtD(m_font), m_txtFeedback(m_font), m_txtFin(m_font), m_txtHUD(m_font), m_txtPregunta(m_font),
  m_bgm(m_bufBgm), m_sndCorrect(m_bufCorrect), m_sndThrow(m_bufThrow), m_sndWrong(m_bufWrong)
{
    std::cout << "[DEBUG] Constructor MinijuegoArte iniciado\n";
    cargarAssets();
    armarPreguntas();
    iniciar();
    std::cout << "[DEBUG] Constructor MinijuegoArte completado\n";
}

void MinijuegoArte::cargarAssets() {
    std::cout << "[DEBUG] Iniciando carga de assets...\n";

    auto loadT = [&](sf::Texture& t, const std::string& path){
        if (!t.loadFromFile(path)) {
            std::cerr << "[ERROR] No se pudo cargar: " << path << "\n";
        } else {
            std::cout << "[DEBUG] Textura cargada: " << path << "\n";
        }
    };

    // imágenes
    loadT(m_texFondo,    "assets/MinijuegoArte/fondo.png");
    loadT(m_texDialog,   "assets/MinijuegoArte/dialog.png");
    loadT(m_texGreninja, "assets/MinijuegoArte/greninja.png");
    loadT(m_texShuriken, "assets/MinijuegoArte/shuriken.png");
    loadT(m_texDiana,    "assets/MinijuegoArte/diana.png");

    loadT(m_texGreninja, "assets/MinijuegoArte/greninja_shuriken_sheet.png");
    m_texGreninja.setSmooth(false); // pixel art nítido

    m_sprGreninja.setTexture(m_texGreninja, true);

    // Construimos 4 frames horizontales de 120x90 (usa constructor SFML3)
    m_framesGreninja.clear();
    for (int i = 0; i < 4; ++i) {
        m_framesGreninja.emplace_back(
            sf::Vector2i{i * m_frameSize.x, 0},
            m_frameSize
        );
    }

    // Pose inicial (idle)
    m_frameIdx = 0;
    m_sprGreninja.setTextureRect(m_framesGreninja[m_frameIdx]);
    m_sprGreninja.setScale({3.f, 3.f});      // ajusta a tu gusto
    m_posGreninja = {220.f, 670.f};          // tu posición
    m_sprGreninja.setPosition(m_posGreninja);

    // opcional (si no está, usamos flash circular)
    //m_texExplosion.loadFromFile("assets/MinijuegoArte/explosion.png");

    // fuente
    if (!m_font.openFromFile("assets/Pokemon_GB.ttf")) {
        std::cerr << "[ERROR] No se pudo cargar la fuente\n";
    } else {
        std::cout << "[DEBUG] Fuente cargada correctamente\n";
    }

    // audio
    if (m_bufBgm.loadFromFile("assets/MinijuegoArte/bgm.mp3")) {
        m_bgm.setBuffer(m_bufBgm);
        m_bgm.setLooping(true);
        std::cout << "[DEBUG] BGM cargado\n";
    } else {
        std::cerr << "[ERROR] No se pudo cargar BGM\n";
    }

    if (m_bufThrow.loadFromFile("assets/MinijuegoArte/throw.mp3")) {
        m_sndThrow.setBuffer(m_bufThrow);
        std::cout << "[DEBUG] Sonido de lanzamiento cargado\n";
    }

    if (m_bufCorrect.loadFromFile("assets/MinijuegoArte/correct.mp3")) {
        m_sndCorrect.setBuffer(m_bufCorrect);
        std::cout << "[DEBUG] Sonido de acierto cargado\n";
    }

    if (m_bufWrong.loadFromFile("assets/MinijuegoArte/wrong.mp3")) {
        m_sndWrong.setBuffer(m_bufWrong);
        std::cout << "[DEBUG] Sonido de error cargado\n";
    }

    // sprites base
    m_sprFondo.setTexture(m_texFondo, true);
    m_sprFondo.setPosition({0.f,240.f});
    const auto ws = m_window.getSize();
    const auto ts = m_texFondo.getSize();
    if (ts.x && ts.y) {
        float scaleX = float(ws.x)/ts.x;
        float targetH = 835.f;
        float scaleY = targetH/ts.y;
        m_sprFondo.setScale({scaleX, scaleY});
    }
    std::cout << "[DEBUG] Sprite de fondo configurado\n";

    m_sprDialog.setTexture(m_texDialog, true);
    m_sprDialog.setScale({12.f, 7.f});
    m_sprDialog.setPosition({0.f, 0.f});
    std::cout << "[DEBUG] Sprite de diálogo configurado\n";

    m_sprGreninja.setTexture(m_texGreninja, true);
    m_sprGreninja.setScale({2.5f, 2.5f});
    m_posGreninja = {220.f, 670.f};
    m_sprGreninja.setPosition(m_posGreninja);
    std::cout << "[DEBUG] Greninja posicionado en (" << m_posGreninja.x << ", " << m_posGreninja.y << ")\n";

    m_sprShuriken.setTexture(m_texShuriken, true);
    m_sprShuriken.setScale({0.3f, 0.3f});
    m_sprShuriken.setColor(sf::Color(255,255,255,0));
    std::cout << "[DEBUG] Shuriken inicializado (invisible)\n";

    // dianas (A-D)
    m_posDianas = {
        sf::Vector2f{870.f, 480.f},
        sf::Vector2f{1120.f, 420.f},
        sf::Vector2f{1370.f, 420.f},
        sf::Vector2f{1620.f, 480.f}
    };

    for (size_t i = 0; i < 4; ++i) {
        m_dianas.emplace_back(m_texDiana);
        m_dianas.back().setScale({0.3f, 0.3f}); // 🔹 antes 1.8f, ahora más pequeñas
        auto sz = m_dianas.back().getGlobalBounds().size;
        m_dianas.back().setOrigin({sz.x / 2.f, sz.y / 2.f});
        m_dianas.back().setPosition(m_posDianas[i]);
        std::cout << "[DEBUG] Diana " << i << " posicionada en ("
                  << m_posDianas[i].x << ", " << m_posDianas[i].y << ")\n";
    }


    // textos
    auto setup = [&](sf::Text& t, unsigned ch=26){
        t.setFont(m_font);
        t.setCharacterSize(ch);
        t.setFillColor(sf::Color::Black);
    };
    setup(m_txtPregunta, 28);
    setup(m_txtA); setup(m_txtB); setup(m_txtC); setup(m_txtD);
    setup(m_txtHUD, 22);
    setup(m_txtFeedback, 28);
    setup(m_txtFin, 30);
    std::cout << "[DEBUG] Textos configurados\n";

    // flash circular (backup si no hay explosion.png)
    m_flash.setRadius(60.f);
    m_flash.setOrigin({60.f,60.f});
    m_flash.setFillColor(sf::Color(255,255,0,100));
    m_flash.setOutlineColor(sf::Color::White);
    m_flash.setOutlineThickness(3.f);
    std::cout << "[DEBUG] Efecto de flash configurado\n";

    std::cout << "[DEBUG] Carga de assets completada\n";
}

void MinijuegoArte::armarPreguntas() {
    while (!m_cola.empty()) m_cola.pop();

    m_cola.push({"Uno de los siguientes personajes fue el encargado de pintar la\ncapilla Sixtina:", {"Miguel Angel","Donatello","Leonardo Da Vinci","Francis Bacon"}, 0});
    m_cola.push({"Genio del renacimiento que esculpió el Moises, el David y la\nPieta:", {"Miguel Angel Buonarroti","Leonardo Da Vinci","Rafael Sanzio","Galileo Galilei"}, 0});
    m_cola.push({"Durante el renacimiento el estilo artistico que impregno el\narte, la filosofia, la pintura escritura fue el:", {"El gotico","El barroco","El clasicismo","El romanticismo"}, 1});
    m_cola.push({"Durante el renacimiento surge una nueva vision del hombre, que\nse vio reflejada en el arte, en la politica y en las ciencias\nsociales y humanas, a lo que se denomina:", {"Antropocentrismo","Humanismo","Paradigma antropologico","Teocentrismo"}, 1});
    m_cola.push({"Cuatro genios del renacimiento (Leonardo, Donatello, Rafael y\nMichelangelo) han sido llevados a la pantalla en los comics de:", {"Las tortugas ninjas","Los caballeros del Zodiaco","Los cuatro fantasticos","Los antagonistas de Attack Titan"}, 0});
}

void MinijuegoArte::iniciar() {
    std::cout << "[DEBUG] Iniciando minijuego...\n";

    m_correctas = 0;
    m_fallosSeguidos = 0;
    m_fallosTotales = 0;            // <-- NUEVO
    m_fase = Fase::EsperaSeleccion;

    m_frameIdx  = 0;
    m_frameTime = 0.f;
    if (!m_framesGreninja.empty())
        m_sprGreninja.setTextureRect(m_framesGreninja[m_frameIdx]);


    std::cout << "[DEBUG] Contadores inicializados - Correctas: " << m_correctas
              << ", Fallos seguidos: " << m_fallosSeguidos << "\n";

    cargarPreguntaActual();
    m_bgm.play();

    std::cout << "[DEBUG] BGM reproduciendo, fase: EsperaSeleccion\n";
}

void MinijuegoArte::cargarPreguntaActual() {
    std::cout << "[DEBUG] Cargando pregunta actual...\n";

    if (m_cola.empty()) {
        // Borrar opciones y parar música
        m_txtA.setString(""); m_txtB.setString(""); m_txtC.setString(""); m_txtD.setString("");
        m_bgm.stop();

        if (m_correctas == 5) {
            m_personaje.ganarVida();
            m_txtPregunta.setString("Felicidades, has acertado todas las dianas.\nHas ganado una vida extra");
        } else {
            m_txtPregunta.setString("Mala suerte, no ganas una vida extra");
        }

        // Mantén el mensaje final en m_txtPregunta y termina
        m_fase = Fase::Fin;
        return;
    }

    m_actual = m_cola.front();
    std::cout << "[DEBUG] Pregunta cargada: " << m_actual.enunciado << "\n";
    std::cout << "[DEBUG] Respuesta correcta: índice " << m_actual.correcta
              << " (" << m_actual.opciones[m_actual.correcta] << ")\n";

    // enunciado y opciones
    m_txtPregunta.setString(m_actual.enunciado);
    m_txtPregunta.setPosition({120.f, 48.f});

    m_txtA.setString("A) " + m_actual.opciones[0]);
    m_txtB.setString("B) " + m_actual.opciones[1]);
    m_txtC.setString("C) " + m_actual.opciones[2]);
    m_txtD.setString("D) " + m_actual.opciones[3]);

    // debajo del enunciado
    float oy = 144.f, dy = 40.f;
    m_txtA.setPosition({140.f, oy + dy*0});
    m_txtB.setPosition({140.f, oy + dy*1});
    m_txtC.setPosition({140.f, oy + dy*2});
    m_txtD.setPosition({140.f, oy + dy*3});

    m_txtLetras.clear();

    const char* Letras = "ABCD";
    for (int i = 0; i < 4; ++i) {
        m_txtLetras.emplace_back(m_font, Letras[i], 28);
        m_txtLetras.back().setFont(m_font);
        m_txtLetras.back().setString(std::string(1, Letras[i]));
        m_txtLetras.back().setCharacterSize(28);            // ajusta si quieres
        m_txtLetras.back().setFillColor(sf::Color::Black);
        m_txtLetras.back().setOutlineColor(sf::Color::White);
        m_txtLetras.back().setOutlineThickness(2.f);

        // Centrar sobre la diana (usa tu helper)
        setTextoCentro(m_txtLetras.back(), m_posDianas[i].x, m_posDianas[i].y);
    }

    m_txtFeedback.setString("");
    m_txtHUD.setString("Dianas acertadas: " + std::to_string(m_correctas));
    m_txtHUD.setPosition({50.f, 980.f});
    // reset de tiro
    m_sprShuriken.setColor(sf::Color(255,255,255,0));
    m_shurikenEnVuelo = false;
    m_explota = false;
    m_dianaQueExplota = -1;
    m_dianaObjetivo = -1;

    std::cout << "[DEBUG] Interfaz actualizada, esperando selección del jugador\n";
}

void MinijuegoArte::procesarSeleccion(int idxDiana) {
    std::cout << "[DEBUG] Procesando selección de diana " << idxDiana << "\n";

    if (m_shurikenEnVuelo) {
        std::cout << "[DEBUG] Shuriken ya en vuelo, ignorando selección\n";
        return;
    }

    m_dianaObjetivo = idxDiana;
    std::cout << "[DEBUG] Diana objetivo establecida: " << idxDiana
              << " (" << char('A' + idxDiana) << ")\n";

    // preparar shuriken
    m_posShuriken = m_posGreninja + sf::Vector2f{120.f, 40.f};
    m_sprShuriken.setPosition(m_posShuriken);
    m_sprShuriken.setColor(sf::Color::White);
    std::cout << "[DEBUG] Shuriken posicionado en (" << m_posShuriken.x
              << ", " << m_posShuriken.y << ")\n";

    // vector de dirección
    sf::Vector2f dst = m_posDianas[idxDiana];
    sf::Vector2f dir = {dst.x - m_posShuriken.x, dst.y - m_posShuriken.y};
    float mag = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    if (mag < 1.f) mag = 1.f;
    dir.x /= mag; dir.y /= mag;
    m_velShuriken = {dir.x * m_speedShuriken, dir.y * m_speedShuriken};
    startAnimLanzamiento(false);

    std::cout << "[DEBUG] Vector dirección: (" << dir.x << ", " << dir.y
              << "), magnitud: " << mag << "\n";
    std::cout << "[DEBUG] Velocidad shuriken: (" << m_velShuriken.x
              << ", " << m_velShuriken.y << ")\n";

    m_shurikenEnVuelo = true;
    m_clk.restart();

    m_sndThrow.play();
    std::cout << "[DEBUG] Shuriken lanzado, sonido reproducido\n";
}

void MinijuegoArte::resolverImpacto() {
    std::cout << "[DEBUG] Resolviendo impacto...\n";
    std::cout << "[DEBUG] Diana golpeada: " << m_dianaObjetivo
              << ", Respuesta correcta: " << m_actual.correcta << "\n";

    // comprobar si fue correcta
    m_ultimoCorrecto = (m_dianaObjetivo == m_actual.correcta);

    if (m_ultimoCorrecto) {
        ++m_correctas;
        m_fallosSeguidos = 0;
        m_txtPregunta.setString("Respuesta correcta, has acertado la diana");
        m_txtA.setString(""); m_txtB.setString(""); m_txtC.setString(""); m_txtD.setString("");
        m_sndCorrect.play();

        // efecto “explosión”
        m_explota = true;
        m_dianaQueExplota = m_dianaObjetivo;
        m_exploTimer = 0.f;
        m_flash.setPosition(m_posDianas[m_dianaQueExplota]);
    } else {
        ++m_fallosSeguidos;
        ++m_fallosTotales;
        // Mensaje en m_txtPregunta y limpiar opciones
        m_txtPregunta.setString("Respuesta incorrecta");
        m_txtA.setString(""); m_txtB.setString(""); m_txtC.setString(""); m_txtD.setString("");
        m_sndWrong.play();
    }

    // detener anim y volver a idle
   m_animLanzando = false;
    m_animLoop = false;
    m_frameIdx = 0;
    m_sprGreninja.setTextureRect(m_framesGreninja[m_frameIdx]);

    // Actualizar HUD después del tiro
    m_txtHUD.setString("Dianas acertadas: " + std::to_string(m_correctas));

    m_fase = Fase::Feedback;
}

void MinijuegoArte::siguiente() {
    std::cout << "[DEBUG] Avanzando a siguiente pregunta...\n";

    // consumir la pregunta que estaba al frente
    if (!m_cola.empty()) {
        m_cola.pop();
        std::cout << "[DEBUG] Pregunta anterior eliminada, quedan "
                  << m_cola.size() << " preguntas\n";
    }

    cargarPreguntaActual();

    if (m_fase != Fase::Fin) {
        m_fase = Fase::EsperaSeleccion;
        std::cout << "[DEBUG] Regresando a fase EsperaSeleccion\n";
    }
}

void MinijuegoArte::terminar(const std::string& mensaje) {
    std::cout << "[DEBUG] Terminando minijuego: " << mensaje << "\n";

    // Mostrar el mensaje FINAL en m_txtFin (es el que se dibuja en Fase::Fin)
    m_txtFin.setFont(m_font);
    m_txtFin.setCharacterSize(30);
    m_txtFin.setFillColor(sf::Color::Black);
    m_txtFin.setString(mensaje);
    setTextoCentro(m_txtFin, 960.f, 540.f);

    // Limpiar textos de opciones para no dejar “basura” en pantalla
    m_txtPregunta.setString("");
    m_txtA.setString("");
    m_txtB.setString("");
    m_txtC.setString("");
    m_txtD.setString("");

    m_fase = Fase::Fin;
    m_bgm.stop();

    std::cout << "[DEBUG] BGM detenido, fase establecida a Fin\n";
    std::cout << "[DEBUG] Estadísticas finales - Aciertos: " << m_correctas
              << ", Fallos seguidos: " << m_fallosSeguidos << "\n";
}

void MinijuegoArte::salirDelMinijuego() {
    std::cout << "[DEBUG] Saliendo del minijuego...\n";

    gestor->sacarEstado();
    personaje.setPosition(550, 295);

    std::cout << "[DEBUG] Estado removido, personaje reposicionado\n";
}

void MinijuegoArte::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            std::cout << "[DEBUG] Evento: Ventana cerrada\n";
            window.close();
            return;
        }

        if (auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            std::cout << "[DEBUG] Tecla presionada en fase " << static_cast<int>(m_fase) << "\n";

            if (k->code == sf::Keyboard::Key::Enter) {
                std::cout << "[DEBUG] Tecla ENTER presionada\n";

                if (m_fase == Fase::Feedback) {
                    std::cout << "[DEBUG] Avanzando desde Feedback\n";
                    siguiente();
                }
                else if (m_fase == Fase::Fin) {
                    std::cout << "[DEBUG] Saliendo desde Fin\n";
                    salirDelMinijuego();
                }
            }
        }

        if (auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (m->button == sf::Mouse::Button::Left && m_fase == Fase::EsperaSeleccion) {
                sf::Vector2f mp{float(m->position.x), float(m->position.y)};
                std::cout << "[DEBUG] Click izquierdo en (" << mp.x << ", " << mp.y << ")\n";

                for (int i=0;i<4;++i) {
                    if (containsPoint(m_dianas[i].getGlobalBounds(), mp)) {
                        std::cout << "[DEBUG] Click detectado en diana " << i << "\n";
                        procesarSeleccion(i);
                        break;
                    }
                }
            }
        }
    }
}

void MinijuegoArte::actualizar() {
    const float dt = m_clk.restart().asSeconds();
    actualizarAnimGreninja(dt);

    // pequeño "hover" a dianas
    const sf::Vector2i mi = sf::Mouse::getPosition(m_window);
    const sf::Vector2f mp{float(mi.x), float(mi.y)};

    for (int i=0;i<4;++i) {
        const bool over = (m_fase == Fase::EsperaSeleccion) && containsPoint(m_dianas[i].getGlobalBounds(), mp);
        const float s = over ? 0.5f : 0.3f;
        m_dianas[i].setScale({s, s});
    }

    // vuelo del shuriken
    if (m_shurikenEnVuelo && m_fase == Fase::EsperaSeleccion) {
        m_posShuriken += m_velShuriken * dt;
        m_sprShuriken.setPosition(m_posShuriken);

        // rotación estética
        m_sprShuriken.rotate(sf::degrees(1080.f * dt));

        // llegó (simple: distancia al centro diana)
        const sf::Vector2f dst = m_posDianas[m_dianaObjetivo];
        const float dx = dst.x - m_posShuriken.x;
        const float dy = dst.y - m_posShuriken.y;
        const float distSq = dx*dx + dy*dy;

        if (distSq < 30.f*30.f) {
            std::cout << "[DEBUG] Shuriken alcanzó la diana (distancia: "
                      << std::sqrt(distSq) << " pixels)\n";

            m_shurikenEnVuelo = false;
            m_sprShuriken.setColor(sf::Color(255,255,255,0));
            resolverImpacto();
        }
    }

    // anim explosión/flash
    if (m_explota) {
        m_exploTimer += dt;
        const float t = std::min(m_exploTimer / m_exploDur, 1.f);
        const float s = 1.f + (m_exploScaleMax - 1.f) * t;

        if (m_texExplosion.getSize().x > 0) {
            // si hay textura de explosión, la hacemos "pum" con escala
        } else {
            m_flash.setScale({s, s});
            auto c = m_flash.getFillColor();
            c.a = static_cast<std::uint8_t>(100 * (1.f - t));
            m_flash.setFillColor(c);
        }

        if (t >= 1.f) {
            std::cout << "[DEBUG] Animación de explosión completada\n";
            m_explota = false;
        }
    }
}

void MinijuegoArte::dibujar(sf::RenderWindow& window) {
    window.setView(window.getDefaultView());
    window.draw(m_sprFondo);
    window.draw(m_sprDialog);

    // pregunta/opciones
    window.draw(m_txtPregunta);
    window.draw(m_txtA);
    window.draw(m_txtB);
    window.draw(m_txtC);
    window.draw(m_txtD);


    // dianas
    for (auto& d : m_dianas) window.draw(d);

    // greninja + shuriken
    window.draw(m_sprGreninja);
    if (m_sprShuriken.getColor().a > 0) window.draw(m_sprShuriken);

    // explosión o flash
    if (m_explota) {
        if (m_texExplosion.getSize().x > 0) {
            sf::Sprite boom(m_texExplosion);
            auto gb = boom.getGlobalBounds();
            boom.setOrigin({gb.size.x/2.f, gb.size.y/2.f});
            boom.setPosition(m_posDianas[m_dianaQueExplota]);
            float t = std::min(m_exploTimer / m_exploDur, 1.f);
            float s = 0.4f + 1.2f * t;
            boom.setScale({s, s});
            window.draw(boom);
        } else {
            window.draw(m_flash);
        }
    }
    for (int i=0;i<4;++i) window.draw(m_txtLetras[i]);

    // feedback y HUD
    if (m_fase == Fase::Feedback) {
        setTextoCentro(m_txtFeedback, 960.f, 780.f);
        window.draw(m_txtFeedback);
    }
    window.draw(m_txtHUD);

    // fin
    /*if (m_fase == Fase::Fin) {
        window.draw(m_txtFin);
    }*/
}

void MinijuegoArte::startAnimLanzamiento(bool loop) {
    m_animLanzando = true;
    m_animLoop     = loop;
    m_frameIdx     = 0;
    m_frameTime    = 0.f;
    if (!m_framesGreninja.empty()) {
        m_sprGreninja.setTextureRect(m_framesGreninja[m_frameIdx]);
    }
}

void MinijuegoArte::actualizarAnimGreninja(float dt) {
    if (!m_animLanzando || m_framesGreninja.empty()) return;

    m_frameTime += dt;
    if (m_frameTime >= m_frameDur) {
        m_frameTime = 0.f;

        // avanzar
        int next = m_frameIdx + 1;

        // Si NO hay loop y llegamos al último frame, volvemos a idle y detenemos
        if (!m_animLoop && next >= static_cast<int>(m_framesGreninja.size())) {
            m_animLanzando = false;          // 🔚 detener animación
            m_frameIdx = 0;                  // 💤 volver a quieto (frame 0)
            m_sprGreninja.setTextureRect(m_framesGreninja[m_frameIdx]);
            return;
        }

        // Si hay loop o aún no llegamos al final
        if (m_animLoop && next >= static_cast<int>(m_framesGreninja.size())) {
            next = 0;
        }
        m_frameIdx = next;
        m_sprGreninja.setTextureRect(m_framesGreninja[m_frameIdx]);
    }
}
