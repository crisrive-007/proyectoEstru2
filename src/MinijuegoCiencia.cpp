#include "MinijuegoCiencia.h"
#include <iostream>
#include <algorithm>

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
  m_sprMagikarp(m_texMagikarp),
  m_txtPregunta(m_font),
  m_txtA(m_font), m_txtB(m_font), m_txtC(m_font), m_txtD(m_font),
  m_txtFeedback(m_font),
  m_bgm(m_bufBgm), m_sndFish(m_bufFish), m_sndFail(m_bufFail)
{
    DEBUG_LOG("Inicializando MinijuegoCiencia...");
    DEBUG_LOG("Dirección del gestor de estados: " << g);
    DEBUG_LOG("Dirección de la ventana: " << &w);
    DEBUG_LOG("Dirección del personaje: " << &p);

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
    /*if (m_bufBgm.loadFromFile("assets/MinijuegoPesca/bgm.mp3")) {
        m_bgm.setBuffer(m_bufBgm);
        m_bgm.setLooping(true);
        DEBUG_LOG("BGM cargado correctamente");
    } else {
        DEBUG_WARNING("No se pudo cargar BGM");
    }*/

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

    // Fondo
    m_sprFondo.setTexture(m_texFondo, true);
    m_sprFondo.setScale({5.5f,5.2f});
    m_sprDialog.setTexture(m_texDialog, true);
    m_sprDialog.setScale({12.0f, 7.f});
    m_sprDialog.setPosition({0.f, 0.f});
    DEBUG_LOG("Sprites de fondo y diálogo configurados");

    // Textos
    auto setup = [&](sf::Text& t){
        t.setFont(m_font);
        t.setCharacterSize(25);
        t.setFillColor(sf::Color::Black);
        t.setScale({0.8f,1.f});
    };
    setup(m_txtPregunta);
    setup(m_txtA);
    setup(m_txtB);
    setup(m_txtC);
    setup(m_txtD);
    setup(m_txtFeedback);

    m_txtPregunta.setPosition({100.f, 40.f});
    float baseY = 90.f, dy = 38.f;
    m_txtA.setPosition({120.f, baseY});
    m_txtB.setPosition({120.f, baseY+dy});
    m_txtC.setPosition({120.f, baseY+dy*2});
    m_txtD.setPosition({120.f, baseY+dy*3});
    DEBUG_LOG("Textos configurados en posiciones: baseY=" << baseY << ", dy=" << dy);

    // Magikarp
    m_sprMagikarp.setTexture(m_texMagikarp, true);
    m_sprMagikarp.setScale({3.0f,3.0f});
    m_sprMagikarp.setColor(sf::Color(255,255,255,0));
    DEBUG_LOG("Sprite de Magikarp configurado");

    // Círculos: ovals blancos con borde
    float oceanY = 880.f;
    float spacing = 505.f;
    DEBUG_LOG("Configurando círculos en Y=" << oceanY << ", spacing=" << spacing);

    for (int i = 0; i < 4; ++i) {
        m_circulos[i].setRadius(60.f);
        m_circulos[i].setPointCount(60);
        m_circulos[i].setOrigin({60.f, 60.f});
        m_circulos[i].setPosition({spacing/2 + spacing * i, oceanY});
        m_circulos[i].setScale({1.8f, 1.0f}); // estira en X (óvalo)
        m_circulos[i].setFillColor(sf::Color(255,255,255,0)); // transparente
        m_circulos[i].setOutlineThickness(3.f);
        m_circulos[i].setOutlineColor(sf::Color::White);

        DEBUG_LOG("Círculo " << i << " en posición: ("
                  << (spacing/2 + spacing * i) << ", " << oceanY << ")");
    }

    DEBUG_LOG("Carga de assets completada");
}

void MinijuegoCiencia::armarPreguntas() {
    DEBUG_LOG("Armando preguntas...");
    m_pregs.clear();
    m_pregs.push_back({"Entre los siguientes renacentistas seleccione, uno de los precursores filosofo-cientifico del heliocentrismo (teoria que afirma que el sol es el centro del universo):", {"Tomas Moro","Galileo","Platon","Arquimedes"}, 1});
    m_pregs.push_back({"El metodo cientifico se introduce por el interés de tres filosofos. Entre los siguientes uno de los mencionados no es precursor del metodo cientifico:", {"Francis Bacon","Galileo Galilei","Nicolas Maquiavelo","Rene Descartes"}, 2});
    m_pregs.push_back({"Es uno de los precursores del pensamiento Moderno:", {"Isaac Newton","Rene Descartes","Erasmo de Roterdam","Francis Bacon"}, 1});
    m_pregs.push_back({"De los siguientes filosofos niega el geocentrismo (teoria que afirma que el centro de nuestro sistema solar es la tierra):", {"Aristoteles","Nicolas Copernico","Tomas de Aquino","Isaac Newton"}, 1});
    m_pregs.push_back({"Uno de los inventos que suscito un conocimiento ilimitado, fue el de Gutenberg:", {"El astrolabio","La imprenta","La Nao y la Carabela","El Telescopio"}, 1});

    DEBUG_LOG("Se crearon " << m_pregs.size() << " preguntas");
    for (size_t i = 0; i < m_pregs.size(); ++i) {
        DEBUG_LOG("Pregunta " << i << ": " << m_pregs[i].enunciado
                  << " (respuesta correcta: " << m_pregs[i].correcta << ")");
    }
}

void MinijuegoCiencia::iniciar() {
    DEBUG_LOG("Iniciando minijuego...");
    m_idxPregunta = 0;
    m_fase = Fase::Pregunta;
    m_showMagikarp = false;

    DEBUG_LOG("Reproduciendo BGM...");
    m_bgm.play();

    mostrarPregunta(m_idxPregunta);
    DEBUG_LOG("Minijuego iniciado. Fase actual: Pregunta, Índice: " << m_idxPregunta);
}

void MinijuegoCiencia::mostrarPregunta(int idx) {
    DEBUG_LOG("Mostrando pregunta índice: " << idx);

    if (idx >= (int)m_pregs.size()) {
        DEBUG_LOG("No hay más preguntas. Cambiando a fase Fin");
        m_fase = Fase::Fin;
        m_bgm.stop();
        m_txtPregunta.setString("¡Pescaste todos los Magikarp! (ENTER para salir)");
        m_txtA.setString("");
        m_txtB.setString("");
        m_txtC.setString("");
        m_txtD.setString("");
        m_txtFeedback.setString("");
        return;
    }

    const auto& P = m_pregs[idx];
    m_opcCorrecta = P.correcta;
    DEBUG_LOG("Pregunta actual: " << P.enunciado);
    DEBUG_LOG("Opción correcta: " << m_opcCorrecta);

    m_txtPregunta.setString(P.enunciado);
    m_txtA.setString("A) " + P.opciones[0]);
    m_txtB.setString("B) " + P.opciones[1]);
    m_txtC.setString("C) " + P.opciones[2]);
    m_txtD.setString("D) " + P.opciones[3]);

    m_txtFeedback.setString("");
    m_sprMagikarp.setColor(sf::Color(255,255,255,0));
    m_showMagikarp = false;
    m_fase = Fase::Pregunta;

    DEBUG_LOG("Pregunta mostrada en pantalla");
}

void MinijuegoCiencia::siguientePregunta() {
    DEBUG_LOG("Pasando a siguiente pregunta. Índice anterior: " << m_idxPregunta);
    ++m_idxPregunta;
    DEBUG_LOG("Nuevo índice: " << m_idxPregunta);
    mostrarPregunta(m_idxPregunta);
}

void MinijuegoCiencia::procesarRespuesta(int idx) {
    DEBUG_LOG("Procesando respuesta: " << idx);
    DEBUG_LOG("Fase actual: " << (int)m_fase << ", Opción correcta: " << m_opcCorrecta);

    if (m_fase != Fase::Pregunta) {
        DEBUG_WARNING("Intento de procesar respuesta en fase incorrecta: " << (int)m_fase);
        return;
    }

    if (idx == m_opcCorrecta) {
        DEBUG_LOG("Respuesta CORRECTA");
        m_sndFish.play();
        m_txtFeedback.setString("¡Has pescado un Magikarp!");
        m_showMagikarp = true;
        m_sprMagikarp.setColor(sf::Color::White);

        m_magiPos = m_circulos[idx].getPosition();
        m_magiPos.x -= 65.f;
        m_magiPos.y -= 50.f;
        m_sprMagikarp.setPosition(m_magiPos);
        m_magiVy = -90.f;
        m_clock.restart();

        DEBUG_LOG("Magikarp apareciendo en posición: (" << m_magiPos.x << ", " << m_magiPos.y << ")");
    } else {
        DEBUG_LOG("Respuesta INCORRECTA");
        m_sndFail.play();
        m_txtFeedback.setString("No ha picado nada...");
    }

    m_fase = Fase::Feedback;
    DEBUG_LOG("Cambiando a fase Feedback");
}

void MinijuegoCiencia::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            DEBUG_LOG("Evento: Ventana cerrada");
            window.close();
            return;
        }

        if (auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            DEBUG_LOG("Evento: Tecla presionada - " << (int)k->code);

            if (k->code == sf::Keyboard::Key::Enter) {
                DEBUG_LOG("Tecla ENTER presionada en fase: " << (int)m_fase);
                if (m_fase == Fase::Feedback) {
                    DEBUG_LOG("Avanzando a siguiente pregunta desde Feedback");
                    siguientePregunta();
                    return;
                }
                if (m_fase == Fase::Fin) {
                    DEBUG_LOG("Saliendo del minijuego desde fase Fin");
                    salirDelMinijuego();
                    return;
                }
            }
        }

        if (auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (m->button == sf::Mouse::Button::Left) {
                DEBUG_LOG("Evento: Click izquierdo en (" << m->position.x << ", " << m->position.y << ")");

                if (m_fase == Fase::Pregunta) {
                    sf::Vector2f mp = {float(m->position.x), float(m->position.y)};
                    DEBUG_LOG("Procesando click en fase Pregunta, coordenadas: (" << mp.x << ", " << mp.y << ")");

                    for (int i = 0; i < 4; ++i) {
                        sf::FloatRect bounds = m_circulos[i].getGlobalBounds();
                        bool contains = containsPoint(bounds, mp);
                        DEBUG_LOG("Círculo " << i << " bounds: (" << bounds.position.x << ", " << bounds.position.y
                                  << ", " << bounds.size.x << ", " << bounds.size.y << ") - Contiene: " << contains);

                        if (contains) {
                            DEBUG_LOG("Click en círculo " << i << " - Procesando respuesta");
                            procesarRespuesta(i);
                            break;
                        }
                    }
                } else {
                    DEBUG_LOG("Click ignorado - Fase actual no es Pregunta: " << (int)m_fase);
                }
            }
        }
    }
}

void MinijuegoCiencia::actualizar() {
    // Hover: agranda el óvalo cuando el cursor está encima
    const sf::Vector2i mi = sf::Mouse::getPosition(m_window);
    const sf::Vector2f mp = {float(mi.x), float(mi.y)};

    if (DEBUG_MODE) {
        static int hoverDebugCounter = 0;
        if (hoverDebugCounter++ % 60 == 0) { // Log cada ~1 segundo a 60 FPS
            DEBUG_LOG("Posición del mouse: (" << mp.x << ", " << mp.y << ")");
        }
    }

    for (int i = 0; i < 4; ++i) {
        bool over = (m_fase == Fase::Pregunta) && containsPoint(m_circulos[i].getGlobalBounds(), mp);
        float s = over ? 1.2f : 1.0f;
        m_circulos[i].setScale({1.8f*s, 1.0f*s});

        if (DEBUG_MODE && over) {
            static int lastHovered = -1;
            if (lastHovered != i) {
                DEBUG_LOG("Hover sobre círculo " << i);
                lastHovered = i;
            }
        }
    }

    // Animación del Magikarp
    if (m_showMagikarp) {
        float dt = m_clock.restart().asSeconds();
        m_magiPos.y += m_magiVy * dt;
        m_sprMagikarp.setPosition(m_magiPos);

        if (DEBUG_MODE) {
            static int magikarpDebugCounter = 0;
            if (magikarpDebugCounter++ % 30 == 0) {
                DEBUG_LOG("Magikarp posición: (" << m_magiPos.x << ", " << m_magiPos.y
                          << "), Velocidad Y: " << m_magiVy << ", Delta tiempo: " << dt);
            }
        }

        if (m_magiPos.y < 760.f) {
            auto c = m_sprMagikarp.getColor();
            if (c.a > 6) c.a -= 6; else c.a = 0;
            m_sprMagikarp.setColor(c);

            if (DEBUG_MODE && c.a % 30 == 0) {
                DEBUG_LOG("Magikarp alpha: " << (int)c.a);
            }

            if (c.a == 0) {
                m_showMagikarp = false;
                DEBUG_LOG("Animación de Magikarp completada");
            }
        }
    }
}

void MinijuegoCiencia::dibujar(sf::RenderWindow& window) {
    if (DEBUG_MODE) {
        static int frameCounter = 0;
        frameCounter++;
        if (frameCounter % 180 == 0) { // Log cada ~3 segundos a 60 FPS
            DEBUG_LOG("Dibujando frame " << frameCounter << ", Fase: " << (int)m_fase
                      << ", Magikarp visible: " << m_showMagikarp);
        }
    }

    window.setView(window.getDefaultView());
    window.draw(m_sprFondo);
    window.draw(m_sprDialog);
    window.draw(m_txtPregunta);
    window.draw(m_txtA);
    window.draw(m_txtB);
    window.draw(m_txtC);
    window.draw(m_txtD);
    for (auto& c : m_circulos) window.draw(c);
    if (m_showMagikarp) window.draw(m_sprMagikarp);
    if (m_fase == Fase::Feedback) {
        m_txtFeedback.setPosition({100.f, 250.f});
        window.draw(m_txtFeedback);
    }
}

void MinijuegoCiencia::salirDelMinijuego() {
    DEBUG_LOG("Saliendo del minijuego...");
    DEBUG_LOG("Deteniendo BGM");
    m_bgm.stop();

    DEBUG_LOG("Llamando a sacarEstado() del gestor");
    gestor->sacarEstado();

    DEBUG_LOG("Reposicionando personaje en (60, 400)");
    personaje.setPosition(60,400);

    DEBUG_LOG("Minijuego finalizado correctamente");
}

MinijuegoCiencia::~MinijuegoCiencia()
{
    DEBUG_LOG("Destructor de MinijuegoCiencia llamado");
    //dtor
}
