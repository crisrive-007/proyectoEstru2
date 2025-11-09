#include "MinijuegoHistoria.h"
#include <iostream>

MinijuegoHistoria::MinijuegoHistoria(GestorEstados* g, sf::RenderWindow& w, Personaje& p)
: Estado(g, p),
  m_window(w),
  m_personaje(p),
  // Sprites primero (siguen el orden del .h)
  m_sprFondo(m_texFondo),
  m_sprDialog(m_texDialog),
  m_sprSnorlax(m_texSnorlax),
  // Textos (pregunta + HUD)
  m_txtPregunta(m_font, "", 26),
  m_txtHUD(m_font, "", 22),
  // Sonidos al final
  m_bgm(m_bufBgm),
  m_sndEat(m_bufEat),
  m_sndVomit(m_bufVomit),
  m_sndVictory(m_bufVictory)
{
    cargarAssets();
    iniciar();
}

void MinijuegoHistoria::cargarAssets() {
    auto load = [&](sf::Texture& t, const std::string& path){
        if (!t.loadFromFile(path))
            std::cerr << "[ERROR] No se pudo cargar: " << path << "\n";
    };

    load(m_texFondo,  "assets/MinijuegoHistoria/fondo.png");
    load(m_texDialog, "assets/MinijuegoHistoria/dialog.png");
    load(m_texSnorlax,"assets/MinijuegoHistoria/snorlax.png");
    load(m_texBayas[0],"assets/MinijuegoHistoria/atania.png");
    load(m_texBayas[1],"assets/MinijuegoHistoria/zidra.png");
    load(m_texBayas[2],"assets/MinijuegoHistoria/zreza.png");
    load(m_texBayas[3],"assets/MinijuegoHistoria/meloc.png");

    if (!m_font.openFromFile("assets/Pokemon_GB.ttf")) {
        std::cerr << "[ERROR] No se pudo cargar la fuente Pokemon_GB.ttf\n";
    }

    // Fondo (zona baja de la pantalla tipo “campo”)
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

    // Diálogo superior
    m_sprDialog.setTexture(m_texDialog, true);
    m_sprDialog.setPosition({0.f, 0.f});
    m_sprDialog.setScale({12.f, 7.f});

    // Snorlax
    m_sprSnorlax.setTexture(m_texSnorlax, true);
    m_sprSnorlax.setScale({6.f, 6.f});
    m_sprSnorlax.setPosition({850.f, 450.f});

    // Sonidos
    if (!m_bufBgm.loadFromFile("assets/MinijuegoHistoria/bgm.mp3")) {
        std::cerr << "[WARN] bgm.mp3 no cargó (prueba .wav/.ogg si tu build no soporta mp3)\n";
    } else {
        m_bgm.setBuffer(m_bufBgm);
        m_bgm.setLooping(true);
    }
    if (!m_bufEat.loadFromFile("assets/MinijuegoHistoria/eat.mp3"))
        std::cerr << "[WARN] eat.mp3 no cargó (usa .wav/.ogg si falla en tu build)\n";
    if (!m_bufVomit.loadFromFile("assets/MinijuegoHistoria/vomit.mp3"))
        std::cerr << "[WARN] vomit.mp3 no cargó\n";
    if (!m_bufVictory.loadFromFile("assets/MinijuegoHistoria/victory.mp3"))
        std::cerr << "[WARN] victory.mp3 no cargó\n";

    m_sndEat.setBuffer(m_bufEat);
    m_sndVomit.setBuffer(m_bufVomit);
    m_sndVictory.setBuffer(m_bufVictory);

    // Texto pregunta (cuadro superior)
    m_txtPregunta.setFont(m_font);
    m_txtPregunta.setCharacterSize(28);
    m_txtPregunta.setFillColor(sf::Color::Black);
    m_txtPregunta.setPosition({120.f, 48.f});

    // HUD (esquina inferior izquierda)
    m_txtHUD.setFont(m_font);
    m_txtHUD.setCharacterSize(22);
    m_txtHUD.setFillColor(sf::Color::Black);
    m_txtHUD.setPosition({50.f, 980.f});
    m_txtHUD.setString("Bayas comidas: 0");
}

void MinijuegoHistoria::armarPreguntas() {
    // Vaciar la cola por si reiniciamos
    while (!m_pregs.empty()) m_pregs.pop();

    // NOTA: corrige aquí los índices con tu banco real
    m_pregs.push({"Despues del feudalismo medieval acudimos al surgimiento de una nueva clase social conocida como la:",{"A) La monarquia","B) El mercantilismo","C) La burguesia","D) El proletariado"}, 2});
    m_pregs.push({"El renacimiento supone una epoca de absolutismo y nacionalismos, como el nacimiento de fuertes monarquias europeas centralizadas como:",{"A) Grecia","B) Inglaterra","C) Yugoslavia","D) Egipto"}, 1});
    m_pregs.push({"Antes de la consolidacion del estado moderno, Italia estuvo divida en pequeñas ciudades-estado normalmente enfrentadas entre si, como es el caso de:",{"A) Florencia-Napoli","B) Amsterdam-Cracovia","C) Reims-Colonia","D) Milan-Londres"}, 0});
    m_pregs.push({"La toma de Constantinopla supone un bloqueo comercial entre Europa y Asia (la ruta de la seda) y ocurrio en lo que hoy es actualmente:",{"A) Eslovaquia","B) Estambul en Turquia","C) Mesopotamia","D) Jerusalen"}, 1});
    m_pregs.push({"Resurge el interés por Grecia y Roma, junto al declive del sistema feudal, el crecimiento del comercio e innovaciones entre las que mencionamos:", {"A) La imprenta y la brujula","B) La rueda y la escritura","C) Las maquinas de vapor y la produccion en masa","D) La polvora y la rueda"}, 0});
}

void MinijuegoHistoria::iniciar() {
    std::cout << "[DEBUG] Iniciar minijuego (cola)\n";
    armarPreguntas();            // ← siempre reponemos la cola al iniciar
    m_fase = Fase::Pregunta;
    m_seleccion = -1;
    m_pregActual.reset();

    m_bayasComidas = 0;
    actualizarHUD();

    m_bgm.play();
    siguientePregunta();         // toma la primera
}

void MinijuegoHistoria::mostrarPregunta() {
    if (!m_pregActual.has_value()) {
        std::cerr << "[ERROR] mostrarPregunta() sin pregunta activa\n";
        return;
    }
    const auto& P = *m_pregActual;

    // Texto de pregunta
    m_txtPregunta.setString(P.enunciado);
    m_correcta = P.correcta;

    // Opciones en el cuadro
    m_txtOpciones.clear();
    float oy = 144.f, dy = 40.f;
    for (int i = 0; i < 4; ++i) {
        m_txtOpciones.emplace_back(m_font, P.opciones[i], 26);
        m_txtOpciones.back().setFillColor(sf::Color::Black);
        m_txtOpciones.back().setString(P.opciones[i]);
        m_txtOpciones.back().setPosition({140.f, oy + dy * i});
    }

    // Bayas + letras (A-D) alineadas en la parte baja
    const auto ws = m_window.getSize();
    const float centerX = ws.x * 0.5f;
    const float y = 780.f;
    const float spacing = 280.f;

    m_bayas.clear();
    m_bayas.reserve(4);
    m_txtLetras.clear();
    for (int i = 0; i < 4; ++i) {
        float cx = centerX - (1.5f * spacing) + (i * spacing);
        m_bayas.emplace_back(m_texBayas[i]);
        m_bayas.back().setScale({6.f, 6.f});
        m_bayas.back().setPosition({cx, y});

        m_txtLetras.emplace_back(m_font, std::string(1, char('A' + i)), 22);
        m_txtLetras.back().setFillColor(sf::Color::Black);
        m_txtLetras.back().setPosition({cx + 15.f, y + 110.f});
    }

    actualizarHUD();
}

void MinijuegoHistoria::resolverClick(int idxBaya) {
    if (m_fase != Fase::Pregunta) return;
    if (idxBaya < 0 || idxBaya > 3) return;

    m_seleccion = idxBaya;
    bool ok = (idxBaya == m_correcta);

    if (ok) {
        m_sndEat.play();
        m_txtPregunta.setString("Respuesta correcta, Snorlax se ha comido la baya");
        m_sprSnorlax.setColor(sf::Color::White);
        m_bayasComidas++;
        actualizarHUD();
    } else {
        m_sndVomit.play();
        m_txtPregunta.setString("Respuesta incorrecta, la baya ya estaba pordida");
        // puedes dar un pequeño tinte si quieres feedback visual
        m_sprSnorlax.setColor(sf::Color(255, 220, 220));
    }

    // IMPORTANTE: limpiar opciones para que solo quede el mensaje de feedback
    limpiarOpciones();

    m_fase = Fase::Feedback; // Enter avanza
}

void MinijuegoHistoria::siguientePregunta() {
    // ¿Quedan preguntas en la cola?
    if (m_pregs.empty()) {
        m_fase = Fase::Fin;
        m_bgm.stop();

        // Limpia A/B/C/D por si acaso
        limpiarOpciones();

        // Mensaje final según 5/5
        if (m_bayasComidas == m_total) {
            m_txtPregunta.setString("Felicidades Snorlax esta satisfecho.\nHas ganado una vida extra.");
            m_personaje.ganarVida();
        } else {
            m_txtPregunta.setString("Mala suerte, no ganas una vida extra");
        }
        return;
    }

    // Tomar la siguiente pregunta (FIFO)
    m_pregActual = m_pregs.front();
    m_pregs.pop();

    // Reset visual / lógica y pintar
    m_seleccion = -1;
    m_fase = Fase::Pregunta;
    m_sprSnorlax.setColor(sf::Color::White);

    mostrarPregunta();
}

void MinijuegoHistoria::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            window.close();
            return;
        }
        if (auto* kb = ev->getIf<sf::Event::KeyPressed>()) {
            if (kb->code == sf::Keyboard::Key::Enter) {
                if (m_fase == Fase::Feedback) {
                    siguientePregunta();
                    return;
                }
                if (m_fase == Fase::Fin) {
                    salir();
                    return;
                }
            }
        }
        if (auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button == sf::Mouse::Button::Left && m_fase == Fase::Pregunta) {
                const sf::Vector2f mpos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                for (int i = 0; i < 4; ++i) {
                    if (m_bayas[i].getGlobalBounds().contains(mpos)) {
                        resolverClick(i);
                        break;
                    }
                }
            }
        }
    }
}

void MinijuegoHistoria::actualizar() {
    const sf::Vector2f mpos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
    for (int i = 0; i < static_cast<int>(m_bayas.size()); ++i) {
        bool over = (m_fase == Fase::Pregunta) && m_bayas[i].getGlobalBounds().contains(mpos);
        float hover = over ? 6.5f : 6.0f;
        m_bayas[i].setScale({hover, hover});
    }
}

void MinijuegoHistoria::dibujar(sf::RenderWindow& window) {
    window.setView(window.getDefaultView());
    window.draw(m_sprFondo);
    window.draw(m_sprDialog);
    window.draw(m_txtPregunta);

    // (Opcional) texto de opciones en el cuadro
    for (auto& t : m_txtOpciones) window.draw(t);

    for (auto& s : m_bayas) window.draw(s);
    for (auto& l : m_txtLetras) window.draw(l);

    window.draw(m_sprSnorlax);

    // HUD siempre visible
    window.draw(m_txtHUD);
}

void MinijuegoHistoria::salir() {
    gestor->sacarEstado();      // vuelve al estado anterior
    personaje.setPosition(1530, 295); // reubicar si así lo usas en tu mapa
}

void MinijuegoHistoria::limpiarOpciones() {
    // Vacía textos A/B/C/D y oculta letras bajo las bayas
    for (auto& t : m_txtOpciones) t.setString("");
    m_txtLetras.clear();
}

void MinijuegoHistoria::actualizarHUD() {
    m_txtHUD.setString("Bayas comidas: " + std::to_string(m_bayasComidas));
}

MinijuegoHistoria::~MinijuegoHistoria() {}
