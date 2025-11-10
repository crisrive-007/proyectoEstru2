#include "Combate.h"
#include <SFML/Config.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>

static bool containsPoint(const sf::FloatRect& r, const sf::Vector2f& p) {
    return (p.x >= r.position.x) && (p.x <= r.position.x + r.size.x) &&
           (p.y >= r.position.y)  && (p.y <= r.position.y  + r.size.y);
}

Combate::Combate(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje)
: Estado(gestor, personaje), m_window(window), m_personaje(personaje), m_txtPregunta(m_font),
  m_txtOpcA(m_font), m_txtOpcB(m_font), m_txtOpcC(m_font), m_txtOpcD(m_font), m_bgm(m_bufBgm), m_sndAdvance(m_bufAdvance), m_sndVictory(m_bufVictory),
  m_sprFondo(m_texFondo), m_sprDialog(m_texDialog), m_sprPika(m_texPika), m_sprChari(m_texChari),
  m_btnA(m_texBtnA), m_btnB(m_texBtnB), m_btnC(m_texBtnC), m_btnD(m_texBtnD), m_sprLife(m_texLife[5]),
  m_txtNombreJ(m_font), m_txtNombreN(m_font), m_lblTurno(m_font),
  m_sndEmpirista(m_bufEmpirista), m_sndRacionalista(m_bufRacionalista){
    cargarAssets();
    armarPreguntas();
    dibujar(m_window);
}

void Combate::cargarAssets() {
    // === Carga de assets ===
    auto load = [&](sf::Texture& t, const std::string& path){
        if (!t.loadFromFile(path)) std::cerr << "Falta " << path << "\n";
    };
    load(m_texFondo, "assets/Combate/escenario.png");
    load(m_texDialog, "assets/Combate/dialog.png");
    load(m_texPika, "assets/Combate/pikachu.png");
    load(m_texChari, "assets/Combate/charizard.png");
    load(m_texBtnA, "assets/Combate/botonA.png");
    load(m_texBtnB, "assets/Combate/botonB.png");
    load(m_texBtnC, "assets/Combate/botonC.png");
    load(m_texBtnD, "assets/Combate/botonD.png");

    for (int i = 0; i <= 5; ++i)
        load(m_texLife[i], "assets/Combate/life" + std::to_string(i) + ".png");

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

    if(m_bufBgm.loadFromFile("assets/Combate/bgm.mp3")){
        m_bgm.setBuffer(m_bufBgm);
        m_bgm.setLooping(false);
        m_bgm.setVolume(100.f);
    }

    if (m_bufAdvance.loadFromFile("assets/Combate/advance.mp3")) {
        m_sndAdvance.setBuffer(m_bufAdvance);
        m_sndAdvance.setVolume(100.f);
    }

    if(m_bufVictory.loadFromFile("assets/Combate/victory.mp3")) {
        m_sndVictory.setBuffer(m_bufVictory);
        m_sndVictory.setVolume(100.f);
    }

    m_fxPika.load("assets/Combate/efectoPikachu.png", 5);
    m_fxChari.load("assets/Combate/efectoCharizard.png", 5);

    // === HUD JUGADOR ===
    m_hudBoxJ.setSize({250.f, 80.f});
    m_hudBoxJ.setPosition({40.f, 370.f});
    m_hudBoxJ.setFillColor(sf::Color(0, 0, 0, 120));
    m_hudBoxJ.setOutlineColor(sf::Color::White);
    m_hudBoxJ.setOutlineThickness(2.f);

    if (!m_fontHud.openFromFile("assets/Pokemon_GB.ttf")) {
        std::cerr << "✗ ERROR: assets/Pokemon_GB.ttf\n";
    }
    m_txtNombreJ.setFont(m_fontHud);
    m_txtNombreJ.setCharacterSize(20);
    m_txtNombreJ.setFillColor(sf::Color::White);
    m_txtNombreJ.setOutlineColor(sf::Color::Black);
    m_txtNombreJ.setOutlineThickness(2.f);
    m_txtNombreJ.setPosition(m_hudBoxJ.getPosition() + sf::Vector2f{15.f, 12.f});
    m_txtNombreJ.setString(m_personaje.getNombre()); // mismo que en Gimnasio

    if (!m_texCorazon.loadFromFile("assets/vidas.png")) {
        std::cerr << "⚠ No se pudo cargar assets/vidas.png\n";
    }

    // === HUD NPC === (espejo, arriba derecha)
    m_hudBoxN.setSize({250.f, 80.f});
    const float pad = 20.f;
    const auto vw = m_window.getSize().x;
    m_hudBoxN.setPosition({960.f, 150.f});
    m_hudBoxN.setFillColor(sf::Color(0, 0, 0, 120));
    m_hudBoxN.setOutlineColor(sf::Color::White);
    m_hudBoxN.setOutlineThickness(2.f);

    m_txtNombreN.setFont(m_fontHud);
    m_txtNombreN.setCharacterSize(20);
    m_txtNombreN.setFillColor(sf::Color::White);
    m_txtNombreN.setOutlineColor(sf::Color::Black);
    m_txtNombreN.setOutlineThickness(2.f);
    m_txtNombreN.setPosition(m_hudBoxN.getPosition() + sf::Vector2f{15.f, 12.f});
    m_txtNombreN.setString("NPC");

    // Etiqueta de turno
    m_lblTurno.setFont(m_fontHud);           // misma fuente que ya cargas para HUD
    m_lblTurno.setCharacterSize(22);
    m_lblTurno.setFillColor(sf::Color::White);
    m_lblTurno.setOutlineColor(sf::Color::Black);
    m_lblTurno.setOutlineThickness(2.f);
    m_lblTurno.setPosition({20.f, 110.f});   // debajo del HUD del jugador
    setLeyendaTurno();                       // inicializa el texto según m_turnoActual

    // Resalto de opción del NPC (un rectángulo semitransparente)
    m_resaltoNPC.setFillColor(sf::Color(255, 255, 255, 40));
    m_resaltoNPC.setOutlineColor(sf::Color::White);
    m_resaltoNPC.setOutlineThickness(2.f);
    m_npcOpcion = -1;

    if (m_bufEmpirista.loadFromFile("assets/Combate/empirista.wav")) {
        m_sndEmpirista.setBuffer(m_bufEmpirista);
        m_sndEmpirista.setVolume(100.f);
    } else {
        std::cerr << "⚠ No se pudo cargar empirista.mp3\n";
    }

    if (m_bufRacionalista.loadFromFile("assets/Combate/racionalista.mp3")) {
        m_sndRacionalista.setBuffer(m_bufRacionalista);
        m_sndRacionalista.setVolume(100.f);
    } else {
        std::cerr << "⚠ No se pudo cargar racionalista.mp3\n";
    }
}

void Combate::armarPreguntas() {
    m_preguntas.clear();
    m_preguntas.push_back({"Para algunos de los siguientes filosofos, el criterio de verdad es la evidencia sensible:",
        {"Empiristas","Criticistas","Racionalistas","Dogmaticos"}, 0});
    m_preguntas.push_back({"De las siguientes, una de ellas es la corriente filosofica que en general tiende a negar\nla posibilidad de la metafisica y a sostener que hay conocimiento unicamente de los fenomenos.",
        {"Racionalistas","Empiristas","Escolasticos","Escepticos"}, 1});
    m_preguntas.push_back({"Para unos de los siguientes filosofos, la experiencia como unica fuente del conocimiento.",
        {"Epistemologos","Racionalistas","Empiristas","Escepticos"}, 2});
    m_preguntas.push_back({"Filosofos para quienes la unica fuente del conocimiento es la razon.",
        {"Epistemologos","Racionalistas","Empiristas","Escepticos"}, 1});
    m_preguntas.push_back({"Filosofos que postulan las ideas innatas en el sujeto.",
        {"Empiristas","Idealistas","Racionalistas","Innatistas"}, 2});
    m_preguntas.push_back({"De los siguientes filosofos selecciones el que no se considera Racionalista:",
        {"David Hume","John Locke","Nicolas Malebranch","Francis Bacon"}, 2});
    m_preguntas.push_back({"Es la doctrina que establece que todos nuestros conocimientos provienen de la razón.",
        {"Empirismo","Criticismo","Racionalismo","Epistemologia"}, 2});
    m_preguntas.push_back({"Uno de los siguientes filosofos, postula las ideas innatas en el sujeto:",
        {"George Berkeley","David Hume","Leibniz","Hipatia"}, 2});
}

void Combate::iniciarCombate() {
    startBGM();
    m_window.setView(m_window.getDefaultView());

    m_estado = EstadoCombate::Jugando;
    m_vidaJugador = m_personaje.getVidas();
    m_vidaNpc = 4;

    m_vidasCacheJ = -1;
    m_vidasCacheN = -1;
    actualizarHUDJugador();
    actualizarHUDNpc();

    m_sprFondo.setPosition({0.f, 0.f});

    // clave:
    m_idxPregunta  = -1;              // empezamos "antes" de la 0
    m_proximoTurno = Turno::Jugador;  // abre el jugador
    avanzarTurno();                   // sube a 0 y muestra la 0
}

void Combate::mostrarPregunta(int idx) {
    if (m_preguntas.empty()) return; // por seguridad

    if (idx < 0 || idx >= static_cast<int>(m_preguntas.size())) {
        // En vez de terminar el combate, cicla el índice:
        m_idxPregunta = 0;
        idx = 0;
        // (Opcional: barajar preguntas aquí)
    }

    const auto& p = m_preguntas[idx];
    m_txtPregunta.setString(p.enunciado);
    m_txtOpcA.setString(p.opciones[0]);
    m_txtOpcB.setString(p.opciones[1]);
    m_txtOpcC.setString(p.opciones[2]);
    m_txtOpcD.setString(p.opciones[3]);
    m_respuestaCorrecta = p.correcta;

    m_subestado  = Subestado::Pregunta;
    idxSeleccion = -1;

    m_btnA.setScale({1.f,1.f}); m_btnB.setScale({1.f,1.f});
    m_btnC.setScale({1.f,1.f}); m_btnD.setScale({1.f,1.f});
    m_txtOpcA.setScale({1.f,1.f}); m_txtOpcB.setScale({1.f,1.f});
    m_txtOpcC.setScale({1.f,1.f}); m_txtOpcD.setScale({1.f,1.f});

    m_sprPika.setPosition(m_pikaBase);
    m_sprChari.setPosition(m_chariBase);
    m_shakePika = m_shakeChari = 0.f;
}

void Combate::siguientePregunta() {
    ++m_idxPregunta;
    mostrarPregunta(m_idxPregunta);
}

void Combate::procesarRespuesta(int idxOpcion) {
    if (m_estado != EstadoCombate::Jugando || m_subestado != Subestado::Pregunta) return;

    idxSeleccion = idxOpcion;
    bool ok = (idxOpcion == m_respuestaCorrecta);

    m_sndAdvance.play();

    if (ok) {
        m_txtPregunta.setString("¡Correcto! PIKACHU uso Impactrueno... (Presiona ENTER)");
        m_shakeChari = 0.45f;

        const sf::Vector2f hit = anchorOn(m_sprChari, m_anchorChari.x, m_anchorChari.y) + m_fxOffsetChari;
        m_fxPika.play(hit);

        m_vidaNpc = std::max(0, m_vidaNpc - 1);
        actualizarHUDNpc();
        if (m_vidaNpc == 0) {
            m_estado = EstadoCombate::Gano;
            onWin();
        }

    } else {
        m_txtPregunta.setString("¡Incorrecto! CHARIZARD uso Lanzallamas... (Presiona ENTER)");
        m_vidaJugador = std::max(0, m_vidaJugador - 1);
        actualizarHUDJugador();
        m_sprLife.setTexture(m_texLife[m_vidaJugador]);
        m_shakePika = 0.45f;

        const sf::Vector2f hit = anchorOn(m_sprPika, m_anchorPika.x, m_anchorPika.y) + m_fxOffsetPika;
        m_fxChari.play(hit);

        if (m_vidaJugador == 0) {
            m_estado = EstadoCombate::Perdio;
            onLose();
        }
    }

    m_subestado = Subestado::Feedback;
}

void Combate::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { window.close(); return; }

        if (auto* kb = ev->getIf<sf::Event::KeyPressed>()) {
            if (kb->code == sf::Keyboard::Key::Enter) {
                if (m_subestado == Subestado::Feedback) {
                    if (m_estado == EstadoCombate::Jugando) {
                        avanzarTurno();
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
                if (m_turnoActual != Turno::Jugador) continue;
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

void Combate::actualizar() {
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

    m_fxPika.update(dt);
    m_fxChari.update(dt);

    // --- Turno NPC temporizado ---
    if (m_estado == EstadoCombate::Jugando && m_turnoActual == Turno::Npc && m_subestado == Subestado::Pregunta) {
        if (m_npcWait > 0.f) {
            m_npcWait -= dt;
            if (m_npcWait <= 0.f) {
                turnoNpcAuto();
            }
        }
    }
}

void Combate::dibujar(sf::RenderWindow& window) {
    window.setView(window.getDefaultView());
    window.draw(m_sprFondo);
    window.draw(m_sprPika);
    window.draw(m_sprChari);

    m_fxPika.draw(window);
    m_fxChari.draw(window);

    window.draw(m_sprDialog);
    window.draw(m_btnA); window.draw(m_btnB);
    window.draw(m_btnC); window.draw(m_btnD);

    window.draw(m_hudBoxJ); window.draw(m_txtNombreJ);
    for (auto& s : m_corazonesJ) window.draw(s);
    window.draw(m_hudBoxN); window.draw(m_txtNombreN);
    for (auto& s : m_corazonesN) window.draw(s);

    window.draw(m_txtPregunta);
    window.draw(m_txtOpcA); window.draw(m_txtOpcB);
    window.draw(m_txtOpcC); window.draw(m_txtOpcD);

    dibujarResaltoNpc(window);

    window.draw(m_lblTurno);
}

void Combate::startBGM() {
    if(!m_bgm.isLooping()) {
        m_bgm.play();
        m_bgm.setLooping(true);
    }
}

void Combate::stopBGM() {
    if(m_bgm.isLooping()) {
        m_bgm.stop();
        m_bgm.setLooping(false);
    }
}

void Combate::onWin() {
    stopBGM();

    // Sonido según equipo del personaje
    if (m_personaje.getEquipo() == "Empirista") {
        m_sndEmpirista.play();
    } else if (m_personaje.getEquipo() == "Racionalista") {
        m_sndRacionalista.play();
    } else {
        // sonido genérico por si acaso
        m_sndVictory.play();
    }

    m_txtPregunta.setString("Has ganado... (Presiona ENTER para salir)");
    m_sprChari.setColor(sf::Color(255,255,255,0));
}

void Combate::salirDelMinijuego() {
    m_bgm.stop();
    gestor->sacarEstado();
}

void Combate::onLose() {
    stopBGM();

    // Sonido según equipo del personaje
    if (m_personaje.getEquipo() == "Empirista") {
        m_sndRacionalista.play();
    } else if (m_personaje.getEquipo() == "Racionalista") {
        m_sndEmpirista.play();
    } else {
        // sonido genérico por si acaso
        m_sndVictory.play();
    }
    m_txtPregunta.setString("Has perdido... (Presiona ENTER para salir)");
}

void Combate::iniciarRonda() {
    // Por pregunta: primero juega el jugador, luego el NPC
    m_turnos = std::queue<Turno>();
    m_turnos.push(Turno::Jugador);
    m_turnos.push(Turno::Npc);
}

void Combate::avanzarTurno() {
    if (m_estado != EstadoCombate::Jugando) return; // <- NUEVO

    m_turnoActual  = m_proximoTurno;
    m_proximoTurno = (m_turnoActual == Turno::Jugador) ? Turno::Npc : Turno::Jugador;

    m_idxPregunta += 1;
    if (m_idxPregunta >= static_cast<int>(m_preguntas.size())) {
        m_idxPregunta = 0; // cíclico, ver punto 1
    }

    mostrarPregunta(m_idxPregunta);

    m_subestado   = Subestado::Pregunta;
    idxSeleccion  = -1;
    m_npcOpcion   = -1;

    if (m_turnoActual == Turno::Npc) {
        m_npcWait = 3.f;
        m_lblTurno.setString("Turno del NPC (pensando...)");
    } else {
        m_lblTurno.setString("Tu turno");
    }
}

void Combate::procesarRespuestaTurno(int idxOpcion, Turno actor) {
    if (m_estado != EstadoCombate::Jugando || m_subestado != Subestado::Pregunta) return;

    idxSeleccion = idxOpcion;
    bool ok = (idxOpcion == m_respuestaCorrecta);
    m_sndAdvance.play();

    if (actor == Turno::Jugador) {
        if (ok) {
            m_txtPregunta.setString("¡Correcto! PIKACHU uso Impactrueno... (Presiona ENTER)");
            m_shakeChari = 0.45f;
            const sf::Vector2f hit = anchorOn(m_sprChari, m_anchorChari.x, m_anchorChari.y) + m_fxOffsetChari;
            m_fxPika.play(hit);

            // daño a NPC
            m_vidaNpc = std::max(0, m_vidaNpc - 1);
            actualizarHUDNpc();
            if (m_vidaNpc == 0) {
                m_estado = EstadoCombate::Gano;
                onWin();
            }
        } else {
            m_txtPregunta.setString("¡Incorrecto! CHARIZARD uso Lanzallamas... (Presiona ENTER)");
            m_vidaJugador = std::max(0, m_vidaJugador - 1);
            actualizarHUDJugador();
            m_sprLife.setTexture(m_texLife[m_vidaJugador]);
            m_shakePika = 0.45f;
            const sf::Vector2f hit = anchorOn(m_sprPika, m_anchorPika.x, m_anchorPika.y) + m_fxOffsetPika;
            m_fxChari.play(hit);
            if (m_vidaJugador == 0) {
                m_estado = EstadoCombate::Perdio;
                onLose();
            }
        }
    } else { // Turno::Npc
        if (ok) {
            m_txtPregunta.setString("¡El NPC acierta! CHARIZARD uso Lanzallamas... (Presiona ENTER)");
            m_vidaJugador = std::max(0, m_vidaJugador - 1);
            actualizarHUDJugador();
            m_sprLife.setTexture(m_texLife[m_vidaJugador]);
            m_shakePika = 0.45f;
            const sf::Vector2f hit = anchorOn(m_sprPika, m_anchorPika.x, m_anchorPika.y) + m_fxOffsetPika;
            m_fxChari.play(hit);
            if (m_vidaJugador == 0) {
                m_estado = EstadoCombate::Perdio;
                onLose();
            }
        } else {
            m_txtPregunta.setString("¡El NPC falla! PIKACHU contraataca... (Presiona ENTER)");
            m_vidaNpc = std::max(0, m_vidaNpc - 1);
            actualizarHUDNpc();
            m_shakeChari = 0.45f;
            const sf::Vector2f hit = anchorOn(m_sprChari, m_anchorChari.x, m_anchorChari.y) + m_fxOffsetChari;
            m_fxPika.play(hit);
            if (m_vidaNpc == 0) {
                m_estado = EstadoCombate::Gano;
                onWin();
            }
        }
    }

    m_subestado = Subestado::Feedback;
}

void Combate::turnoNpcAuto() {
    bool ok = (m_dist(m_rng) < 0.75f);
    int opcion = ok ? m_respuestaCorrecta
                    : ((m_respuestaCorrecta + 1) % 4);
    m_npcOpcion = opcion;                       // para resaltar cuál eligió
    m_lblTurno.setString("Turno del NPC (respondiendo)");
    procesarRespuestaTurno(opcion, Turno::Npc); // pone feedback y aplica daño
}

void Combate::actualizarHUDJugador() {
    // Solo si cambió la vida
    if (m_vidaJugador == m_vidasCacheJ) return;
    m_vidasCacheJ = m_vidaJugador;

    m_corazonesJ.clear();
    const sf::Vector2f base = m_hudBoxJ.getPosition() + sf::Vector2f{15.f, 42.f};
    for (int i = 0; i < m_vidaJugador; ++i) {
        sf::Sprite c(m_texCorazon);
        c.setScale({0.1f, 0.1f});            // mismo escalado que Gimnasio
        c.setPosition(base + sf::Vector2f{i * 35.f, 0.f});
        m_corazonesJ.push_back(c);
    }
}

void Combate::actualizarHUDNpc() {
    if (m_vidaNpc == m_vidasCacheN) return;
    m_vidasCacheN = m_vidaNpc;

    m_corazonesN.clear();
    const sf::Vector2f base = m_hudBoxN.getPosition() + sf::Vector2f{15.f, 42.f};
    for (int i = 0; i < m_vidaNpc; ++i) {
        sf::Sprite c(m_texCorazon);
        c.setScale({0.1f, 0.1f});
        c.setPosition(base + sf::Vector2f{i * 35.f, 0.f});
        m_corazonesN.push_back(c);
    }
}

void Combate::setLeyendaTurno() {
    if (m_turnoActual == Turno::Jugador) {
        m_lblTurno.setString("Tu turno");
    } else {
        // Si quieres, puedes actualizar a “respondiendo...” cuando llama turnoNpcAuto()
        m_lblTurno.setString("Turno del NPC (pensando...)");
    }
}

sf::FloatRect Combate::boundsOpcion(int idx) const {
    switch (idx) {
        case 0: return m_txtOpcA.getGlobalBounds();
        case 1: return m_txtOpcB.getGlobalBounds();
        case 2: return m_txtOpcC.getGlobalBounds();
        default: return m_txtOpcD.getGlobalBounds();
    }
}

void Combate::dibujarResaltoNpc(sf::RenderWindow& window) {
    if (m_npcOpcion < 0) return;

    auto b = boundsOpcion(m_npcOpcion);
    // GlobalBounds en SFML3 trae .position y .size; ajusta si usas SFML2: (left, top, width, height)
    sf::Vector2f pos(b.position.x - 8.f, b.position.y - 4.f);
    sf::Vector2f sz (b.size.x + 16.f,   b.size.y + 8.f);
    m_resaltoNPC.setPosition(pos);
    m_resaltoNPC.setSize(sz);
    window.draw(m_resaltoNPC);
}

Combate::~Combate()
{
    //dtor
}
