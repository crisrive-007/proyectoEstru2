#include "MinijuegoArte.h"
#include <SFML/Graphics/Rect.hpp>
#include <cassert>
#include <algorithm>

static sf::IntRect hpFrame(const sf::Texture& tex, int frames, int idx) {
    auto sz = tex.getSize();
    int w = static_cast<int>(sz.x) / frames;
    int h = static_cast<int>(sz.y);
    return sf::IntRect({idx * w, 0}, {w, h});
}

MinijuegoArte::MinijuegoArte(sf::RenderWindow& window) : m_window(window),
                                                         m_texBg(), m_bg(m_texBg),
                                                         m_texDialog(), m_dialog(m_texDialog),
                                                         m_texPika(), m_pika(m_texPika),
                                                         m_texChari(), m_chari(m_texChari),
                                                         m_texBtnA(), m_btnA(m_texBtnA),
                                                         m_texBtnB(), m_btnB(m_texBtnB),
                                                         m_texBtnC(), m_btnC(m_texBtnC),
                                                         m_texBtnD(), m_btnD(m_texBtnD),
                                                         m_texHP(), m_hpPlayer(m_texHP), m_hpEnemy(m_texHP),

                                                         m_font(),
                                                         m_txtPregunta(m_font),
                                                         m_txtA(m_font),
                                                         m_txtB(m_font),
                                                         m_txtC(m_font),
                                                         m_txtD(m_font),
                                                         m_txtFeedback(m_font){
    cargarAssets();
    armarPreguntas();
    reset();
}

void MinijuegoArte::cargarAssets() {
    if (!m_texBg.loadFromFile("assets/Minijuego1/escenario.png"))      throw std::runtime_error("escenario.png");
    if (!m_texDialog.loadFromFile("assets/Minijuego1/dialog.png"))     throw std::runtime_error("dialog.png");
    if (!m_texPika.loadFromFile("assets/Minijuego1/pikachu.png"))      throw std::runtime_error("pikachu.png");
    if (!m_texChari.loadFromFile("assets/Minijuego1/charizard.png"))   throw std::runtime_error("charizard.png");

    if (!m_texBtnA.loadFromFile("assets/Minijuego1/botonA.png"))       throw std::runtime_error("botonA.png");
    if (!m_texBtnB.loadFromFile("assets/Minijuego1/botonB.png"))       throw std::runtime_error("botonB.png");
    if (!m_texBtnC.loadFromFile("assets/Minijuego1/botonC.png"))       throw std::runtime_error("botonC.png");
    if (!m_texBtnD.loadFromFile("assets/Minijuego1/botonD.png"))       throw std::runtime_error("botonD.png");

    if (!m_texHP.loadFromFile("assets/Minijuego1/life.png"))           throw std::runtime_error("life.png");

    if (!m_font.openFromFile("assets/Pokemon_GB.ttf"))      throw std::runtime_error("Pokemon_GB.ttf");

    m_bg.setTexture(m_texBg);
    m_dialog.setTexture(m_texDialog);
    m_dialog.setPosition(sf::Vector2f(24.f, 16.f));

    m_pika.setTexture(m_texPika);
    m_pika.setPosition(sf::Vector2f(120.f, 360.f));

    m_chari.setTexture(m_texChari);
    m_chari.setPosition(sf::Vector2f(700.f, 160.f));

    m_btnA.setTexture(m_texBtnA);  m_btnA.setPosition(sf::Vector2f(60.f, 560.f));
    m_btnB.setTexture(m_texBtnB);  m_btnB.setPosition(sf::Vector2f(360.f, 560.f));
    m_btnC.setTexture(m_texBtnC);  m_btnC.setPosition(sf::Vector2f(660.f, 560.f));
    m_btnD.setTexture(m_texBtnD);  m_btnD.setPosition(sf::Vector2f(960.f, 560.f));

    m_hpPlayer.setTexture(m_texHP);
    m_hpEnemy.setTexture(m_texHP);
    m_hpPlayer.setTextureRect(hpFrame(m_texHP, m_hpFrames, 0));
    m_hpEnemy .setTextureRect(hpFrame(m_texHP, m_hpFrames, 0));
    m_hpPlayer.setPosition(sf::Vector2f(180.f, 320.f));
    m_hpEnemy .setPosition(sf::Vector2f(760.f, 90.f));

    auto makeText = [&](sf::Text& t, unsigned size){
        t.setFont(m_font);
        t.setCharacterSize(size);
        t.setFillColor(sf::Color::Black);
    };
    makeText(m_txtPregunta, 26);
    makeText(m_txtA, 22); makeText(m_txtB, 22); makeText(m_txtC, 22); makeText(m_txtD, 22);
    makeText(m_txtFeedback, 26);

    m_rectLineaA = {{m_btnA.getPosition().x + 46.f, m_btnA.getPosition().y + 44.f}, {250.f, 28.f} };
    m_rectLineaB = {{m_btnB.getPosition().x + 46.f, m_btnB.getPosition().y + 44.f}, {250.f, 28.f }};
    m_rectLineaC = {{m_btnC.getPosition().x + 46.f, m_btnC.getPosition().y + 44.f}, {250.f, 28.f }};
    m_rectLineaD = {{m_btnD.getPosition().x + 46.f, m_btnD.getPosition().y + 44.f}, {250.f, 28.f }};
    m_rectDialog = {{m_dialog.getPosition().x + 28.f, m_dialog.getPosition().y + 10.f}, {1030.f, 40.f }};
}

void MinijuegoArte::armarPreguntas() {
    std::queue<Pregunta> q;
    q.push({"Uno de los siguientes personajes fue el encargado de pintar la capilla Sixtina:", {"Miguel Ángel","Donatello","Leonardo Da Vinci","Francis Bacon"}, 0});
    m_queue = std::move(q);
}

void MinijuegoArte::iniciarCombate() {
    reset();
}

void MinijuegoArte::reset() {
    while(!m_queue.empty()) m_queue.pop();
    armarPreguntas();
    m_idxHPPlayer = 0;
    m_idxHPEnemy = 0;
    m_estado = Estado::Jugando;
    m_bloquearInput = false;
    m_timerFeedback = 0.f;
    m_hpPlayer.setTextureRect(hpFrame(m_texHP, m_hpFrames, m_idxHPPlayer));
    m_hpEnemy .setTextureRect(hpFrame(m_texHP, m_hpFrames, m_idxHPEnemy));
    siguientePregunta();
}

void MinijuegoArte::siguientePregunta() {
    if (m_queue.empty()) { m_estado = Estado::Gano; m_actual.reset(); return; }
    m_actual = m_queue.front(); m_queue.pop();
    mostrarPreguntaActual();
}

void MinijuegoArte::mostrarPreguntaActual() {
    assert(m_actual.has_value());

    // 1) Actualiza textos
    m_txtPregunta.setString(m_actual->pregunta);
    m_txtA.setString(m_actual->opciones[0]);
    m_txtB.setString(m_actual->opciones[1]);
    m_txtC.setString(m_actual->opciones[2]);
    m_txtD.setString(m_actual->opciones[3]);

    // 2) Helper: centrar texto dentro de un FloatRect usando sus campos
    auto centerIn = [](sf::Text& t, const sf::FloatRect& r) {
        // Centro del rect destino
        const sf::Vector2f center{ r.position.x + r.size.x * 0.5f, r.position.y + r.size.y * 0.5f };

        // Bounds locales del texto (considera offset del glyph)
        const sf::FloatRect b = t.getLocalBounds();

        // Centrar el origen en el texto
        t.setOrigin({b.position.x + b.size.x * 0.5f, b.position.y + b.size.y * 0.5f});

        // Posicionar el texto en el centro del rect
        t.setPosition(center);
    };

    // 3) Centrar cada elemento en su rectángulo correspondiente
    centerIn(m_txtPregunta, m_rectDialog);
    centerIn(m_txtA,        m_rectLineaA);
    centerIn(m_txtB,        m_rectLineaB);
    centerIn(m_txtC,        m_rectLineaC);
    centerIn(m_txtD,        m_rectLineaD);
}


void MinijuegoArte::manejarEventos(const sf::Event& ev) {
    if (m_estado != Estado::Jugando || m_bloquearInput) return;

    if (const auto* mouseEvent = ev.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f m(mouseEvent->position.x, mouseEvent->position.y);
            chequearClick(m);
        }
    }

    if (const auto* keyEvent = ev.getIf<sf::Event::KeyPressed>()) {
        if      (keyEvent->code == sf::Keyboard::Key::A) procesarRespuesta(0);
        else if (keyEvent->code == sf::Keyboard::Key::B) procesarRespuesta(1);
        else if (keyEvent->code == sf::Keyboard::Key::C) procesarRespuesta(2);
        else if (keyEvent->code == sf::Keyboard::Key::D) procesarRespuesta(3);
    }
}

void MinijuegoArte::chequearClick(const sf::Vector2f& p) {
    auto hit = [&](const sf::Sprite& s){ return s.getGlobalBounds().contains(p); };
    if (hit(m_btnA)) { procesarRespuesta(0); return; }
    if (hit(m_btnB)) { procesarRespuesta(1); return; }
    if (hit(m_btnC)) { procesarRespuesta(2); return; }
    if (hit(m_btnD)) { procesarRespuesta(3); return; }
}

void MinijuegoArte::procesarRespuesta(int idx) {
    if(!m_actual) return;

    bool ok = (idx == m_actual->correcta);

    if (ok) {
        m_idxHPEnemy = std::min(m_hpFrames-1, m_idxHPEnemy+1);
        m_hpEnemy.setTextureRect(hpFrame(m_texHP, m_hpFrames, m_idxHPEnemy));
        m_txtFeedback.setString("¡Ataque efectivo!");
    } else {
        m_idxHPPlayer = std::min(m_hpFrames-1, m_idxHPPlayer+1);
        m_hpPlayer.setTextureRect(hpFrame(m_texHP, m_hpFrames, m_idxHPPlayer));
        m_txtFeedback.setString("¡Fallaste! Recibes daño.");
    }

    m_txtFeedback.setFont(m_font);
    m_txtFeedback.setCharacterSize(26);
    m_txtFeedback.setFillColor(sf::Color::Black);
    auto b = m_txtFeedback.getLocalBounds();
    m_txtFeedback.setOrigin({b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f});
    m_txtFeedback.setPosition({m_rectDialog.position.x + m_rectDialog.size.x/2.f, m_rectDialog.position.y + m_rectDialog.size.y/2.f});

    m_bloquearInput = true;
    m_timerFeedback = 0.f;

    if (m_idxHPEnemy == m_hpFrames-1) { m_estado = Estado::Gano; }
    else if (m_idxHPPlayer == m_hpFrames-1) { m_estado = Estado::Perdio; }
}

void MinijuegoArte::actualizar(float dt) {
    if (m_bloquearInput) {
        m_timerFeedback += dt;
        if (m_timerFeedback >= 0.9f) {
            m_bloquearInput = false;
            m_timerFeedback = 0.f;
            if (m_estado == Estado::Jugando) siguientePregunta();
        }
    }
}

void MinijuegoArte::dibujar() {
    m_window.draw(m_bg);
    m_window.draw(m_pika);
    m_window.draw(m_chari);

    m_window.draw(m_hpPlayer);
    m_window.draw(m_hpEnemy);

    m_window.draw(m_dialog);
    if (m_bloquearInput) m_window.draw(m_txtFeedback);
    else                 m_window.draw(m_txtPregunta);

    m_window.draw(m_btnA); m_window.draw(m_btnB);
    m_window.draw(m_btnC); m_window.draw(m_btnD);
    m_window.draw(m_txtA); m_window.draw(m_txtB);
    m_window.draw(m_txtC); m_window.draw(m_txtD);
}

bool MinijuegoArte::terminado() const { return m_estado != Estado::Jugando; }
MinijuegoArte::Estado MinijuegoArte::estado() const { return m_estado; }
