#include "MinijuegoCiencia.h"
#include <iostream>
#include <algorithm>
#include <cmath>

// Constantes para depuración
#define DEBUG_MODE true
#define DEBUG_LOG(x) if (DEBUG_MODE) std::cout << "[DEBUG] " << x << std::endl
#define DEBUG_ERROR(x) if (DEBUG_MODE) std::cerr << "[ERROR] " << x << std::endl
#define DEBUG_WARNING(x) if (DEBUG_MODE) std::cout << "[WARNING] " << x << std::endl

MinijuegoCiencia::MinijuegoCiencia(GestorEstados* g, sf::RenderWindow& w, Personaje& p)
: Estado(g, p),
  m_window(w),
  m_personaje(p),
  m_sprFondo(m_texFondo),
  m_sprDialog(m_texDialog),
  m_sprMagikarp(m_texMagikarp), m_sprPescador(m_texPescador),
  m_txtPregunta(m_font),
  m_txtA(m_font), m_txtB(m_font), m_txtC(m_font), m_txtD(m_font),
  m_txtFeedback(m_font), m_txtHUD(m_font),
  m_bgm(m_bufBgm), m_sndFish(m_bufFish), m_sndFail(m_bufFail)
{
    DEBUG_LOG("Inicializando MinijuegoCiencia...");
    cargarAssets();
    armarPreguntas();
    iniciar();
    DEBUG_LOG("MinijuegoCiencia inicializado correctamente");
}

void MinijuegoCiencia::cargarAssets() {
    DEBUG_LOG("Cargando assets...");

    auto loadT = [&](sf::Texture& t, const std::string& path){
        DEBUG_LOG("Intentando cargar textura: " << path);
        if (!t.loadFromFile(path)) {
            DEBUG_ERROR("No se pudo cargar textura: " << path);
        } else {
            DEBUG_LOG("Textura cargada: " << path);
        }
    };

    loadT(m_texFondo,   "assets/MinijuegoCiencia/fondo.png");
    loadT(m_texDialog,  "assets/MinijuegoCiencia/dialog.png");
    loadT(m_texMagikarp,"assets/MinijuegoCiencia/magikarp.png");

    DEBUG_LOG("Intentando cargar fuente: assets/Pokemon_GB.ttf");
    if (!m_font.openFromFile("assets/Pokemon_GB.ttf")) {
        DEBUG_ERROR("No se pudo cargar fuente: assets/Pokemon_GB.ttf");
    } else {
        DEBUG_LOG("Fuente cargada correctamente");
    }

    // Audio
    DEBUG_LOG("Cargando archivos de audio...");
    if (m_bufBgm.loadFromFile("assets/MinijuegoCiencia/bgm.mp3")) {
        m_bgm.setBuffer(m_bufBgm);
        m_bgm.setLooping(true);
        DEBUG_LOG("BGM cargado correctamente");
    } else {
        DEBUG_WARNING("No se pudo cargar BGM");
    }

    if (m_bufFish.loadFromFile("assets/MinijuegoCiencia/fish.wav")) {
        m_sndFish.setBuffer(m_bufFish);
        DEBUG_LOG("Sonido de pesca cargado correctamente");
    } else {
        DEBUG_WARNING("No se pudo cargar sonido de pesca");
    }

    if (m_bufFail.loadFromFile("assets/MinijuegoCiencia/fail.mp3")) {
        m_sndFail.setBuffer(m_bufFail);
        DEBUG_LOG("Sonido de fallo cargado correctamente");
    } else {
        DEBUG_WARNING("No se pudo cargar sonido de fallo");
    }

    // Fondo y diálogo
    m_sprFondo.setTexture(m_texFondo, true);
    m_sprFondo.setScale({5.5f,5.2f});
    m_sprDialog.setTexture(m_texDialog, true);
    m_sprDialog.setScale({12.0f, 7.f});
    m_sprDialog.setPosition({0.f, 0.f});
    DEBUG_LOG("Sprites de fondo y diálogo configurados");

    // Textos
    auto setup = [&](sf::Text& t, unsigned ch=26){
        t.setFont(m_font);
        t.setCharacterSize(ch);
        t.setFillColor(sf::Color::Black);
    };
    setup(m_txtPregunta, 28);
    setup(m_txtA); setup(m_txtB); setup(m_txtC); setup(m_txtD);
    setup(m_txtFeedback, 28);
    setup(m_txtHUD, 22); // HUD

    m_txtPregunta.setPosition({120.f, 48.f});
    float oy = 144.f, dy = 40.f;
    m_txtA.setPosition({140.f, oy + dy*0});
    m_txtB.setPosition({140.f, oy + dy*1});
    m_txtC.setPosition({140.f, oy + dy*2});
    m_txtD.setPosition({140.f, oy + dy*3});

    // HUD abajo-izquierda
    m_txtHUD.setPosition({50.f, 980.f});
    m_txtHUD.setString("Magikarps pescados: 0");

    // Pescador
    if (!m_texPescador.loadFromFile("assets/MinijuegoCiencia/personaje.png")) {
        DEBUG_WARNING("No se pudo cargar personaje.png");
    }
    m_texPescador.setSmooth(false);
    m_sprPescador.setTexture(m_texPescador, true);

    // frames (SFML3: IntRect({pos}, {size}))
    m_framesPesc.clear();
    for (int i = 0; i < 4; ++i) {
        m_framesPesc.emplace_back(sf::Vector2i{i * m_frameSize.x, 0}, m_frameSize);
    }
    m_frameIdx = 0;
    m_sprPescador.setTextureRect(m_framesPesc[m_frameIdx]);
    m_sprPescador.setScale({4.f, 4.f});
    m_sprPescador.setPosition({896.f, 400.f});

    m_ropeVA = sf::VertexArray(sf::PrimitiveType::LineStrip);

    // Magikarp
    m_sprMagikarp.setTexture(m_texMagikarp, true);
    m_sprMagikarp.setScale({3.0f,3.0f});
    m_sprMagikarp.setColor(sf::Color(255,255,255,0));
    DEBUG_LOG("Sprite de Magikarp configurado");

    // Círculos
    float oceanY = 880.f;
    float spacing = 505.f;
    for (int i = 0; i < 4; ++i) {
        m_circulos[i].setRadius(60.f);
        m_circulos[i].setPointCount(60);
        m_circulos[i].setOrigin({60.f, 60.f});
        m_circulos[i].setPosition({spacing/2 + spacing * i, oceanY});
        m_circulos[i].setScale({1.8f, 1.0f}); // óvalo
        m_circulos[i].setFillColor(sf::Color(255,255,255,0));
        m_circulos[i].setOutlineThickness(3.f);
        m_circulos[i].setOutlineColor(sf::Color::White);
    }

    DEBUG_LOG("Carga de assets completada");
}

void MinijuegoCiencia::armarPreguntas() {
    DEBUG_LOG("Armando preguntas...");
    while (!m_pregs.empty()) m_pregs.pop(); // limpiar cola

    m_pregs.push({"Entre los siguientes renacentistas seleccione, uno de los\nprecursores filosofo-cientifico del heliocentrismo (teoria que\nafirma que el sol es el centro del universo):", {"Tomas Moro","Galileo","Platon","Arquimedes"}, 1});
    m_pregs.push({"El metodo cientifico se introduce por el interés de tres\nfilosofos. Entre los siguientes uno de los mencionados no es\nprecursor del metodo cientifico:", {"Francis Bacon","Galileo Galilei","Nicolas Maquiavelo","Rene Descartes"}, 2});
    m_pregs.push({"Es uno de los precursores del pensamiento Moderno:", {"Isaac Newton","Rene Descartes","Erasmo de Roterdam","Francis Bacon"}, 1});
    m_pregs.push({"De los siguientes filosofos niega el geocentrismo (teoria que\nafirma que el centro de nuestro sistema solar es la tierra):", {"Aristoteles","Nicolas Copernico","Tomas de Aquino","Isaac Newton"}, 1});
    m_pregs.push({"Uno de los inventos que suscito un conocimiento ilimitado, fue\nel de Gutenberg:", {"El astrolabio","La imprenta","La Nao y la Carabela","El Telescopio"}, 1});

    DEBUG_LOG("Se crearon " << m_pregs.size() << " preguntas en cola");
}

void MinijuegoCiencia::iniciar() {
    DEBUG_LOG("Iniciando minijuego...");
    m_fase = Fase::Pregunta;
    m_showMagikarp = false;
    m_pescados = 0;
    m_txtHUD.setString("Magikarps pescados: 0");

    DEBUG_LOG("Reproduciendo BGM...");
    m_bgm.play();

    mostrarPregunta();
}

void MinijuegoCiencia::mostrarPregunta() {
    if (m_pregs.empty()) {
        // FIN DEL JUEGO: mensaje final pedido
        m_bgm.stop();
        // limpiar opciones
        m_txtA.setString(""); m_txtB.setString(""); m_txtC.setString(""); m_txtD.setString("");

        if (m_pescados == m_total) {
            m_txtPregunta.setString("Felicidades has pescado todos los Magikarps.\nHas ganado una vida extra.");
            m_personaje.ganarVida();
        } else {
            m_txtPregunta.setString("Mala suerte, no ganas una vida extra");
        }

        m_txtFeedback.setString("");
        m_fase = Fase::Fin;
        return;
    }

    m_pregActual = m_pregs.front(); // tomar la primera
    m_opcCorrecta = m_pregActual.correcta;

    m_txtPregunta.setString(m_pregActual.enunciado);
    m_txtA.setString("A) " + m_pregActual.opciones[0]);
    m_txtB.setString("B) " + m_pregActual.opciones[1]);
    m_txtC.setString("C) " + m_pregActual.opciones[2]);
    m_txtD.setString("D) " + m_pregActual.opciones[3]);
    m_txtFeedback.setString("");

    // reinicio de visuales por pregunta
    m_showMagikarp = false;
    m_sprMagikarp.setColor(sf::Color(255,255,255,0));

    m_fase = Fase::Pregunta;
    DEBUG_LOG("Mostrando pregunta actual de la cola");
}

void MinijuegoCiencia::siguientePregunta() {
    DEBUG_LOG("Pasando a siguiente pregunta...");
    if (!m_pregs.empty()) m_pregs.pop(); // eliminar la actual

    // reset animaciones y cuerda
    m_animLanzando = false;
    m_animLoop     = false;
    m_frameIdx     = 0;
    if (!m_framesPesc.empty()) m_sprPescador.setTextureRect(m_framesPesc[m_frameIdx]);
    m_cuerdaActiva = false;
    m_cuerdaT      = 0.f;
    m_objetivoIdx  = -1;

    mostrarPregunta(); // siguiente o fin
}

void MinijuegoCiencia::procesarRespuesta(int idx) {
    DEBUG_LOG("Procesando respuesta: " << idx);
    if (m_fase != Fase::Pregunta && m_fase != Fase::Lanzamiento) {
        DEBUG_WARNING("Intento de procesar respuesta en fase incorrecta: " << (int)m_fase);
        return;
    }

    // Limpia opciones SIEMPRE tras responder
    auto limpiarOpciones = [&](){
        m_txtA.setString(""); m_txtB.setString("");
        m_txtC.setString(""); m_txtD.setString("");
        m_txtFeedback.setString("");
    };

    if (idx == m_opcCorrecta) {
        DEBUG_LOG("Respuesta CORRECTA");
        m_sndFish.play();

        // mensaje en m_txtPregunta
        m_txtPregunta.setString("Respuesta correcta, has pescado un Magikarp");

        // sumar pescados + HUD
        m_pescados++;
        m_txtHUD.setString("Magikarps pescados: " + std::to_string(m_pescados));

        // Mostrar magikarp
        m_showMagikarp = true;
        m_sprMagikarp.setColor(sf::Color::White);
        m_magiPos = m_circulos[idx].getPosition();
        m_magiPos.x -= 65.f;
        m_magiPos.y -= 50.f;
        m_sprMagikarp.setPosition(m_magiPos);
        m_magiVy = -90.f;

        DEBUG_LOG("Magikarp en (" << m_magiPos.x << ", " << m_magiPos.y << ")");
    } else {
        DEBUG_LOG("Respuesta INCORRECTA");
        m_sndFail.play();

        // mensaje en m_txtPregunta
        m_txtPregunta.setString("Respuesta incorrecta, no ha picado nada");

        // no mostrar magikarp
        m_showMagikarp = false;
        m_sprMagikarp.setColor(sf::Color(255,255,255,0));
    }

    // limpiar A/B/C/D tras responder
    limpiarOpciones();

    // Entrar a Feedback (Enter para avanzar)
    m_fase = Fase::Feedback;
    DEBUG_LOG("Cambiando a fase Feedback");
}

void MinijuegoCiencia::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        if (auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::Enter) {
                if (m_fase == Fase::Feedback) { // avanzar
                    siguientePregunta();
                    return;
                }
                if (m_fase == Fase::Fin) {
                    salirDelMinijuego();
                    return;
                }
            }
        }

        if (auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (m->button == sf::Mouse::Button::Left) {
                if (m_fase == Fase::Pregunta) {
                    sf::Vector2f mp = {float(m->position.x), float(m->position.y)};
                    for (int i = 0; i < 4; ++i) {
                        if (containsPoint(m_circulos[i].getGlobalBounds(), mp)) {
                            m_objetivoIdx = i;

                            // Origen cuerda: mano del pescador aprox
                            sf::Vector2f origen = m_sprPescador.getPosition() + sf::Vector2f{40.f, 10.f};
                            sf::Vector2f destino = m_circulos[i].getPosition();

                            iniciarCuerda(origen, destino);
                            startAnimLanzamiento(false);

                            m_fase = Fase::Lanzamiento;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void MinijuegoCiencia::actualizar() {
    // dt global
    float dt = m_clock.restart().asSeconds();

    // hover círculos
    const sf::Vector2i mi = sf::Mouse::getPosition(m_window);
    const sf::Vector2f mp = {float(mi.x), float(mi.y)};
    for (int i = 0; i < 4; ++i) {
        bool over = (m_fase == Fase::Pregunta) && containsPoint(m_circulos[i].getGlobalBounds(), mp);
        float s = over ? 1.2f : 1.0f;
        m_circulos[i].setScale({1.8f*s, 1.0f*s});
    }

    // anim pescador
    actualizarAnimPescador(dt);

    // cuerda y llegada
    if (m_fase == Fase::Lanzamiento && m_cuerdaActiva && m_objetivoIdx >= 0) {
        actualizarCuerda(dt);
        if (m_cuerdaT >= 1.f) {
            // stop anim
            m_animLanzando = false;
            m_animLoop     = false;
            m_frameIdx     = 0;
            if (!m_framesPesc.empty())
                m_sprPescador.setTextureRect(m_framesPesc[m_frameIdx]);

            m_cuerdaActiva = false;

            // evaluar
            procesarRespuesta(m_objetivoIdx);
            m_objetivoIdx = -1;
        }
    }

    // anim magikarp
    if (m_showMagikarp) {
        m_magiPos.y += m_magiVy * dt;
        m_sprMagikarp.setPosition(m_magiPos);

        if (m_magiPos.y < 760.f) {
            auto c = m_sprMagikarp.getColor();
            if (c.a > 6) c.a -= 6; else c.a = 0;
            m_sprMagikarp.setColor(c);
            if (c.a == 0) m_showMagikarp = false;
        }
    }
}

void MinijuegoCiencia::dibujar(sf::RenderWindow& window) {
    window.setView(window.getDefaultView());
    window.draw(m_sprFondo);
    window.draw(m_sprDialog);

    // textos y opciones (A/B/C/D vacíos durante feedback/fin)
    window.draw(m_txtPregunta);
    window.draw(m_txtA); window.draw(m_txtB); window.draw(m_txtC); window.draw(m_txtD);

    // sprites
    window.draw(m_sprPescador);
    if (m_cuerdaActiva || m_fase == Fase::Lanzamiento) {
        dibujarCuerda(window);
    }
    for (auto& c : m_circulos) window.draw(c);
    if (m_showMagikarp) window.draw(m_sprMagikarp);

    // HUD siempre visible
    window.draw(m_txtHUD);
}

void MinijuegoCiencia::salirDelMinijuego() {
    m_bgm.stop();
    gestor->sacarEstado();
    personaje.setPosition(1200, 295);
}

void MinijuegoCiencia::startAnimLanzamiento(bool loop) {
    m_animLanzando = true;
    m_animLoop     = loop;
    m_frameIdx     = 0;
    m_frameTime    = 0.f;
    if (!m_framesPesc.empty())
        m_sprPescador.setTextureRect(m_framesPesc[m_frameIdx]);
}

void MinijuegoCiencia::actualizarAnimPescador(float dt) {
    if (!m_animLanzando || m_framesPesc.empty()) return;

    m_frameTime += dt;
    if (m_frameTime >= m_frameDur) {
        m_frameTime = 0.f;
        int next = m_frameIdx + 1;

        // sin loop
        if (!m_animLoop && next >= (int)m_framesPesc.size()) {
            m_animLanzando = false;
            m_frameIdx = 0; // idle
            m_sprPescador.setTextureRect(m_framesPesc[m_frameIdx]);
            return;
        }

        if (m_animLoop && next >= (int)m_framesPesc.size()) next = 0;
        m_frameIdx = next;
        m_sprPescador.setTextureRect(m_framesPesc[m_frameIdx]);
    }
}

void MinijuegoCiencia::iniciarCuerda(const sf::Vector2f& origen, const sf::Vector2f& destino) {
    m_cuerdaA = origen;
    m_cuerdaB = destino;
    m_cuerdaT = 0.f;
    m_cuerdaActiva = true;

    const int N = 20;
    m_ropeVA = sf::VertexArray(sf::PrimitiveType::LineStrip);
    m_ropeVA.resize(N);

    for (int i = 0; i < N; ++i) {
        m_ropeVA[i].color = sf::Color(20, 20, 20, 230);
    }
}

static sf::Vector2f bezierQuad(const sf::Vector2f& A, const sf::Vector2f& C, const sf::Vector2f& B, float t) {
    const float u = 1.f - t;
    return (u*u)*A + (2.f*u*t)*C + (t*t)*B;
}

void MinijuegoCiencia::actualizarCuerda(float dt) {
    if (!m_cuerdaActiva) return;
    m_cuerdaT = std::min(1.f, m_cuerdaT + m_cuerdaVel * dt);

    sf::Vector2f mid = (m_cuerdaA + m_cuerdaB) * 0.5f + sf::Vector2f{0.f, -60.f};

    int N = (int)m_ropeVA.getVertexCount();
    for (int i = 0; i < N; ++i) {
        float tLocal = (float)i / (N - 1);
        float tCapped = std::min(tLocal, m_cuerdaT);
        sf::Vector2f p = bezierQuad(m_cuerdaA, mid, m_cuerdaB, tCapped);

        float waveAmp = 6.f * (1.f - m_cuerdaT);
        float wave    = std::sin(18.f * tLocal + 12.f * m_cuerdaT) * waveAmp;
        p.y += wave;

        m_ropeVA[i].position = p;
    }
}

void MinijuegoCiencia::dibujarCuerda(sf::RenderWindow& window) const {
    window.draw(m_ropeVA);
}

MinijuegoCiencia::~MinijuegoCiencia() {
    DEBUG_LOG("Destructor de MinijuegoCiencia llamado");
}
