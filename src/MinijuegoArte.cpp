#include "MinijuegoArte.h"
#include <SFML/Config.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>

static bool containsPoint(const sf::FloatRect& r, const sf::Vector2f& p) {
    return (p.x >= r.position.x) && (p.x <= r.position.x + r.size.x) &&
           (p.y >= r.position.y)  && (p.y <= r.position.y  + r.size.y);
}

MinijuegoArte::MinijuegoArte(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje)
: Estado(gestor, personaje), m_window(window), m_personaje(personaje), m_txtPregunta(m_font),
  m_txtOpcA(m_font), m_txtOpcB(m_font), m_txtOpcC(m_font), m_txtOpcD(m_font), m_bgm(m_bufBgm), m_sndAdvance(m_bufAdvance), m_sndVictory(m_bufVictory),
  m_sprFondo(m_texFondo), m_sprDialog(m_texDialog), m_sprPika(m_texPika), m_sprChari(m_texChari),
  m_btnA(m_texBtnA), m_btnB(m_texBtnB), m_btnC(m_texBtnC), m_btnD(m_texBtnD), m_sprLife(m_texLife[5]){
    cargarAssets();
    armarPreguntas();
    dibujar(m_window);
}

void MinijuegoArte::cargarAssets() {
    // === Carga de assets ===
    auto load = [&](sf::Texture& t, const std::string& path){
        if (!t.loadFromFile(path)) std::cerr << "Falta " << path << "\n";
    };
    load(m_texFondo, "assets/MinijuegoArte/escenario.png");
    load(m_texDialog, "assets/MinijuegoArte/dialog.png");
    load(m_texPika, "assets/MinijuegoArte/pikachu.png");
    load(m_texChari, "assets/MinijuegoArte/charizard.png");
    load(m_texBtnA, "assets/MinijuegoArte/botonA.png");
    load(m_texBtnB, "assets/MinijuegoArte/botonB.png");
    load(m_texBtnC, "assets/MinijuegoArte/botonC.png");
    load(m_texBtnD, "assets/MinijuegoArte/botonD.png");

    for (int i = 0; i <= 5; ++i)
        load(m_texLife[i], "assets/MinijuegoArte/life" + std::to_string(i) + ".png");

    if (!m_font.openFromFile("assets/Pokemon_GB.ttf"))
        std::cerr << "No se pudo cargar fuente\n";

    // === Fondo ===
    m_sprFondo.setTexture(m_texFondo, true);
    m_sprFondo.setPosition({0.f, 0.f});
    sf::Vector2u ws = m_window.getSize();
    sf::Vector2u ts = m_texFondo.getSize();
    if (ts.x && ts.y) {
        float scaleX = float(ws.x) / ts.x;
        float targetHeight = 880.f; // Altura deseada en píxeles
        float scaleY = targetHeight / ts.y;
        m_sprFondo.setScale({scaleX, scaleY});
    }

    // === Diálogo ===
    m_sprDialog.setTexture(m_texDialog, true);
    m_sprDialog.setScale({12.0f, 2.2f});
    m_sprDialog.setPosition({0.f, 0.f});

    // === Pokémon ===
    m_sprPika.setTexture(m_texPika, true);
    m_sprPika.setPosition({200.f, 525.f});
    m_pikaBase = m_sprPika.getPosition();

    m_sprChari.setTexture(m_texChari, true);
    m_sprChari.setPosition({1200.f, 140.f});
    m_chariBase = m_sprChari.getPosition();

    // === Vida ===
    m_sprLife.setTexture(m_texLife[5], true);
    m_sprLife.setPosition({200.f, 600.f});

    // === Botones ===
    float baseY = 880.f;
    float spaceX = 505.f;
    m_btnA.setTexture(m_texBtnA, true); m_btnA.setPosition({0.f, baseY});
    m_btnB.setTexture(m_texBtnB, true); m_btnB.setPosition({0.f + spaceX, baseY});
    m_btnC.setTexture(m_texBtnC, true); m_btnC.setPosition({0.f + spaceX * 2, baseY});
    m_btnD.setTexture(m_texBtnD, true); m_btnD.setPosition({0.f + spaceX * 3, baseY});

    // === Textos ===
    auto setupText = [&](sf::Text& t){
        t.setFont(m_font);
        t.setCharacterSize(25);
        t.setFillColor(sf::Color::Black);
        t.setScale({0.8f, 1.0f});
    };
    setupText(m_txtPregunta);
    setupText(m_txtOpcA);
    setupText(m_txtOpcB);
    setupText(m_txtOpcC);
    setupText(m_txtOpcD);

    m_txtPregunta.setPosition({100.f, 40.f});

    // Centrar los textos sobre cada botón
    m_txtOpcA.setPosition({m_btnA.getPosition().x + 40.f, baseY + 85.f});
    m_txtOpcB.setPosition({m_btnB.getPosition().x + 40.f, baseY + 85.f});
    m_txtOpcC.setPosition({m_btnC.getPosition().x + 40.f, baseY + 85.f});
    m_txtOpcD.setPosition({m_btnD.getPosition().x + 40.f, baseY + 85.f});

    if(m_bufBgm.loadFromFile("assets/MinijuegoArte/bgm.mp3")){
        m_bgm.setBuffer(m_bufBgm);
        m_bgm.setLooping(false);
        m_bgm.setVolume(100.f);
    }

    if (m_bufAdvance.loadFromFile("assets/MinijuegoArte/advance.mp3")) {
        m_sndAdvance.setBuffer(m_bufAdvance);
        m_sndAdvance.setVolume(100.f);
    }

    if(m_bufVictory.loadFromFile("assets/MinijuegoArte/victory.mp3")) {
        m_sndVictory.setBuffer(m_bufVictory);
        m_sndVictory.setVolume(100.f);
    }
}

void MinijuegoArte::armarPreguntas() {
    m_preguntas.clear();
    m_preguntas.push_back({"Uno de los siguientes personajes fue el encargado de pintar la capilla Sixtina:",
        {"Miguel Angel","Donatello","Leonardo Da\n\nVinci","Francis Bacon"}, 0});
    m_preguntas.push_back({"Genio del renacimiento que esculpio el Moises, el David y la Pieta:",
        {"Miguel Angel\n\nBuonarroti","Leonardo Da\n\nVinci","Rafael Sanzio","Galileo Galilei"}, 0});
    m_preguntas.push_back({"Durante el renacimiento el estilo artistico que impregno el arte, la filosofia, la pintura,\nla escritura fue el:",
        {"El Gotico","El Barroco","El Clasicismo","EL Romanticismo"}, 1});
    m_preguntas.push_back({"Durante el renacimiento surge una nueva vision del hombre, que se vio reflejada en el arte,\nen la politica y en las ciencias sociales y humanas, a lo que se denomina:",
        {"Antropocentrismo","Humanismo","Paradigma antropologico","Teocentrismo"}, 1});
    m_preguntas.push_back({"Cuatro genios del renacimiento (Leonardo, Donatello, Rafael y Michelangelo) han sido\nllevados a la pantalla en los comics de:",
        {"Las tortugas ninjas","Los caballeros del Zodiaco","Los cuatro fantasticos","Los antagonistas de Attack Titan"}, 0});
}

void MinijuegoArte::iniciarCombate() {
    startBGM();
    m_window.setView(m_window.getDefaultView());

    m_estado = EstadoCombate::Jugando;
    m_vidaJugador = 5;
    m_idxPregunta = 0;

    m_sprFondo.setPosition({0.f, 0.f});

    mostrarPregunta(m_idxPregunta);
}

void MinijuegoArte::mostrarPregunta(int idx) {
    if (idx < 0 || idx >= static_cast<int>(m_preguntas.size())) {
        m_estado = EstadoCombate::Gano;
        stopBGM();
        onWin();
        m_subestado = Subestado::Feedback;
        if (m_estado == EstadoCombate::Gano || m_estado == EstadoCombate::Perdio) {
            if(m_vidaJugador == 5) {
                m_txtPregunta.setString("¡Felicidades! Has ganado una vida extra... (Presiona ENTER)");
            } else {
                m_txtPregunta.setString("¡Felicidades! Has ganado... (Presiona ENTER)");

            }
        }

        m_subestado = Subestado::Feedback;
        idxSeleccion = -1;
        return;
    }

    const auto& p = m_preguntas[idx];
    m_txtPregunta.setString(p.enunciado);
    m_txtOpcA.setString(p.opciones[0]);
    m_txtOpcB.setString(p.opciones[1]);
    m_txtOpcC.setString(p.opciones[2]);
    m_txtOpcD.setString(p.opciones[3]);
    m_respuestaCorrecta = p.correcta;

    m_subestado    = Subestado::Pregunta;
    idxSeleccion = -1;

    // Reinicia escalas (por si quedó un hover aplicado)
    m_btnA.setScale({1.f,1.f}); m_btnB.setScale({1.f,1.f});
    m_btnC.setScale({1.f,1.f}); m_btnD.setScale({1.f,1.f});
    m_txtOpcA.setScale({1.f,1.f}); m_txtOpcB.setScale({1.f,1.f});
    m_txtOpcC.setScale({1.f,1.f}); m_txtOpcD.setScale({1.f,1.f});

    // Reset de temblores y posiciones base
    m_sprPika.setPosition(m_pikaBase);
    m_sprChari.setPosition(m_chariBase);
    m_shakePika = m_shakeChari = 0.f;
}

void MinijuegoArte::siguientePregunta() {
    ++m_idxPregunta;
    mostrarPregunta(m_idxPregunta);
}

void MinijuegoArte::procesarRespuesta(int idxOpcion) {
    if (m_estado != EstadoCombate::Jugando || m_subestado != Subestado::Pregunta) return;

    idxSeleccion = idxOpcion;
    bool ok = (idxOpcion == m_respuestaCorrecta);

    m_sndAdvance.play();

    if (ok) {
        m_txtPregunta.setString("¡Correcto! PIKACHU uso Impactrueno... (Presiona ENTER)");
        m_shakeChari = 0.45f;
    } else {
        m_txtPregunta.setString("¡Incorrecto! CHARIZARD uso Lanzallamas... (Presiona ENTER)");
        m_vidaJugador = std::max(0, m_vidaJugador - 1);
        m_sprLife.setTexture(m_texLife[m_vidaJugador]);
        m_shakePika = 0.45f;               // sacudida en Pikachu
        if (m_vidaJugador == 0) {
            m_estado = EstadoCombate::Perdio;
            onLose();
        }
    }

    m_subestado = Subestado::Feedback;
}

void MinijuegoArte::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { window.close(); return; }

        if (auto* kb = ev->getIf<sf::Event::KeyPressed>()) {
            if (kb->code == sf::Keyboard::Key::Enter) {
                if (m_subestado == Subestado::Feedback) {
                    if (m_estado == EstadoCombate::Jugando) {
                        siguientePregunta();
                        return;
                    }

                    if (m_estado == EstadoCombate::Gano || m_estado == EstadoCombate::Perdio) {
                        salirDelMinijuego();
                        return;
                    }
                }
            }
        }

        if (auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button == sf::Mouse::Button::Left) {
                if (m_subestado != Subestado::Pregunta) continue; // bloqueado durante feedback
                const sf::Vector2i mposI = sf::Mouse::getPosition(window);
                const sf::Vector2f mpos  = { float(mposI.x), float(mposI.y) };

                if (containsPoint(m_btnA.getGlobalBounds(), mpos)) { procesarRespuesta(0); }
                else if (containsPoint(m_btnB.getGlobalBounds(), mpos)) { procesarRespuesta(1); }
                else if (containsPoint(m_btnC.getGlobalBounds(), mpos)) { procesarRespuesta(2); }
                else if (containsPoint(m_btnD.getGlobalBounds(), mpos)) { procesarRespuesta(3); }
            }
        }
    }
}

void MinijuegoArte::actualizar() {
    // dt
    float dt = m_clock.restart().asSeconds();

    // --- SHAKES ---
    auto applyShake = [&](sf::Sprite& spr, sf::Vector2f base, float& timer){
        if (timer > 0.f) {
            timer -= dt;
            float t = std::max(0.f, timer);
            float amp = 6.f * (t / 0.45f);                // decae hacia 0
            float ox = std::sin(50.f * (0.45f - t)) * amp;
            float oy = std::cos(60.f * (0.45f - t)) * amp * 0.5f;
            spr.setPosition(base + sf::Vector2f{ox, oy});
            if (timer <= 0.f) spr.setPosition(base);
        }
    };
    applyShake(m_sprPika,  m_pikaBase,  m_shakePika);
    applyShake(m_sprChari, m_chariBase, m_shakeChari);

    // --- HOVER: escala botón + texto a la vez ---
    const sf::Vector2i mposI = sf::Mouse::getPosition(m_window);
    const sf::Vector2f mpos  = { float(mposI.x), float(mposI.y) };

    auto hoverPair = [&](sf::Sprite& btn, sf::Text& txt){
        bool over = containsPoint(btn.getGlobalBounds(), mpos) && (m_subestado == Subestado::Pregunta);
        float s = over ? 1.08f : 1.0f;
        btn.setScale({s, s});

        // Aumentar también el texto (escala) y compensar posición para que se vea centrado
        txt.setScale({s, s});
        // Recalibrar: mueve un poco a la izquierda/arriba cuando crece
        // (ajuste fino para tus botones; si cambias gráficos, quizá quieras tocar estos 2 px)
        /*sf::Vector2f base = txt.getPosition();
        if (over) txt.setPosition({base.x - 6.f, base.y - 4.f});
        else      txt.setPosition({base.x + 6.f, base.y + 4.f}); */// vuelve al sitio original
    };

    hoverPair(m_btnA, m_txtOpcA);
    hoverPair(m_btnB, m_txtOpcB);
    hoverPair(m_btnC, m_txtOpcC);
    hoverPair(m_btnD, m_txtOpcD);
}

void MinijuegoArte::dibujar(sf::RenderWindow& window) {
    window.setView(window.getDefaultView());
    window.draw(m_sprFondo);
    window.draw(m_sprPika);
    window.draw(m_sprChari);

    window.draw(m_sprDialog);
    window.draw(m_btnA); window.draw(m_btnB);
    window.draw(m_btnC); window.draw(m_btnD);

    window.draw(m_txtPregunta);
    window.draw(m_txtOpcA); window.draw(m_txtOpcB);
    window.draw(m_txtOpcC); window.draw(m_txtOpcD);
}

void MinijuegoArte::startBGM() {
    if(!m_bgm.isLooping()) {
        m_bgm.play();
        m_bgm.setLooping(true);
    }
}

void MinijuegoArte::stopBGM() {
    if(m_bgm.isLooping()) {
        m_bgm.stop();
        m_bgm.setLooping(false);
    }
}

void MinijuegoArte::onWin() {
    stopBGM();
    m_sndVictory.play();
    m_txtPregunta.setString("Has ganado... (Presiona ENTER para salir)");
    m_sprChari.setColor(sf::Color(255,255,255,0));
}

void MinijuegoArte::salirDelMinijuego() {
    m_bgm.stop();
    gestor->sacarEstado();
    personaje.setPosition(60, 400);
}

void MinijuegoArte::onLose() {
    stopBGM();
    m_txtPregunta.setString("Has perdido... (Presiona ENTER para salir)");
}
