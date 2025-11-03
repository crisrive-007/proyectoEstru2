#include "MinijuegoHistoria.h"
#include <iostream>

MinijuegoHistoria::MinijuegoHistoria(GestorEstados* g, sf::RenderWindow& w, Personaje& p)
: Estado(g, p), m_window(w), m_personaje(p),
  m_sprFondo(m_texFondo), m_sprDialog(m_texDialog), m_sprSnorlax(m_texSnorlax),
  m_sndEat(m_bufEat), m_sndVomit(m_bufVomit), m_sndVictory(m_bufVictory), m_txtPregunta(m_font)
{
    std::cout << "[DEBUG] Constructor MinijuegoHistoria iniciado\n";
    cargarAssets();
    armarPreguntas();
    iniciar();
    std::cout << "[DEBUG] Constructor MinijuegoHistoria completado\n";
}

void MinijuegoHistoria::cargarAssets() {
    std::cout << "[DEBUG] Iniciando carga de assets...\n";

    auto load = [&](sf::Texture& t, const std::string& path){
        std::cout << "[DEBUG] Intentando cargar: " << path << "\n";
        if (!t.loadFromFile(path)) {
            std::cerr << "[ERROR] No se pudo cargar: " << path << "\n";
        } else {
            std::cout << "[DEBUG] Cargado exitosamente: " << path << "\n";
        }
    };

    load(m_texFondo,  "assets/MinijuegoHistoria/fondo.png");
    load(m_texDialog, "assets/MinijuegoHistoria/dialog.png");
    load(m_texSnorlax,"assets/MinijuegoHistoria/snorlax.png");
    load(m_texBayas[0],"assets/MinijuegoHistoria/atania.png");
    load(m_texBayas[1],"assets/MinijuegoHistoria/zidra.png");
    load(m_texBayas[2],"assets/MinijuegoHistoria/zreza.png");
    load(m_texBayas[3],"assets/MinijuegoHistoria/meloc.png");

    std::cout << "[DEBUG] Cargando fuente...\n";
    if (!m_font.openFromFile("assets/Pokemon_GB.ttf")) {
        std::cerr << "[ERROR] No se pudo cargar la fuente\n";
    } else {
        std::cout << "[DEBUG] Fuente cargada exitosamente\n";
    }

    // Fondo
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
    std::cout << "[DEBUG] Sprite fondo configurado\n";

    // Diálogo
    m_sprDialog.setTexture(m_texDialog, true);
    m_sprDialog.setPosition({0.f, 0.f});
    m_sprDialog.setScale({12.f, 5.f});
    std::cout << "[DEBUG] Sprite diálogo configurado\n";

    // Snorlax
    m_sprSnorlax.setTexture(m_texSnorlax, true);
    m_sprSnorlax.setScale({6.f, 6.f});
    m_sprSnorlax.setPosition({850.f, 450.f});
    std::cout << "[DEBUG] Sprite Snorlax configurado en posición (850, 450)\n";

    // Sonidos
    std::cout << "[DEBUG] Cargando sonidos...\n";
    if (!m_bufEat.loadFromFile("assets/MinijuegoHistoria/eat.mp3")) {
        std::cerr << "[ERROR] No se pudo cargar eat.mp3\n";
    }
    if (!m_bufVomit.loadFromFile("assets/MinijuegoHistoria/vomit.mp3")) {
        std::cerr << "[ERROR] No se pudo cargar vomit.mp3\n";
    }
    if (!m_bufVictory.loadFromFile("assets/MinijuegoHistoria/victory.mp3")) {
        std::cerr << "[ERROR] No se pudo cargar victory.mp3\n";
    }

    m_sndEat.setBuffer(m_bufEat);
    m_sndVomit.setBuffer(m_bufVomit);
    m_sndVictory.setBuffer(m_bufVictory);
    std::cout << "[DEBUG] Sonidos configurados\n";

    // Texto pregunta
    m_txtPregunta.setFont(m_font);
    m_txtPregunta.setCharacterSize(26);
    m_txtPregunta.setFillColor(sf::Color::Black);
    m_txtPregunta.setPosition({80.f, 36.f});
    std::cout << "[DEBUG] Texto de pregunta configurado\n";
}

void MinijuegoHistoria::armarPreguntas() {
    std::cout << "[DEBUG] Armando preguntas...\n";
    m_pregs.clear();
    m_pregs.push_back({"¿Qué Pokémon es de tipo fuego?", {"Squirtle","Bulbasaur","Charmander","Snorlax"}, 3});
    m_pregs.push_back({"¿Cuál es la evolución de Eevee tipo agua?", {"Flareon","Vaporeon","Jolteon","Umbreon"}, 1});
    m_pregs.push_back({"¿Qué objeto sirve para atrapar Pokémon?", {"Baya","Pokébola","Superpoción","Antídoto"}, 1});
    m_pregs.push_back({"¿Cuál es el color de Pikachu?", {"Rojo","Amarillo","Azul","Verde"}, 1});
    std::cout << "[DEBUG] Total de preguntas cargadas: " << m_pregs.size() << "\n";
}

void MinijuegoHistoria::iniciar() {
    std::cout << "[DEBUG] Iniciando minijuego...\n";
    m_idx = 0;
    m_fase = Fase::Pregunta;
    mostrarPregunta(m_idx);
    std::cout << "[DEBUG] Minijuego iniciado - Primera pregunta mostrada\n";
}

void MinijuegoHistoria::mostrarPregunta(int idx) {
    std::cout << "[DEBUG] Mostrando pregunta #" << idx << "\n";

    if (idx >= static_cast<int>(m_pregs.size())) {
        std::cout << "[DEBUG] Todas las preguntas completadas - Cambiando a fase Fin\n";
        m_fase = Fase::Fin;
        m_txtPregunta.setString("¡Snorlax está satisfecho! (ENTER para salir)");
        m_sndVictory.play();
        std::cout << "[DEBUG] Sonido de victoria reproducido\n";
        return;
    }

    const auto& P = m_pregs[idx];
    m_txtPregunta.setString(P.enunciado);
    m_correcta = P.correcta;
    std::cout << "[DEBUG] Pregunta: " << P.enunciado << "\n";
    std::cout << "[DEBUG] Respuesta correcta: " << P.correcta << " (" << P.opciones[P.correcta] << ")\n";

    m_txtOpciones.clear();
    m_txtLetras.clear();
    for (int i = 0; i < 4; ++i) {
        m_txtOpciones.emplace_back(m_font, P.opciones[i], 22);
        m_txtOpciones.back().setFillColor(sf::Color::Black);
        m_txtOpciones.back().setPosition({120.f, 80.f + i * 28.f});

        m_txtLetras.emplace_back(m_font, std::string(1, static_cast<char>('A' + i)), 22);
        m_txtLetras.back().setFillColor(sf::Color::Black);
    }

    const auto ws = m_window.getSize();
    const float centerX = ws.x * 0.5f;
    const float y = 780.f;
    const float spacing = 280.f;

    std::cout << "[DEBUG] Tamaño ventana: " << ws.x << "x" << ws.y << ", CenterX: " << centerX << "\n";

    m_bayas.clear();
    m_txtLetras.clear();

    for (int i = 0; i < 4; ++i) {
        // Crear sprite de baya directamente desde la textura
        m_bayas.emplace_back(m_texBayas[i]);
        m_bayas.back().setScale({6.f, 6.f});

        float cx = centerX - (1.5f * spacing) + (i * spacing);
        m_bayas.back().setPosition({cx, y});

        // Letra A, B, C, D
        m_txtLetras.emplace_back(m_font, std::string(1, static_cast<char>('A' + i)), 22);
        m_txtLetras.back().setFillColor(sf::Color::Black);
        m_txtLetras.back().setPosition({cx + 15.f, y + 110.f});

        std::cout << "[DEBUG] Baya " << i << " posicionada en (" << cx << ", " << y << ")\n";
    }
}

void MinijuegoHistoria::resolverClick(int idxBaya) {
    std::cout << "[DEBUG] Click en baya #" << idxBaya << "\n";

    if (m_fase != Fase::Pregunta) {
        std::cout << "[DEBUG] Click ignorado - No está en fase Pregunta\n";
        return;
    }
    if (idxBaya < 0 || idxBaya > 3) {
        std::cout << "[DEBUG] Click ignorado - Índice fuera de rango\n";
        return;
    }

    m_seleccion = idxBaya;
    bool correcto = (idxBaya == m_correcta);

    std::cout << "[DEBUG] Selección: " << idxBaya << ", Correcta: " << m_correcta
              << ", Resultado: " << (correcto ? "CORRECTA" : "INCORRECTA") << "\n";

    if (correcto) {
        m_sndEat.play();
        m_txtPregunta.setString("¡Snorlax se comió la baya con gusto! (ENTER)");
        m_sprSnorlax.setColor(sf::Color::White);
        std::cout << "[DEBUG] Respuesta correcta - Sonido 'eat' reproducido\n";
    } else {
        m_sndVomit.play();
        m_txtPregunta.setString("¡Puaj! Snorlax vomitó la baya... (ENTER)");
        m_sprSnorlax.setColor(sf::Color(255, 150, 150));
        std::cout << "[DEBUG] Respuesta incorrecta - Sonido 'vomit' reproducido\n";
    }

    m_fase = Fase::Feedback;
    std::cout << "[DEBUG] Cambiando a fase Feedback\n";
}

void MinijuegoHistoria::siguientePregunta() {
    m_fase = Fase::Pregunta;
    std::cout << "[DEBUG] Avanzando a siguiente pregunta (actual: " << m_idx << ")\n";
    ++m_idx;
    mostrarPregunta(m_idx);
    m_sprSnorlax.setColor(sf::Color::White);
    std::cout << "[DEBUG] Color de Snorlax restaurado\n";
}

void MinijuegoHistoria::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            std::cout << "[DEBUG] Evento: Cerrar ventana\n";
            window.close();
            return;
        }

        if (auto* kb = ev->getIf<sf::Event::KeyPressed>()) {
            std::cout << "[DEBUG] Tecla presionada - Fase actual: "
                      << (m_fase == Fase::Pregunta ? "Pregunta" :
                          m_fase == Fase::Feedback ? "Feedback" : "Fin") << "\n";

            if (kb->code == sf::Keyboard::Key::Enter) {
                std::cout << "[DEBUG] Tecla ENTER presionada\n";
                if (m_fase == Fase::Feedback) {
                    std::cout << "[DEBUG] Procesando siguiente pregunta...\n";
                    siguientePregunta();
                    return;
                }
                if (m_fase == Fase::Fin) {
                    std::cout << "[DEBUG] Saliendo del minijuego...\n";
                    salir();
                    return;
                }
            }
        }

        if (auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button == sf::Mouse::Button::Left && m_fase == Fase::Pregunta) {
                const sf::Vector2f mpos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                std::cout << "[DEBUG] Click izquierdo en posición: (" << mpos.x << ", " << mpos.y << ")\n";

                for (int i=0;i<4;++i) {
                    if (m_bayas[i].getGlobalBounds().contains(mpos)) {
                        std::cout << "[DEBUG] Click detectado en baya #" << i << "\n";
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

    for (int i=0;i<4;++i) {
        bool over = (m_fase == Fase::Pregunta) && m_bayas[i].getGlobalBounds().contains(mpos);
        float hover = over ? 6.5f : 6.0f;

        // Log solo cuando cambia el hover (evitar spam)
        static bool wasOver[4] = {false, false, false, false};
        if (over != wasOver[i]) {
            std::cout << "[DEBUG] Hover baya #" << i << ": " << (over ? "ON" : "OFF") << "\n";
            wasOver[i] = over;
        }

        m_bayas[i].setScale({hover, hover});
    }
}

void MinijuegoHistoria::dibujar(sf::RenderWindow& window) {
    // Comentar este log si genera demasiada salida (se llama cada frame)
    // std::cout << "[DEBUG] Dibujando frame...\n";

    window.setView(window.getDefaultView());
    window.draw(m_sprFondo);
    window.draw(m_sprDialog);
    window.draw(m_txtPregunta);

    for (int i=0;i<4;++i) window.draw(m_txtOpciones[i]);
    for (int i=0;i<4;++i) window.draw(m_bayas[i]);
    for (int i=0;i<4;++i) window.draw(m_txtLetras[i]);

    window.draw(m_sprSnorlax);
}

void MinijuegoHistoria::salir() {
    std::cout << "[DEBUG] Ejecutando salida del minijuego...\n";
    gestor->sacarEstado();
    personaje.setPosition(60,400);
    std::cout << "[DEBUG] Personaje reposicionado a (60, 400)\n";
    std::cout << "[DEBUG] Estado removido del gestor\n";
}

MinijuegoHistoria::~MinijuegoHistoria()
{
    std::cout << "[DEBUG] Destructor MinijuegoHistoria llamado\n";
}
