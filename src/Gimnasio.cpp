#include "Gimnasio.h"
#include "ProgresoJuego.h"
#include <iostream>
#include <algorithm>

static inline sf::FloatRect FR(const sf::Vector2f& pos, const sf::Vector2f& sz) {
    return sf::FloatRect(pos, sz);
}

bool Gimnasio::intersecta(const sf::FloatRect& A, const sf::FloatRect& B) {
    const float ax1 = A.position.x, ay1 = A.position.y;
    const float ax2 = ax1 + A.size.x, ay2 = ay1 + A.size.y;
    const float bx1 = B.position.x, by1 = B.position.y;
    const float bx2 = bx1 + B.size.x, by2 = by1 + B.size.y;
    return (ax1 < bx2) && (ax2 > bx1) && (ay1 < by2) && (ay2 > by1);
}


Gimnasio::Gimnasio(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje)
: Estado(gestor, personaje)
, m_window(window)
, m_personaje(personaje)
, m_sprFondo(m_texFondo)
, m_txtNombre(m_font)
, m_txtDialogo(m_font)
{
    // === Fondo base (cámbialo por el que quieras) ===
    if (!m_texFondo.loadFromFile("assets/Tilesets/GimnasioPokemon.png")) {
        std::cerr << "✗ ERROR: assets/Tilesets/FondoSimple.png\n";
    }
    m_sprFondo.setTexture(m_texFondo, true);
    const auto sz = m_texFondo.getSize();
    m_sprFondo.setPosition(MAP_ORIG);
    if (sz.x > 0 && sz.y > 0) {
        m_sprFondo.setScale({ MAP_SIZE.x / sz.x, MAP_SIZE.y / sz.y });
    }

    // Posición y escala inicial del personaje
    m_personaje.setPosition(MAP_ORIG.x + MAP_SIZE.x * 0.5f - 25.f,
                            MAP_ORIG.y + MAP_SIZE.y - 100.f);
    m_personaje.setScale(5.f, 5.f);

    // === HUD === (caja, fuente, nombre, corazones)
    m_hudBox.setSize({250.f, 80.f});
    m_hudBox.setPosition({MAP_ORIG.x + 10.f, MAP_ORIG.y + 10.f});
    m_hudBox.setFillColor(sf::Color(0, 0, 0, 120));
    m_hudBox.setOutlineColor(sf::Color::White);
    m_hudBox.setOutlineThickness(2.f);

    if (!m_font.openFromFile("assets/Pokemon_GB.ttf")) {
        std::cerr << "✗ ERROR: assets/Pokemon_GB.ttf\n";
    }
    m_txtNombre.setFont(m_font);
    m_txtNombre.setCharacterSize(20);
    m_txtNombre.setFillColor(sf::Color::White);
    m_txtNombre.setOutlineColor(sf::Color::Black);
    m_txtNombre.setOutlineThickness(2.f);
    m_txtNombre.setPosition(m_hudBox.getPosition() + sf::Vector2f{15.f, 12.f});
    m_txtNombre.setString(m_personaje.getNombre());

    if (!m_texCorazon.loadFromFile("assets/vidas.png")) {
        std::cerr << "⚠ No se pudo cargar assets/vidas.png\n";
    }
    actualizarHUD(); // reconstruye sprites de corazones según vidas actuales

    // === NPCs ===
    setupNPC();

    // Caja de diálogo (estilo simple)
    m_boxDialogo.setSize({700.f, 120.f});
    m_boxDialogo.setFillColor(sf::Color(0, 0, 0, 160));
    m_boxDialogo.setOutlineColor(sf::Color::White);
    m_boxDialogo.setOutlineThickness(2.f);
    // parte baja del mapa
    m_boxDialogo.setPosition({ MAP_ORIG.x + (MAP_SIZE.x - m_boxDialogo.getSize().x) * 0.5f,
                               MAP_ORIG.y + MAP_SIZE.y - m_boxDialogo.getSize().y - 24.f });

    m_txtDialogo.setFont(m_font);
    m_txtDialogo.setCharacterSize(20);
    m_txtDialogo.setFillColor(sf::Color::White);
    m_txtDialogo.setOutlineColor(sf::Color::Black);
    m_txtDialogo.setOutlineThickness(2.f);

    m_areaSalida.setSize({210.f, 32.f});
    m_areaSalida.setPosition({ MAP_ORIG.x + MAP_SIZE.x * 0.5f - 105.f, MAP_ORIG.y + MAP_SIZE.y + 0.f });
    m_areaSalida.setFillColor(sf::Color(0, 0, 0, 0));

    // Colisiones del Gimnasio
    cargarColisionesMapa();

    ejecutarMapa();
}

void Gimnasio::ejecutarMapa() {
    m_window.setView(m_window.getDefaultView());
}

void Gimnasio::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            window.close();
            return;
        }
        if (auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::Space) {
                if (jugadorCercaDelNPC()) {
                    m_dialogoActivo   = true;
                    m_retoDisponible  = true;  // ← habilita aceptar con Enter
                    m_txtDialogo.setString(m_npc.dialogo);

                    // centrar dentro de la caja
                    const auto boxPos  = m_boxDialogo.getPosition();
                    const auto boxSize = m_boxDialogo.getSize();
                    auto gb = m_txtDialogo.getGlobalBounds();
                    m_txtDialogo.setPosition({
                        boxPos.x + (boxSize.x - gb.size.x) * 0.5f,
                        boxPos.y + (boxSize.y - gb.size.y) * 0.5f
                    });
                }
            }
            if (k->code == sf::Keyboard::Key::Enter) {
                if (m_dialogoActivo && m_retoDisponible && jugadorCercaDelNPC()) {
                    // Aceptó el reto → lanza el combate del Tercer Nivel
                    iniciarCombate();
                    m_dialogoActivo  = false;
                    m_retoDisponible = false;
                } else {
                    // Enter fuera de contexto: solo cierra el cuadro si está abierto
                    m_dialogoActivo = false;
                }
            }
        }
    }
}

void Gimnasio::actualizar() {
    // Guarda posición previa
    m_prevPosJugador = m_personaje.getPosition();

    // Movimiento libre (input)
    m_personaje.actualizarSinTiles(MAP_SIZE.x, MAP_SIZE.y);

    // Clamp al área del mapa
    {
        sf::Vector2f pos = m_personaje.getPosition();
        const sf::FloatRect hb = m_personaje.obtenerHitbox();
        const float minX = MAP_ORIG.x, minY = MAP_ORIG.y;
        const float maxX = MAP_ORIG.x + MAP_SIZE.x - hb.size.x;
        const float maxY = MAP_ORIG.y + MAP_SIZE.y - hb.size.y;
        pos.x = std::clamp(pos.x, minX, maxX);
        pos.y = std::clamp(pos.y, minY, maxY);
        m_personaje.setPosition(pos.x, pos.y);
    }

    // Aplica colisiones del mapa (rebota a pos previa si choca)
    aplicarColisiones();

    // === COLISIÓN CON NPC ===
    // Construye obstáculos (1 rect)
    const auto obsts = buildNpcObstacles();
    if (!obsts.empty()) {
        // Revisa intersección
        sf::FloatRect me = m_personaje.obtenerHitbox();
        if (rectsIntersect(me, obsts[0])) {
            // Resuelve por el eje de MENOR penetración (se siente más “natural”)
            const auto& o = obsts[0];

            float leftPen   = (me.position.x + me.size.x) - o.position.x;
            float rightPen  = (o.position.x + o.size.x) - me.position.x;
            float topPen    = (me.position.y + me.size.y) - o.position.y;
            float bottomPen = (o.position.y + o.size.y) - me.position.y;

            // Penetraciones positivas reales
            leftPen   = std::max(0.f, leftPen);
            rightPen  = std::max(0.f, rightPen);
            topPen    = std::max(0.f, topPen);
            bottomPen = std::max(0.f, bottomPen);

            // Decide eje/elevación mínima
            float xPush = (leftPen < rightPen) ? -leftPen : rightPen;
            float yPush = (topPen  < bottomPen)? -topPen  : bottomPen;

            if (std::abs(xPush) < std::abs(yPush)) {
                // Empuja en X
                m_personaje.setPosition(m_personaje.getPosition().x + xPush,
                                        m_personaje.getPosition().y);
            } else {
                // Empuja en Y
                m_personaje.setPosition(m_personaje.getPosition().x,
                                        m_personaje.getPosition().y + yPush);
            }
        }
    }

    // === Detección de salida ===
    sf::FloatRect playerBox = m_personaje.obtenerHitbox();
    if (intersecta(playerBox, m_areaSalida.getGlobalBounds())) {
        std::cout << "🚪 Saliendo del gimnasio.\n";
        ProgresoJuego::get().marcarCleared(ProgresoJuego::Nivel::Gimnasio);
        gestor->sacarEstado(); // vuelve al mapa anterior
        m_personaje.setPosition(585.f, 625.f); // posición fuera del gimnasio
        return;
    }

    // HUD (si cambian vidas)
    actualizarHUD();
}

void Gimnasio::dibujar(sf::RenderWindow& window) {
    window.setView(window.getDefaultView());
    window.draw(m_sprFondo);

    if (m_npc.hasTexture) window.draw(m_npc.spr);
    else                  window.draw(m_npc.fallback);

    // Debug colisiones
    if (m_debugColisiones) {
        for (const auto& r : m_dbgColisiones) window.draw(r);
    }

    // Personaje
    m_personaje.dibujar(window);

    // HUD
    window.draw(m_hudBox);
    window.draw(m_txtNombre);
    for (auto& c : m_corazones) window.draw(c);

    // Diálogo
    if (m_dialogoActivo) {
        window.draw(m_boxDialogo);
        window.draw(m_txtDialogo);
    }

    if (m_debugColisiones) {
        for (const auto& r : m_dbgColisiones) window.draw(r);
        window.draw(m_areaSalida); // <-- muestra la zona de salida
    }
}

void Gimnasio::actualizarHUD() {
    // Clonado del patrón de Biblioteca: reconstruir solo si cambia el número de vidas:contentReference[oaicite:2]{index=2}
    int vidas = m_personaje.getVidas();
    if (vidas == m_vidasCache) return;

    m_vidasCache = vidas;
    m_corazones.clear();

    const sf::Vector2f base = m_hudBox.getPosition() + sf::Vector2f{15.f, 42.f};
    for (int i = 0; i < vidas; ++i) {
        sf::Sprite c(m_texCorazon);
        c.setScale({0.1f, 0.1f});
        c.setPosition(base + sf::Vector2f{ i * 35.f, 0.f });
        m_corazones.push_back(c);
    }
}

void Gimnasio::setupNPC() {
    // 1) Determinar equipo del jugador (ajusta a tu API real si difiere)
    // Asumiendo enum:
    //   enum class Personaje::EquipoFilosofico { Racionalistas, Empiristas, Ninguno };
    auto equipo = Personaje::EquipoFilosofico::Ninguno;
    if constexpr (true) { // quita el constexpr si te molesta; solo evita warning si la API cambia
        // Si tienes getEquipoFilosofico():
        equipo = m_personaje.getEquipoFilosofico();
    }

    const bool esEmpirista   = (equipo == Personaje::EquipoFilosofico::Empiristas);
    const bool esRacionalista= (equipo == Personaje::EquipoFilosofico::Racionalistas);

    // 2) Sprite del NPC: SIEMPRE el contrario al del personaje
    std::string rutaSprite;
    if (esEmpirista)       rutaSprite = "assets/npcRacionalista.png";
    else if (esRacionalista) rutaSprite = "assets/npcEmpirista.png";
    else                   rutaSprite = "assets/NPC_Neutro.png"; // fallback si aún no eligió

    // 3) Diálogo que RETA al combate del TERCER NIVEL (Epistemefighter)
    //    Nota: Space muestra el reto; Enter acepta (ver manejarEventos)
    if (esEmpirista) {
        m_npc.dialogo =
            "NPC (Racionalista): Veo que confias en los sentidos... ¿te atreves a probar que la razon no te supera?";
    } else if (esRacionalista) {
        m_npc.dialogo =
            "NPC (Empirista): Hablas de ideas innatas... ven y demuestra que la experiencia no te derriba.";
    } else {
        m_npc.dialogo =
            "NPC: El Tercer Nivel te espera: Epistemefighter. Vuelve cuando elijas bando.";
    }

    // 4) Colocación y carga
    const sf::Vector2f pos {
        MAP_ORIG.x + MAP_SIZE.x * 0.5f - 32.f,
        MAP_ORIG.y + MAP_SIZE.y * 0.35f
    };

    if (m_npc.tex.loadFromFile(rutaSprite)) {
        m_npc.hasTexture = true;
        m_npc.spr.setTexture(m_npc.tex, true); // sprite ya construido con tex (SFML 3)
        auto tsz = m_npc.tex.getSize();
        float s = (tsz.y > 0) ? (96.f / tsz.y) : 1.f;
        m_npc.spr.setScale({s, s});
        m_npc.spr.setPosition(pos);
    } else {
        m_npc.hasTexture = false;
        m_npc.fallback.setSize({48.f, 64.f});
        m_npc.fallback.setFillColor(sf::Color(140, 180, 220, 255));
        m_npc.fallback.setOutlineColor(sf::Color::Black);
        m_npc.fallback.setOutlineThickness(2.f);
        m_npc.fallback.setPosition(pos);
    }

    // 5) Trigger alrededor del NPC
    m_triggerNPC = sf::FloatRect(
        pos - sf::Vector2f{40.f, 20.f},
        sf::Vector2f{48.f + 80.f, 64.f + 80.f}
    );
}

bool Gimnasio::jugadorCercaDelNPC() const {
    sf::FloatRect hb = m_personaje.obtenerHitbox();

    const float ax1 = hb.position.x, ay1 = hb.position.y;
    const float ax2 = ax1 + hb.size.x, ay2 = ay1 + hb.size.y;

    const float bx1 = m_triggerNPC.position.x, by1 = m_triggerNPC.position.y;
    const float bx2 = bx1 + m_triggerNPC.size.x, by2 = by1 + m_triggerNPC.size.y;

    return (ax1 < bx2) && (ax2 > bx1) && (ay1 < by2) && (ay2 > by1);
}

void Gimnasio::iniciarCombate() {
    std::unique_ptr<Combate> estadoCombate = std::make_unique<Combate>(gestor, m_window, m_personaje);
    sf::sleep(sf::seconds(0.5f));
    estadoCombate->iniciarCombate();
    gestor->empujarEstado(std::move(estadoCombate));

    std::cout << "[Gimnasio] Lanzando combate del Tercer Nivel: Epistemefighter...\n";
}

std::vector<sf::FloatRect> Gimnasio::buildNpcObstacles() const {
    std::vector<sf::FloatRect> out;
    out.reserve(1);

    // Usa el sprite si tiene textura, si no el rectángulo de fallback:
    if (m_npc.hasTexture) {
        out.push_back(npcSolidRect(m_npc.spr, 2.f));
    } else {
        // npcSolidRect está pensado para sprite; con fallback usa su global bounds directo
        auto gb = m_npc.fallback.getGlobalBounds();
        // Recortar a “piernas”: mitad inferior + pequeño inflate
        gb.position.y += gb.size.y * 0.50f;
        gb.size.y     *= 0.50f;
        gb.position.x -= 2.f; gb.position.y -= 2.f;
        gb.size.x     += 4.f; gb.size.y     += 4.f;
        out.push_back(gb);
    }
    return out;
}

void Gimnasio::cargarColisionesMapa() {
    m_colisiones.clear();
    m_dbgColisiones.clear();

    auto W = [&](float x, float y, float w, float h) -> sf::FloatRect {
        return sf::FloatRect({MAP_ORIG.x + x, MAP_ORIG.y + y}, {w, h});
    };
    auto push = [&](const sf::FloatRect& r, sf::Color fill = sf::Color(0,0,0,0)) {
        m_colisiones.push_back(r);
        sf::RectangleShape s;
        s.setPosition(r.position);
        s.setSize(r.size);
        s.setFillColor(fill);
        s.setOutlineColor(sf::Color(0,0,0,0));
        s.setOutlineThickness(1.5f);
        m_dbgColisiones.push_back(s);
    };

    // ======= Barras de colisión principales =======

    // 🔵 BARRA SUPERIOR más gruesa
    const float WALL_TOP_H = 230.f;  // antes 140
    push(W(0.f, 0.f, MAP_SIZE.x, WALL_TOP_H));

    // 🔵 Esquinas superiores (ligeramente más altas)
    const float CORNER_W = 120.f, CORNER_H = 230.f;  // antes 260
    push(W(0.f, 0.f, CORNER_W, CORNER_H));
    push(W(MAP_SIZE.x - CORNER_W, 0.f, CORNER_W, CORNER_H));

    // 🔵 Laterales
    const float SIDE_W = 30.f;
    push(W(0.f,               40.f, SIDE_W,           MAP_SIZE.y - 80.f));
    push(W(MAP_SIZE.x-SIDE_W, 40.f, SIDE_W,           MAP_SIZE.y - 80.f));

    // 🔵 BARRAS INFERIORES más grandes (con hueco central para entrada)
    const float BASE_H = 105.f;  // antes 24
    const float puertaAncho = 230.f;
    const float puertaXc = MAP_SIZE.x * 0.5f;
    push(W(0.f, MAP_SIZE.y - BASE_H, puertaXc - puertaAncho * 0.5f, BASE_H));
    push(W(puertaXc + puertaAncho * 0.5f, MAP_SIZE.y - BASE_H,
           MAP_SIZE.x - (puertaXc + puertaAncho * 0.5f), BASE_H));

    // 🔵 Peldaños laterales (puedes dejarlos igual o quitarlos)
    const float STEP_H = 20.f, STEP_W0 = 32.f, STEP_WINC = 24.f;
    const float Y0 = MAP_SIZE.y - BASE_H - (STEP_H * 3) - 6.f;
    for (int i = 0; i < 3; ++i) {
        float y = Y0 + i * STEP_H;
        float w = STEP_W0 + i * STEP_WINC;
        push(W(0.f, y, w, STEP_H));
        push(W(MAP_SIZE.x - w, y, w, STEP_H));
    }
}

void Gimnasio::aplicarColisiones() {
    const sf::FloatRect playerNow = m_personaje.obtenerHitbox();

    for (const auto& box : m_colisiones) {
        if (intersecta(playerNow, box)) {
            // Rebotar volviendo exactamente a la posición previa (suave y eficaz)
            m_personaje.setPosition(m_prevPosJugador.x, m_prevPosJugador.y);
            return;
        }
    }
}

Gimnasio::~Gimnasio()
{
    //dtor
}
