#include "Biblioteca.h"
#include "MinijuegoArte.h"
#include "MinijuegoCiencia.h"
#include "MinijuegoHistoria.h"
#include "MinijuegoPolitica.h"
#include "ProgresoJuego.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdint>

// ==== helpers ====
static inline sf::FloatRect FR(const sf::Vector2f& pos, const sf::Vector2f& sz) {
    return sf::FloatRect(pos, sz);
}

bool Biblioteca::intersecta(const sf::FloatRect& A, const sf::FloatRect& B) {
    const float ax1 = A.position.x;
    const float ay1 = A.position.y;
    const float ax2 = ax1 + A.size.x;
    const float ay2 = ay1 + A.size.y;

    const float bx1 = B.position.x;
    const float by1 = B.position.y;
    const float bx2 = bx1 + B.size.x;
    const float by2 = by1 + B.size.y;

    return (ax1 < bx2) && (ax2 > bx1) && (ay1 < by2) && (ay2 > by1);
}

// ====== ctor ======
Biblioteca::Biblioteca(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje)
: Estado(gestor, personaje)
, m_window(window)
, m_personaje(personaje)
, m_sprFondo(m_texFondoBase)
, m_sprPantallaR(m_texPantallaR)
, m_txtR(m_fontR)
, m_msgText(m_fontR)
, m_sprRacionalistas(m_texRacionalistas)
, m_sprEmpiristas(m_texEmpiristas)
, m_txtTituloEleccion(m_fontR)
, m_txtNombreRacionalistas(m_fontR)
, m_txtNombreEmpiristas(m_fontR)
, m_txtNombre(m_fontR)
{
    // RNG
    std::random_device rd; m_rng.seed(rd());

    // === Fondo base ===
    if (!m_texFondoBase.loadFromFile("assets/Tilesets/CentroPokemon.png")) {
        std::cerr << "✗ ERROR: assets/Tilesets/CentroPokemon.png\n";
    }
    m_sprFondo.setTexture(m_texFondoBase, true);
    {
        const auto sz = m_texFondoBase.getSize();
        m_sprFondo.setPosition(MAP_ORIG);
        m_sprFondo.setScale({ MAP_SIZE.x / sz.x, MAP_SIZE.y / sz.y });
    }

    // Cargar fondos temáticos
    cargarFondosTematicos();

    // Spawn jugador
    m_personaje.setPosition(MAP_ORIG.x + MAP_SIZE.x * 0.5f - 25.f, MAP_ORIG.y + MAP_SIZE.y - 100.f);
    m_personaje.setScale(5.f,5.f);

    // Colisiones base
    cargarColisionesMapa();

    // Lockables (bars + 4 puertas)
    setupLockables();

    // === TRIGGERS estilo ejemplo ===
    // Salida general (ajusta a donde tengas la “salida”)
    m_areaSalida.setSize({210.f, 32.f});
    m_areaSalida.setPosition({ MAP_ORIG.x + MAP_SIZE.x * 0.5f - 105.f, 950.f});
    m_areaSalida.setFillColor(sf::Color(0, 0, 0, 0));

    // Puertas: alineamos los triggers al rect de cada puerta
    auto mkDoor = [&](sf::RectangleShape& r, const sf::FloatRect& fr, sf::Color c){
        r.setSize( sf::Vector2f(fr.size.x, fr.size.y) );
        r.setPosition( sf::Vector2f(fr.position.x, fr.position.y) );
        r.setFillColor(c);
    };

    mkDoor(m_areaPuerta1, m_puertaRect[0], sf::Color(0,   0, 0,  0));
    mkDoor(m_areaPuerta2, m_puertaRect[1], sf::Color(0, 0,   0,  0));
    mkDoor(m_areaPuerta3, m_puertaRect[2], sf::Color(0,   0, 0,  0));
    mkDoor(m_areaPuerta4, m_puertaRect[3], sf::Color(0, 0, 0,  0));

    // === HUD (Nombre y Vidas) ===
    m_hudBox.setSize({250.f, 80.f});
    m_hudBox.setPosition({MAP_ORIG.x + 10.f, MAP_ORIG.y + 10.f});
    m_hudBox.setFillColor(sf::Color(0, 0, 0, 120));
    m_hudBox.setOutlineColor(sf::Color::White);
    m_hudBox.setOutlineThickness(2.f);

    // Reutilizamos la misma fuente del juego (m_fontR ya está cargada)
    m_txtNombre.setFont(m_fontR);
    m_txtNombre.setCharacterSize(20);
    m_txtNombre.setFillColor(sf::Color::White);
    m_txtNombre.setOutlineColor(sf::Color::Black);
    m_txtNombre.setOutlineThickness(2.f);
    m_txtNombre.setPosition(m_hudBox.getPosition() + sf::Vector2f{15.f, 12.f});

    // Si tu Personaje no tiene getNombre(), usa un literal o añade ese método.
    m_txtNombre.setString(m_personaje.getNombre());

    // Cargar corazón y construir corazones iniciales
    if (!m_texCorazon.loadFromFile("assets/vidas.png")) {
        std::cerr << "⚠ No se pudo cargar assets/UI/corazon.png\n";
    }
    actualizarHUD();

    // Cámara simple
    ejecutarMapa();

    // Ruleta
    setupRuleta();
}

void Biblioteca::ejecutarMapa() {
    m_window.setView(m_window.getDefaultView());
}

// ====== Eventos ======
void Biblioteca::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        // ——— Captura exclusiva mientras la elección está activa ———
        if (m_eleccionActiva) {
            if (auto* m = ev->getIf<sf::Event::MouseMoved>()) {
                sf::Vector2f worldPos = window.mapPixelToCoords({ m->position.x, m->position.y });
                actualizarHoverEleccion(worldPos);
            }
            if (auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    // Posición de clic en coords de ventana → a coords "mundo" (usamos la view por defecto)
                    sf::Vector2f worldPos = window.mapPixelToCoords({ m->position.x, m->position.y });
                    manejarClickEleccion(worldPos);
                }
            }
            // Consumimos eventos mientras la ventana de elección está activa (no dejamos girar ruleta, etc.)
            continue;
        }
        if (ev->is<sf::Event::Closed>()) { window.close(); return; }
        if (auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::F3) m_debugColisiones = !m_debugColisiones;

            // Ruleta: SPACE para girar, ENTER para mostrar/ocultar
            if (m_ruletaActiva && k->code == sf::Keyboard::Key::Space) {
                if (!m_ruletaGirando && !m_ruletaBloqueadaHastaEntrar) { // ✅ respeta bloqueo
                    iniciarRuleta(2.0f, 3.5f);
                } else if (m_ruletaBloqueadaHastaEntrar) {
                    mostrarMensaje("Primero entra a la puerta desbloqueada.", 1.2f);
                }
            }
            if (k->code == sf::Keyboard::Key::Enter) {
                m_ruletaActiva = !m_ruletaActiva;
            }
        }
    }
}

// ====== Update ======
void Biblioteca::actualizar() {
    float dt = m_clk.restart().asSeconds();

    // Cooldowns / mensajes
    if (m_puertaTriggerCooldown > 0.f) m_puertaTriggerCooldown -= dt;
    actualizarMensaje(dt);

    // Movimiento básico + clamp
    m_prevPosJugador = m_personaje.getPosition();
    m_personaje.actualizarSinTiles(MAP_SIZE.x, MAP_SIZE.y);
    {
        sf::FloatRect hb = m_personaje.obtenerHitbox();
        sf::Vector2f pos = m_personaje.getPosition();
        const float minX = MAP_ORIG.x, minY = MAP_ORIG.y;
        const float maxX = MAP_ORIG.x + MAP_SIZE.x - hb.size.x;
        const float maxY = MAP_ORIG.y + MAP_SIZE.y - hb.size.y;
        pos.x = std::clamp(pos.x, minX, maxX);
        pos.y = std::clamp(pos.y, minY, maxY);
        m_personaje.setPosition(pos.x, pos.y);
    }

    // Colisiones
    aplicarColisiones();

    // Triggers estilo ejemplo (salida + puertas con locks)
    interaccionPuertas();

    verificarFinMinijuegos();

    // Ruleta
    if (m_ruletaActiva) actualizarRuleta(dt);

    actualizarHUD();
}

// ====== Draw ======
void Biblioteca::dibujar(sf::RenderWindow& window) {
    window.setView(window.getDefaultView());
    window.draw(m_sprFondo);

    // Debug colisiones base
    if (m_debugColisiones) {
        for (const auto& r : m_dbgColisiones) window.draw(r);
        for (const auto& r : m_dbgLockables)  window.draw(r);
        m_window.draw(m_areaSalida);
        m_window.draw(m_areaPuerta1);
        m_window.draw(m_areaPuerta2);
        m_window.draw(m_areaPuerta3);
        m_window.draw(m_areaPuerta4);
    }

    if (m_ruletaActiva) dibujarRuleta(window);

    m_personaje.dibujar(window);

    if (m_eleccionActiva) {
        dibujarEleccion(window);
    }

    // === Dibujar HUD ===
    window.draw(m_hudBox);
    window.draw(m_txtNombre);
    for (auto& c : m_corazones) window.draw(c);

}

// ======================== COLISIONES MAPA (base) ========================
void Biblioteca::cargarColisionesMapa() {
    m_colisiones.clear();
    m_dbgColisiones.clear();

    auto W = [&](float x, float y, float w, float h) -> sf::FloatRect {
        return sf::FloatRect({MAP_ORIG.x + x, MAP_ORIG.y + y}, {w, h});
    };
    auto push = [&](const sf::FloatRect& r){
        m_colisiones.push_back(r);
        sf::RectangleShape s;
        s.setPosition(r.position);
        s.setSize(r.size);
        s.setFillColor(sf::Color(0,0,0,0));   // cian translúcido
        s.setOutlineColor(sf::Color(0,0,0,0));
        s.setOutlineThickness(1.5f);
        m_dbgColisiones.push_back(s);
    };

    // 1) PARED SUPERIOR (barra)
    const float WALL_TOP_H = 62.f;
    push(W(0.f, 0.f, MAP_SIZE.x, WALL_TOP_H));

    // Esquinas superiores
    const float CORNER_W = 110.f, CORNER_H = 280.f;
    push(W(0.f, 0.f, CORNER_W, CORNER_H));
    push(W(MAP_SIZE.x - CORNER_W, 0.f, CORNER_W, CORNER_H));

    // 2) PAREDES LATERALES
    push(W(0.f,               40.f, 22.f,              MAP_SIZE.y - 80.f));
    push(W(MAP_SIZE.x - 22.f, 40.f, 22.f,              MAP_SIZE.y - 80.f));

    // 3) Zócalo inferior + puerta central (hueco)
    const float BASE_H = 24.f;
    const float puertaAncho = 240.f;
    const float puertaXc = MAP_SIZE.x * 0.5f;
    push(W(0.f,  MAP_SIZE.y - BASE_H, puertaXc - puertaAncho * 0.5f, BASE_H));
    push(W(puertaXc + puertaAncho * 0.5f, MAP_SIZE.y - BASE_H,
           MAP_SIZE.x - (puertaXc + puertaAncho * 0.5f), BASE_H));

    // 4) Escaleras inferiores (tres peldaños laterales)
    const float STEP_H=20.f, STEP_W0=32.f, STEP_WINC=24.f, MARGIN=6.f;
    const float Y0 = MAP_SIZE.y - BASE_H - (STEP_H * 3) - MARGIN;
    for (int i=0;i<3;++i){
        float y = Y0 + i*STEP_H;
        float w = STEP_W0 + i*STEP_WINC;
        push(W(0.f,            y, w, STEP_H));
        push(W(MAP_SIZE.x - w, y, w, STEP_H));
    }

    // 5) Postes (fuste)
    const float POSTE_W_SPRITE = 64.f;
    const float POSTE_W_SHAFT  = 86.f;
    const float POSTE_Y_SHAFT  = 60.f;
    const float POSTE_H_SHAFT  = 420.f;
    const float SHIFT_LEFT = 109.f;
    float posteX[4] = { 245.f - SHIFT_LEFT, 572.f - SHIFT_LEFT, 895.f - SHIFT_LEFT, 1225.f - SHIFT_LEFT };
    for (float px : posteX) {
        float shaftX = px + (POSTE_W_SPRITE - POSTE_W_SHAFT) * 0.5f;
        push(W(shaftX, POSTE_Y_SHAFT, POSTE_W_SHAFT, POSTE_H_SHAFT));
    }

    // Gadgets a la derecha de cada poste (bloques cuadrados)
    auto pushG = [&](float x){ m_colisiones.push_back(W(x,265,110,110));
        sf::RectangleShape s; s.setPosition({MAP_ORIG.x + x, MAP_ORIG.y + 265});
        s.setSize({110,110}); s.setFillColor(sf::Color(0,0,0,60));
        s.setOutlineColor(sf::Color(0,0,0,0)); s.setOutlineThickness(1.5f);
        m_dbgColisiones.push_back(s);
    };
    pushG(220); pushG(550); pushG(870); pushG(1200);

    // PANELES NARANJA
    const float ORANGE_Y  = 70.f;
    const float ORANGE_H  = 120.f;
    const float ORANGE_W  = 120.f;
    push(W(220,  ORANGE_Y, ORANGE_W, ORANGE_H));
    push(W(540,  ORANGE_Y, ORANGE_W, ORANGE_H));
    push(W(870,  ORANGE_Y, ORANGE_W, ORANGE_H));
    push(W(1190, ORANGE_Y, ORANGE_W, ORANGE_H));
}

// ======================== LOCKABLES (bars + puertas) ========================
void Biblioteca::setupLockables() {
    m_dbgLockables.clear();

    auto W = [&](float x, float y, float w, float h) -> sf::FloatRect {
        return sf::FloatRect({MAP_ORIG.x + x, MAP_ORIG.y + y}, {w, h});
    };

    // --- BARS (mostradores) ---
    m_barRect[0] = W(330, 295, 115, 57);
    m_barRect[1] = W(660, 295, 115, 57);
    m_barRect[2] = W(980, 295, 115, 57);
    m_barRect[3] = W(1310,295, 115, 57);

    // --- 4 PUERTAS nuevas ---
    const float PW = 100.f, PH = 90.f;
    m_puertaRect[0] = W(340, 100, PW, PH);
    m_puertaRect[1] = W(670, 100, PW, PH);
    m_puertaRect[2] = W(990, 100, PW, PH);
    m_puertaRect[3] = W(1320,100, PW, PH);

    // Al inicio: todo bloqueado
    for (int i=0;i<4;++i){ m_barUnlocked[i]=false; m_puertaUnlocked[i]=false; }

    actualizarDebugLockables();
}

void Biblioteca::actualizarDebugLockables() {
    m_dbgLockables.clear();

    auto addDbg = [&](const sf::FloatRect& r){
        sf::RectangleShape s;
        s.setPosition(r.position);
        s.setSize(r.size);
        s.setFillColor(sf::Color(0,0,0,0));      // rojo translúcido = bloqueado
        s.setOutlineColor(sf::Color(0,0,0,0));
        s.setOutlineThickness(1.5f);
        m_dbgLockables.push_back(s);
    };

    // Bars bloqueadas
    for (int i=0;i<4;++i) if (!m_barUnlocked[i]) addDbg(m_barRect[i]);
    // Puertas bloqueadas
    for (int i=0;i<4;++i) if (!m_puertaUnlocked[i]) addDbg(m_puertaRect[i]);
}

void Biblioteca::desbloquearCategoria(int idx) {
    idx = std::clamp(idx, 0, 3);
    if (!m_barUnlocked[idx])    m_barUnlocked[idx]    = true;
    if (!m_puertaUnlocked[idx]) m_puertaUnlocked[idx] = true;   // <- IMPORTANTE
    actualizarDebugLockables();
}

void Biblioteca::bloquearCategoria(int idx) {
    idx = std::clamp(idx, 0, 3);
    if (m_barUnlocked[idx])    { m_barUnlocked[idx]    = false; std::cout << "✓ Bar "    << idx << " bloqueado\n"; }
}

// ======================== APLICAR COLISIONES ========================
void Biblioteca::aplicarColisiones() {
    const sf::FloatRect playerNow = m_personaje.obtenerHitbox();

    // 1) Colisiones base
    for (const auto& box : m_colisiones) {
        if (intersecta(playerNow, box)) {
            m_personaje.setPosition(m_prevPosJugador.x, m_prevPosJugador.y);
            return;
        }
    }

    // 2) Bars bloqueadas
    for (int i=0;i<4;++i) if (!m_barUnlocked[i]) {
        if (intersecta(playerNow, m_barRect[i])) {
            m_personaje.setPosition(m_prevPosJugador.x, m_prevPosJugador.y);
            return;
        }
    }
}

// ======================== RULETA ========================
void Biblioteca::setupRuleta() {
    if (!m_texPantallaR.loadFromFile("assets/Spinwheel/screen.png")) {
        std::cerr << "⚠ Falta assets/UI/pantalla_ruleta.png (se verá solo el texto)\n";
    } else {
        m_sprPantallaR.setTexture(m_texPantallaR, true);
        m_sprPantallaR.setOrigin({ m_texPantallaR.getSize().x/2.f, m_texPantallaR.getSize().y/2.f });
    }

    if (!m_fontR.openFromFile("assets/Pokemon_GB.ttf")) {
        std::cerr << "✗ ERROR: assets/Pokemon_GB.ttf\n";
    }

    m_txtR.setFont(m_fontR);
    m_txtR.setString(m_opcionesR[m_idxActual]);
    m_txtR.setCharacterSize(20);
    m_txtR.setFillColor(sf::Color::White);
    m_txtR.setOutlineColor(sf::Color(0,0,0,200));
    m_txtR.setOutlineThickness(3.f);

    // Centro alto del mapa
    m_sprPantallaR.setPosition({1200, 800});
    m_sprPantallaR.setScale({5.f, 5.f});
    centrarTextoRuleta();

    m_ruletaActiva  = true;
    m_ruletaGirando = false;
    m_idxActual     = 0;
    m_tickActual    = m_tickRapido;
}

void Biblioteca::iniciarRuleta(float durMin, float durMax) {
    if (m_opcionesR.empty()) return;
    std::uniform_real_distribution<float> dDur(durMin, durMax);
    std::uniform_int_distribution<int>   dIdx(0, (int)m_opcionesR.size()-1);

    m_ruletaDuracion    = dDur(m_rng);
    m_idxObjetivo       = dIdx(m_rng);
    m_ruletaTiempoTotal = 0.f;
    m_tickAcum          = 0.f;
    m_tickActual        = m_tickRapido;
    m_ruletaGirando     = true;
}

void Biblioteca::actualizarRuleta(float dt) {
    if (!m_ruletaGirando) return;

    m_ruletaTiempoTotal += dt;
    float t = std::clamp(m_ruletaTiempoTotal / std::max(0.0001f, m_ruletaDuracion), 0.f, 1.f);
    m_tickActual = m_tickRapido + (m_tickLento - m_tickRapido) * easeOutCubic(t);

    m_tickAcum += dt;
    if (m_tickAcum >= m_tickActual) {
        m_tickAcum = 0.f;
        m_idxActual = (m_idxActual + 1) % (int)m_opcionesR.size();
        m_txtR.setString(m_opcionesR[m_idxActual]);
        centrarTextoRuleta();
    }

    if (m_ruletaTiempoTotal >= m_ruletaDuracion) {
        if (m_idxActual == m_idxObjetivo && m_tickAcum == 0.f) {
            m_ruletaGirando = false;

            const std::string categoria = m_opcionesR[m_idxActual];
            const int idxPuerta = puertaIndexFromName(categoria);
            if (idxPuerta >= 0) {
                if (m_idxAnterior >= 0) bloquearCategoria(m_idxAnterior);
                desbloquearCategoria(idxPuerta);
                m_idxAnterior = idxPuerta;
                aplicarTemaPorCategoriaNombre(categoria);

                // ✅ Bloquear nuevos giros hasta que entre a una puerta
                m_ruletaBloqueadaHastaEntrar = true;
                mostrarMensaje("Puerta desbloqueada: " + categoria + ". Entra para continuar.", 1.4f);
            }
        }
    }
}

void Biblioteca::dibujarRuleta(sf::RenderTarget& target) const {
    if (m_texPantallaR.getSize().x > 0) target.draw(m_sprPantallaR);
    target.draw(m_txtR);
}

float Biblioteca::easeOutCubic(float x) {
    float inv = 1.f - x;
    return 1.f - inv*inv*inv;
}

void Biblioteca::centrarTextoRuleta() {
    auto gb = m_txtR.getGlobalBounds();
    m_txtR.setOrigin({ gb.size.x/2.f, gb.size.y/2.f });
    m_txtR.setPosition(m_sprPantallaR.getPosition());
}

// —— Consumo por índice (legacy, ahora redirige a nombre)
void Biblioteca::consumirCategoria(int idx) {
    consumirCategoriaNombre(nombrePuerta(idx));
}

// —— Consumo por nombre (estable)
void Biblioteca::consumirCategoriaNombre(const std::string& nom) {
    if (nom.empty()) return;
    if (m_ruletaGirando) return; // no tocar la lista mientras gira

    auto it = std::find(m_opcionesR.begin(), m_opcionesR.end(), nom);
    if (it == m_opcionesR.end()) return;

    const bool estabaMostrada = (!m_opcionesR.empty() && m_opcionesR[m_idxActual] == *it);

    m_opcionesR.erase(it);

    if (m_opcionesR.empty()) {
        m_ruletaActiva = false;
        m_idxActual = 0;
        m_txtR.setString("—");
        centrarTextoRuleta();
        return;
    }

    if (m_idxActual >= static_cast<int>(m_opcionesR.size()))
        m_idxActual = 0;

    if (estabaMostrada) {
        m_txtR.setString(m_opcionesR[m_idxActual]);
        centrarTextoRuleta();
    }
}

// ======================== FONDOS TEMÁTICOS ========================
void Biblioteca::cargarFondosTematicos() {
    // Intenta cargar cada tema; si uno falla, se quedará vacío y luego haremos fallback.
    const char* rutas[4] = {
        "assets/Tilesets/CentroPokemon_Arte.png",
        "assets/Tilesets/CentroPokemon_Politica.png",
        "assets/Tilesets/CentroPokemon_Ciencia.png",
        "assets/Tilesets/CentroPokemon_Historia.png"
    };

    for (int i=0;i<4;++i) {
        if (!m_texFondos[i].loadFromFile(rutas[i])) {
            std::cerr << "⚠ No se cargó " << rutas[i] << " (usaré fondo base si se elige este tema)\n";
        }
    }
    m_idxFondoActual = -1; // por defecto usando base
}

void Biblioteca::aplicarTemaPorCategoria(int idx) {
    // Legacy: conserva firma por si la usas en otro lado.
    idx = std::clamp(idx, 0, 3);

    const sf::Texture* tex = &m_texFondoBase;
    if (m_texFondos[idx].getSize().x > 0 && m_texFondos[idx].getSize().y > 0) {
        tex = &m_texFondos[idx];
        m_idxFondoActual = idx;
    } else {
        m_idxFondoActual = -1;
    }

    m_sprFondo.setTexture(*tex, true);
    const auto sz = tex->getSize();
    m_sprFondo.setPosition(MAP_ORIG);
    m_sprFondo.setScale({ MAP_SIZE.x / sz.x, MAP_SIZE.y / sz.y });
}

void Biblioteca::aplicarTemaPorCategoriaNombre(const std::string& nom) {
    const int idx = puertaIndexFromName(nom);
    if (idx < 0) { aplicarTemaPorCategoria(0); return; }
    aplicarTemaPorCategoria(idx);
}

// ======================== MAPEO nombre <-> índice ========================
int Biblioteca::puertaIndexFromName(const std::string& s) const {
    if (s == "Arte")     return 0;
    if (s == "Politica") return 1;
    if (s == "Ciencia")  return 2;
    if (s == "Historia") return 3;
    return -1;
}

const char* Biblioteca::nombrePuerta(int idx) const {
    switch (std::clamp(idx,0,3)) {
        case 0: return "Arte";
        case 1: return "Politica";
        case 2: return "Ciencia";
        case 3: return "Historia";
        default: return "";
    }
}

// ======================== ACCIÓN AL ENTRAR PUERTAS ========================
void Biblioteca::ejecutarAccionPuerta(int idx) {

    switch (idx) {
        case 0: {// Arte
            m_ruletaBloqueadaHastaEntrar = false;
            sf::sleep(sf::seconds(0.5f));
            std::unique_ptr<MinijuegoArte> estadoArte = std::make_unique<MinijuegoArte>(gestor, m_window, m_personaje);
            estadoArte->iniciar();
            gestor->empujarEstado(std::move(estadoArte));
            consumirCategoriaNombre("Arte");   // ✅ elimina exactamente Arte de la ruleta
            break;
        }
        case 1: {// Política
            m_ruletaBloqueadaHastaEntrar = false;
            sf::sleep(sf::seconds(0.5f));
            std::unique_ptr<MinijuegoPolitica> estadoPolitica = std::make_unique<MinijuegoPolitica>(gestor, m_window, m_personaje);
            estadoPolitica->iniciar();
            gestor->empujarEstado(std::move(estadoPolitica));
            consumirCategoriaNombre("Politica");
            break;
        }
        case 2: {// Ciencia
            m_ruletaBloqueadaHastaEntrar = false;
            sf::sleep(sf::seconds(0.5f));
            std::unique_ptr<MinijuegoCiencia> estadoCiencia = std::make_unique<MinijuegoCiencia>(gestor, m_window, m_personaje);
            estadoCiencia->iniciar();
            gestor->empujarEstado(std::move(estadoCiencia));
            consumirCategoriaNombre("Ciencia");
            break;
        }
        case 3: {// Historia
            m_ruletaBloqueadaHastaEntrar = false;
            sf::sleep(sf::seconds(0.5f));
            std::unique_ptr<MinijuegoHistoria> estadoHistoria = std::make_unique<MinijuegoHistoria>(gestor, m_window, m_personaje);
            estadoHistoria->iniciar();
            gestor->empujarEstado(std::move(estadoHistoria));
            consumirCategoriaNombre("Historia");
            break;
        }
        default:
            mostrarMensaje("Puerta", 0.8f);
            break;
    }
}

void Biblioteca::mostrarMensaje(const std::string& s, float seg) {
    m_msgText.setString(s);
    // arriba del centro de la pantalla
    sf::FloatRect b = m_msgText.getGlobalBounds();
    m_msgText.setOrigin({b.size.x/2.f, b.size.y/2.f});
    m_msgText.setPosition({ MAP_ORIG.x + MAP_SIZE.x*0.5f, MAP_ORIG.y + 40.f });
    m_msgTimer = seg;
}

void Biblioteca::actualizarMensaje(float dt) {
    if (m_msgTimer > 0.f) {
        m_msgTimer -= dt;
        // pequeño fade out en el último 30%
        if (m_msgTimer < 0.3f) {
            sf::Color c = m_msgText.getFillColor();
            c.a = static_cast<uint8_t>(255.f * (m_msgTimer / 0.3f));
            m_msgText.setFillColor(c);
        } else {
            sf::Color c = m_msgText.getFillColor();
            c.a = 255;
            m_msgText.setFillColor(c);
        }
    }
}

void Biblioteca::interaccionPuertas() {
    const sf::FloatRect personajeBounds = m_personaje.obtenerHitbox();

    // 1) Salida de la biblioteca (como el ejemplo)
#if SFML_VERSION_MAJOR >= 3
    if (intersecta(personajeBounds, m_areaSalida.getGlobalBounds())) {
#else
    if (intersecta(personajeBounds, m_areaSalida.getGlobalBounds())) {
#endif
        if (!puedeSalirBiblioteca()) {
            // Empuja hacia atrás y muestra mensaje con faltantes
            m_personaje.setPosition(m_prevPosJugador.x, m_prevPosJugador.y);
            const int faltan = minijuegosRestantes();
            if (faltan == 1)
                mostrarMensaje("Aún falta 1 minijuego.", 1.2f);
            else
                mostrarMensaje("Aún faltan " + std::to_string(faltan) + " minijuegos.", 1.2f);
            return;
        }

        std::cout << "🚪 Saliendo de la biblioteca.\n";
        ProgresoJuego::get().marcarCleared(ProgresoJuego::Nivel::Biblioteca);
        gestor->sacarEstado();
        m_personaje.setPosition(915.f, 300.f);
        return;
    }

    // 2) Puertas 1..4
    const sf::FloatRect r1 = m_areaPuerta1.getGlobalBounds();
    const sf::FloatRect r2 = m_areaPuerta2.getGlobalBounds();
    const sf::FloatRect r3 = m_areaPuerta3.getGlobalBounds();
    const sf::FloatRect r4 = m_areaPuerta4.getGlobalBounds();

    auto tryDoor = [&](int idx, const sf::FloatRect& trigger){
        if (!intersecta(personajeBounds, trigger)) return false;

        if (!m_puertaUnlocked[idx]) {
            // bloqueada → imita tu comportamiento: rebotar + mensaje
            m_personaje.setPosition(m_prevPosJugador.x, m_prevPosJugador.y);
            mostrarMensaje("Puerta bloqueada", 1.0f);
            return true;
        }

        // desbloqueada → respeta tu cooldown y despacha acción (minijuego, tp, etc.)
        if (m_puertaTriggerCooldown <= 0.f) {
            ejecutarAccionPuerta(idx);
            m_puertaTriggerCooldown = 0.6f; // evita spam
        }
        return true;
    };

    if (tryDoor(0, r1)) return;
    if (tryDoor(1, r2)) return;
    if (tryDoor(2, r3)) return;
    if (tryDoor(3, r4)) return;
}

void Biblioteca::verificarFinMinijuegos() {
    // Cuando ya no quedan opciones en la ruleta, lanzamos la elección (si no se mostró aún)
    if (!m_eleccionActiva && m_equipoElegido == -1 && m_opcionesR.empty()) {
        iniciarEleccionEquipo();
    }
}

void Biblioteca::iniciarEleccionEquipo() {
    m_eleccionActiva = true;

    // Overlay oscuro
    m_overlayEleccion.setSize({ MAP_SIZE.x, MAP_SIZE.y });
    m_overlayEleccion.setPosition(MAP_ORIG);
    m_overlayEleccion.setFillColor(sf::Color(0, 0, 0, 140));

    // Título
    m_txtTituloEleccion.setFont(m_fontR);            // ya cargada en setupRuleta()
    m_txtTituloEleccion.setString("Elige tu escuela:");
    m_txtTituloEleccion.setCharacterSize(28);
    m_txtTituloEleccion.setFillColor(sf::Color::White);
    m_txtTituloEleccion.setOutlineColor(sf::Color::Black);
    m_txtTituloEleccion.setOutlineThickness(3.f);
    auto tb = m_txtTituloEleccion.getGlobalBounds();
    m_txtTituloEleccion.setOrigin({ tb.size.x/2.f, tb.size.y/2.f });
    m_txtTituloEleccion.setPosition({ MAP_ORIG.x + MAP_SIZE.x*0.5f, MAP_ORIG.y + 140.f });

    // Carga de sprites (pon tus rutas reales)
    // Ejemplo de rutas (cámbialas si usas otras):
    if (!m_texRacionalistas.loadFromFile("assets/racionalistas.png")) {
        std::cout << "No se pudo cargar racionalistas.png" << std::endl;
    }
    if (!m_texEmpiristas.loadFromFile("assets/empiristas.png")) {
        std::cout << "No se pudo cargar empiristas.png" << std::endl;
    }

    m_sprRacionalistas.setTexture(m_texRacionalistas, true);
    m_sprEmpiristas.setTexture(m_texEmpiristas, true);

    // Escalado objetivo por altura
    auto texR = m_texRacionalistas.getSize();
    auto texE = m_texEmpiristas.getSize();
    float scaleR = 240.f / std::max(1u, texR.y);
    float scaleE = 240.f / std::max(1u, texE.y);
    m_sprRacionalistas.setScale({ scaleR, scaleR });
    m_sprEmpiristas.setScale({ scaleE, scaleE });

    // Posiciones: izquierda/derecha del centro
    sf::Vector2f centro { MAP_ORIG.x + MAP_SIZE.x*0.5f, MAP_ORIG.y + MAP_SIZE.y*0.5f + 40.f };
    m_sprRacionalistas.setPosition({ centro.x - 260.f - (m_sprRacionalistas.getGlobalBounds().size.x*0.5f), centro.y - (m_sprRacionalistas.getGlobalBounds().size.y*0.5f) });
    m_sprEmpiristas.setPosition({  centro.x + 260.f - (m_sprEmpiristas.getGlobalBounds().size.x*0.5f),     centro.y - (m_sprEmpiristas.getGlobalBounds().size.y*0.5f) });

    // ===== Bases de estandartes =====
    // Centro base = centro geométrico actual (tras tu pos+escala inicial)
    {
        auto gR = m_sprRacionalistas.getGlobalBounds();
        auto gE = m_sprEmpiristas.getGlobalBounds();
        m_baseCenterR = { gR.position.x + gR.size.x*0.5f, gR.position.y + gR.size.y*0.5f };
        m_baseCenterE = { gE.position.x + gE.size.x*0.5f, gE.position.y + gE.size.y*0.5f };
        m_baseScaleR  = m_sprRacionalistas.getScale();
        m_baseScaleE  = m_sprEmpiristas.getScale();
    }

    // (Opcional) actualiza los hit dinámicamente igual, pero ya no dependemos de ellos
    m_hitRacionalistas = m_sprRacionalistas.getGlobalBounds();
    m_hitEmpiristas    = m_sprEmpiristas.getGlobalBounds();

    // Textos de nombres encima de cada logo
    // Textos nombres (se crean igual que ya los tenías)
    m_txtNombreRacionalistas.setFont(m_fontR);
    m_txtNombreEmpiristas.setFont(m_fontR);
    m_txtNombreRacionalistas.setString("Racionalistas");
    m_txtNombreEmpiristas.setString("Empiristas");
    m_txtNombreRacionalistas.setCharacterSize(24);
    m_txtNombreEmpiristas.setCharacterSize(24);
    m_txtNombreRacionalistas.setFillColor(sf::Color(80, 200, 255));
    m_txtNombreEmpiristas.setFillColor(sf::Color(255, 200, 80));
    m_txtNombreRacionalistas.setOutlineColor(sf::Color::Black);
    m_txtNombreEmpiristas.setOutlineColor(sf::Color::Black);
    m_txtNombreRacionalistas.setOutlineThickness(2.f);
    m_txtNombreEmpiristas.setOutlineThickness(2.f);

    // Centramos los nombres respecto a la posición INICIAL del sprite (base) y guardamos esa posición
    auto gR = m_sprRacionalistas.getGlobalBounds();
    auto gE = m_sprEmpiristas.getGlobalBounds();
    auto nR = m_txtNombreRacionalistas.getGlobalBounds();
    auto nE = m_txtNombreEmpiristas.getGlobalBounds();

    // Calcula posición fija (encima del logo base)
    m_namePosR = {
        gR.position.x + gR.size.x * 0.5f - nR.size.x * 0.5f,
        gR.position.y - 40.f - nR.size.y * 0.5f
    };
    m_namePosE = {
        gE.position.x + gE.size.x * 0.5f - nE.size.x * 0.5f,
        gE.position.y - 40.f - nE.size.y * 0.5f
    };

    // Asigna una única vez
    m_txtNombreRacionalistas.setPosition(m_namePosR);
    m_txtNombreEmpiristas.setPosition(m_namePosE);

    // ===== Bases de estandartes (para hover anclado) =====
    m_baseCenterR = { gR.position.x + gR.size.x*0.5f, gR.position.y + gR.size.y*0.5f };
    m_baseCenterE = { gE.position.x + gE.size.x*0.5f, gE.position.y + gE.size.y*0.5f };
    m_baseScaleR  = m_sprRacionalistas.getScale();
    m_baseScaleE  = m_sprEmpiristas.getScale();
}

void Biblioteca::dibujarEleccion(sf::RenderTarget& target) {
    target.draw(m_overlayEleccion);
    target.draw(m_txtTituloEleccion);
    target.draw(m_sprRacionalistas);
    target.draw(m_sprEmpiristas);
    target.draw(m_txtNombreRacionalistas);
    target.draw(m_txtNombreEmpiristas);

    // (opcional debug) mostrar rects clicables
    if (m_debugColisiones) {
        auto drawRect = [&](const sf::FloatRect& r){
            sf::RectangleShape sh;
            sh.setPosition(r.position);
            sh.setSize(r.size);
            sh.setFillColor(sf::Color(0,0,0,0));
            sh.setOutlineColor(sf::Color::Green);
            sh.setOutlineThickness(2.f);
            target.draw(sh);
        };
        // drawRect(m_hitRacionalistas);
        // drawRect(m_hitEmpiristas);
    }
}

void Biblioteca::manejarClickEleccion(const sf::Vector2f& worldPos) {
    if (!m_eleccionActiva) return;

    if (m_sprRacionalistas.getGlobalBounds().contains(worldPos)) {
        m_equipoElegido  = 0;
        m_eleccionActiva = false;
        m_personaje.setEquipoFilosofico(Personaje::EquipoFilosofico::Racionalistas);
        mostrarMensaje("Te uniste a los Racionalistas.", 1.6f);
        return;
    }
    if (m_sprEmpiristas.getGlobalBounds().contains(worldPos)) {
        m_equipoElegido  = 1;
        m_eleccionActiva = false;
        m_personaje.aplicarSkinEmpirista();
        m_personaje.setEquipoFilosofico(Personaje::EquipoFilosofico::Empiristas);
        mostrarMensaje("Te uniste a los Empiristas.", 1.6f);
        return;
    }
}

static inline sf::Vector2f centerOf(const sf::FloatRect& r) {
    return { r.position.x + r.size.x*0.5f, r.position.y + r.size.y*0.5f };
}

static inline void placeNameAbove(const sf::Sprite& spr, sf::Text& txt, float offsetY = 40.f) {
    auto gS = spr.getGlobalBounds();
    auto gT = txt.getGlobalBounds();
    // Centrado horizontal, un poco arriba (offsetY)
    txt.setPosition({
        gS.position.x + gS.size.x*0.5f - gT.size.x*0.5f,
        gS.position.y - offsetY - gT.size.y*0.5f
    });
}

void Biblioteca::actualizarHoverEleccion(const sf::Vector2f& worldPos) {
    auto gR = m_sprRacionalistas.getGlobalBounds();
    auto gE = m_sprEmpiristas.getGlobalBounds();

    bool overR = gR.contains(worldPos);
    bool overE = gE.contains(worldPos);

    if (overR != m_hoverRacionalistas) {
        m_hoverRacionalistas = overR;

        // Escala objetivo = base * factor; recentra al MISMO centro base
        sf::Vector2f targetScale = m_baseScaleR * (overR ? m_hoverFactor : 1.f);
        m_sprRacionalistas.setScale(targetScale);
        auto ng = m_sprRacionalistas.getGlobalBounds();
        m_sprRacionalistas.setPosition({
            m_baseCenterR.x - ng.size.x * 0.5f,
            m_baseCenterR.y - ng.size.y * 0.5f
        });

        // NO mover m_txtNombreRacionalistas (permanece en m_namePosR)
        // Si quieres asegurar, re-asigna la misma posición fija:
        m_txtNombreRacionalistas.setPosition(m_namePosR);
    }

    if (overE != m_hoverEmpiristas) {
        m_hoverEmpiristas = overE;

        sf::Vector2f targetScale = m_baseScaleE * (overE ? m_hoverFactor : 1.f);
        m_sprEmpiristas.setScale(targetScale);
        auto ng = m_sprEmpiristas.getGlobalBounds();
        m_sprEmpiristas.setPosition({
            m_baseCenterE.x - ng.size.x * 0.5f,
            m_baseCenterE.y - ng.size.y * 0.5f
        });

        // NO mover m_txtNombreEmpiristas
        m_txtNombreEmpiristas.setPosition(m_namePosE);
    }
}

void Biblioteca::actualizarHUD() {
    int vidas = m_personaje.getVidas();
    if (vidas == m_vidasCache) return; // nada que hacer

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

bool Biblioteca::puedeSalirBiblioteca() const {
    // Cuando la ruleta ya no tiene opciones, significa que se consumieron las 4
    // categorías (Arte, Política, Ciencia, Historia).
    return m_opcionesR.empty();
}

int Biblioteca::minijuegosRestantes() const {
    return static_cast<int>(m_opcionesR.size());
}
