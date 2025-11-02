#include "MinijuegoPolitica.h"
#include <iostream>
#include <algorithm>

MinijuegoPolitica::MinijuegoPolitica(GestorEstados* g, sf::RenderWindow& w, Personaje& p)
: Estado(g, p), m_window(w), m_personaje(p),
  m_sprFondo(m_texFondo), m_sprDialog(m_texDialog), m_txtPregunta(m_font),
  m_sndCorrect(m_bufCorrect), m_sndWrong(m_bufWrong), m_sndVictory(m_bufVictory)
{
    cargarAssets();
    armarPreguntas();
    iniciar();
}

void MinijuegoPolitica::cargarAssets() {
    auto loadTex = [&](sf::Texture& t, const std::string& path){
        if (!t.loadFromFile(path)) std::cerr << "[FALTA] " << path << "\n";
    };

    // Imágenes (usa tus assets)
    loadTex(m_texFondo,    "assets/MinijuegoPolitica/fondo.png");
    loadTex(m_texDialog,   "assets/MinijuegoPolitica/dialog.png");
    loadTex(m_texPokebola, "assets/MinijuegoPolitica/pokebola.png");
    loadTex(m_texEevee,    "assets/MinijuegoPolitica/eevee.png");
    loadTex(m_texGengar,   "assets/MinijuegoPolitica/gengar.png");

    if (!m_font.openFromFile("assets/Pokemon_GB.ttf"))
        std::cerr << "[FALTA] assets/Pokemon_GB.ttf\n";

    // Fondo: lo dejamos desde Y=240 para que el diálogo arriba tenga aire
    m_sprFondo.setTexture(m_texFondo, true);
    m_sprFondo.setPosition({0.f, 240.f});
    const auto ws = m_window.getSize();
    const auto ts = m_texFondo.getSize();
    if (ts.x && ts.y) {
        float scaleX = float(ws.x)/ts.x;
        float targetH = 835.f;
        float scaleY = targetH/ts.y;
        m_sprFondo.setScale({scaleX, scaleY});
    }

    // Barra de diálogo arriba
    m_sprDialog.setTexture(m_texDialog, true);
    m_sprDialog.setScale({12.f, 5.f});
    m_sprDialog.setPosition({0.f, 0.f});

    // Texto superior: pregunta y opciones
    m_txtPregunta.setFont(m_font);
    m_txtPregunta.setCharacterSize(26);
    m_txtPregunta.setFillColor(sf::Color::Black);
    m_txtPregunta.setPosition({80.f, 36.f});
    m_txtPregunta.setScale({0.8f,0.8f});

    m_txtOpc.clear();
    m_txtOpc.reserve(4);
    for (int i=0;i<4;++i){
        m_txtOpc.emplace_back(m_font, "", 22);
        m_txtOpc.back().setFillColor(sf::Color::Black);
        m_txtOpc.back().setPosition({80.f, 80.f + 30.f*i});
    }

    // Sonidos (usa los mp3 subidos)
    if (m_bufCorrect.loadFromFile("assets/MinijuegoPolitica/correct.wav")) m_sndCorrect.setBuffer(m_bufCorrect);
    if (m_bufWrong.loadFromFile("assets/MinijuegoPolitica/wrong.mp3"))     m_sndWrong.setBuffer(m_bufWrong);
    if (m_bufVictory.loadFromFile("assets/MinijuegoPolitica/victory.mp3")) m_sndVictory.setBuffer(m_bufVictory);
}

void MinijuegoPolitica::armarPreguntas() {
    m_pregs.clear();
    m_pregs.push_back({"Durante el renacimiento, el modelo de gobierno es uno de los siguientes:",
        {"Monarquia absoluta","Tirania republicana","Democracia participativa","Liberalismo politico"}, 0});
    m_pregs.push_back({"De los siguientes acontecimientos, selecciones el que inicia el periodo moderno:",
        {"Toma de Constantinopla","Tratado de paz de Westfalia","Toma de la Bastilla","La ruta de la seda"}, 1});
    m_pregs.push_back({"Durante el siglo XV, la sociedad se estratifica en tres estamentos definidos:",
        {"Clase media, baja y alta","Nobleza, clero y estado llano","Artesanos, guardianes y gobernantes","Reyes, nobleza y pecheros"}, 1});
    m_pregs.push_back({"Aparece el realismo politico, que se basaba en un orden establecido, explicacion de un sistema y recomendaciones de como gobernar:",
        {"Tomas Moro","Jean Bodin","Nicolas Maquiavelo","Erasmo de Rotterdam"}, 2});
    m_pregs.push_back({"Terminada la edad media, en el contexto de la politica resulta que:",
        {"La Iglesia resalta su poder","La Iglesia pierde el papel rector en la politica","La Iglesia evangelica se posiciona en la politica","La politica desaparece"}, 1});
}

void MinijuegoPolitica::iniciar() {
    m_window.setView(m_window.getDefaultView());
    m_idx  = 0;
    m_fase = Fase::Pregunta;
    mostrarPregunta(m_idx);
}

void MinijuegoPolitica::mostrarPregunta(int idx) {
    if (idx < 0 || idx >= static_cast<int>(m_pregs.size())) {
        m_fase = Fase::Fin;
        m_txtPregunta.setString("¡Listo! Has terminado. (ENTER para salir)");
        m_sndVictory.play();
        return;
    }

    const auto& P = m_pregs[idx];
    m_txtPregunta.setString(P.enunciado);
    m_correcta = P.correcta;

    static const char* pref[4] = {"A) ", "B) ", "C) ", "D) "};
    for (int i=0;i<4;++i) m_txtOpc[i].setString(std::string(pref[i]) + P.opciones[i]);

    // (Re)crear 4 bolas/letras/revelos
    m_bolas.clear();     m_bolas.reserve(4);
    m_txtLetra.clear();  m_txtLetra.reserve(4);
    m_revelos.clear();   m_revelos.reserve(4);

    for (int i=0;i<4;++i) {
        // Sprite de pokébola con textura y escala base
        m_bolas.emplace_back(m_texPokebola);
        m_bolas.back().setScale({m_scaleBola, m_scaleBola});

        // Letra centrada
        m_txtLetra.emplace_back(m_font, std::string(1,"ABCD"[i]), 100);
        m_txtLetra.back().setFillColor(sf::Color::Black);

        // Revelo (Eevee por defecto), centrado y oculto
        m_revelos.emplace_back(m_texEevee);
        m_revelos.back().setScale({ m_scaleRevelo, m_scaleRevelo });
        m_revelos.back().setColor(sf::Color(255,255,255,0));

        m_overlayTipo[i] = -1;
    }

    // --- Centramos orígenes al centro para alinear todo por "centro" ---
    for (int i=0;i<4;++i) {
        auto lbB = m_bolas[i].getLocalBounds();
        m_bolas[i].setOrigin({lbB.position.x + lbB.size.x * 0.5f, lbB.position.y + lbB.size.y * 0.5f });

        auto lbT = m_txtLetra[i].getLocalBounds();
        m_txtLetra[i].setOrigin({lbT.position.x + lbT.size.x * 0.5f, lbT.position.y + lbT.size.y * 0.5f });

        auto lbR = m_revelos[i].getLocalBounds();
        m_revelos[i].setOrigin({lbR.position.x + lbR.size.x * 0.5f, lbR.position.y + lbR.size.y * 0.5f });
    }

    // --- Layout 2x2 CENTRADO EN PANTALLA ---
    const auto ws = m_window.getSize();
    const float centerX = ws.x * 0.5f;  // 960 en 1920
    const float dx = 520.f;             // separación columnas
    const float yTop = 500.f;           // fila superior
    const float dy = 300.f;             // separación filas

    for (int i = 0; i < 4; ++i) {
        int col = i % 2;        // 0,1
        int row = i / 2;        // 0,1
        float cx = centerX + (col ? +dx * 0.5f : -dx * 0.5f);
        float cy = yTop + dy * row;

        m_bolas[i].setPosition({ cx, cy });
        m_txtLetra[i].setPosition({ cx, cy });
        m_revelos[i].setPosition({ cx, cy });
    }

    m_seleccion = -1;
    m_fase = Fase::Pregunta;
}

void MinijuegoPolitica::resolverClick(int idxPokebola) {
    if (m_fase != Fase::Pregunta) return;
    if (idxPokebola < 0 || idxPokebola > 3) return;

    m_seleccion = idxPokebola;
    const bool ok = (idxPokebola == m_correcta);

    for (int i=0;i<4;++i) m_overlayTipo[i] = -1;

    if (ok) {
        m_sndCorrect.play();
        m_overlayTipo[idxPokebola] = 0; // Eevee
        m_revelos[idxPokebola].setTexture(m_texEevee, true);
        m_txtPregunta.setString("¡Correcto! (ENTER para continuar)");
    } else {
        m_sndWrong.play();
        m_overlayTipo[idxPokebola] = 1; // Gengar
        m_revelos[idxPokebola].setTexture(m_texGengar, true);
        m_txtPregunta.setString("Incorrecto. (ENTER para continuar)");
    }

    // Revelo centrado en la bola elegida
    const sf::Vector2f c = m_bolas[idxPokebola].getPosition();
    m_revelos[idxPokebola].setPosition(c);
    m_revelos[idxPokebola].setColor(sf::Color(255,255,255,255));

    m_fase = Fase::Feedback;
}

void MinijuegoPolitica::siguientePregunta() {
    ++m_idx;
    mostrarPregunta(m_idx);
}

void MinijuegoPolitica::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { window.close(); return; }

        if (auto* kb = ev->getIf<sf::Event::KeyPressed>()) {
            if (kb->code == sf::Keyboard::Key::Enter) {
                if (m_fase == Fase::Feedback) { siguientePregunta(); return; }
                if (m_fase == Fase::Fin)      { salir(); return; }
            }
        }

        if (auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button == sf::Mouse::Button::Left && m_fase == Fase::Pregunta) {
                const sf::Vector2i mpix = sf::Mouse::getPosition(window);
                // Usa la vista ACTUAL del window (por si no es la default)
                const sf::Vector2f mpos = window.mapPixelToCoords(mpix, window.getView());
                for (int i = 0; i < 4; ++i) {
                    if (m_bolas[i].getGlobalBounds().contains(mpos)) {
                        resolverClick(i);
                        break;
                    }
                }
            }
        }
    }
}

void MinijuegoPolitica::actualizar() {
    const sf::Vector2i mpix = sf::Mouse::getPosition(m_window);
    const sf::Vector2f mpos = m_window.mapPixelToCoords(mpix, m_window.getView());

    for (int i = 0; i < 4; ++i) {
        bool over = (m_fase == Fase::Pregunta) && m_bolas[i].getGlobalBounds().contains(mpos);
        float hover = over ? 1.08f : 1.0f;

        m_bolas[i].setScale({ m_scaleBola * hover, m_scaleBola * hover });

        // Origen centrado → basta con “pegar” letra y revelo al centro
        const sf::Vector2f c = m_bolas[i].getPosition();
        m_txtLetra[i].setPosition(c);
        m_revelos[i].setPosition(c);
    }
}

void MinijuegoPolitica::dibujar(sf::RenderWindow& window) {
    window.setView(window.getDefaultView());
    window.draw(m_sprFondo);
    window.draw(m_sprDialog);

    window.draw(m_txtPregunta);
    for (int i=0;i<4;++i) window.draw(m_txtOpc[i]);

    for (int i=0;i<4;++i) window.draw(m_bolas[i]);
    for (int i=0;i<4;++i) window.draw(m_txtLetra[i]);

    if (m_fase != Fase::Pregunta) {
        for (int i=0;i<4;++i) if (m_overlayTipo[i] != -1) window.draw(m_revelos[i]);
    }
}

void MinijuegoPolitica::salir() {
    gestor->sacarEstado();
    personaje.setPosition(60, 400);
}
