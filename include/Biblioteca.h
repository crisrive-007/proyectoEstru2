#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <random>
#include <string>
#include <array>

class GestorEstados;
class Personaje;

#include "Estado.h" // hereda de Estado(gestor, personaje)

class Biblioteca final : public Estado {
public:
    Biblioteca(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje);
    ~Biblioteca() override = default;

    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;
    void ejecutarMapa();

private:
    // --- helpers de setup ---
    void cargarColisionesMapa();        // crea TODAS las hitboxes de este mapa
    void aplicarColisiones();           // bloqueo tipo "revertir a pos previa"
    static bool intersecta(const sf::FloatRect& A, const sf::FloatRect& B);

    // ===== Lockables (4 bars + 4 puertas) =====
    void setupLockables();                   // crea rects de bars/puertas
    void actualizarDebugLockables();         // reconstruye shapes rojos en base a locked[]
    void desbloquearCategoria(int idx);      // desbloquea bar & puerta por índice 0..3
    void bloquearCategoria(int idx);

    // ===== Ruleta (pantalla con opciones que pasan) =====
    void setupRuleta();
    void iniciarRuleta(float durMin = 2.0f, float durMax = 3.5f);
    void actualizarRuleta(float dt);
    void dibujarRuleta(sf::RenderTarget& target) const;
    static float easeOutCubic(float t);
    void centrarTextoRuleta();

    // ——— Consumo por índice (legacy) y por nombre (seguro) ———
    void consumirCategoria(int idx);                    // mantiene compat
    void consumirCategoriaNombre(const std::string&);   // ✅ usar este

    // ===== Fondos temáticos según categoría =====
    void cargarFondosTematicos();            // carga texturas por categoría
    void aplicarTemaPorCategoria(int idx);   // legacy (usa índice 0..3)
    void aplicarTemaPorCategoriaNombre(const std::string& nom); // ✅ por nombre

    // ===== Acciones al colisionar puertas =====
    void ejecutarAccionPuerta(int idx);      // qué hace cada puerta al ser tocada (si está desbloqueada)
    void mostrarMensaje(const std::string& s, float seg);
    void actualizarMensaje(float dt);

    // ===== Mapeo nombre <-> índice canónico =====
    int puertaIndexFromName(const std::string& s) const;   // "Arte"→0...
    const char* nombrePuerta(int idx) const;                // 0→"Arte"

private:
    sf::RenderWindow&          m_window;
    Personaje&                 m_personaje;

    // Fondo
    sf::Texture                m_texFondoBase;     // fondo por defecto
    sf::Sprite                 m_sprFondo;

    // Fondos por tema (0 Arte, 1 Política, 2 Ciencia, 3 Historia)
    std::array<sf::Texture,4>  m_texFondos;
    int                        m_idxFondoActual = -1; // -1 = base

    // Colisiones base (siempre activas)
    std::vector<sf::FloatRect> m_colisiones;
    std::vector<sf::RectangleShape> m_dbgColisiones; // solo si activas debug

    // Estado del jugador (para revertir tras choque)
    sf::Vector2f               m_prevPosJugador;

    // Área visible del mapa (dentro de 1920×1080)
    const sf::Vector2f         MAP_ORIG  {192.f, 108.f};
    const sf::Vector2f         MAP_SIZE  {1536.f, 864.f};

    // ===== Lockables =====
    // Índices 0..3: 0=Arte, 1=Politica, 2=Ciencia, 3=Historia
    sf::FloatRect              m_barRect[4];           // mostradores
    sf::FloatRect              m_puertaRect[4];        // 4 puertas nuevas
    bool                       m_barUnlocked[4]   {false,false,false,false};
    bool                       m_puertaUnlocked[4]{false,false,false,false};
    std::vector<sf::RectangleShape> m_dbgLockables;    // rojo = bloqueado (debug)

    // Debug
    bool m_debugColisiones = true; // F3 toggle

    // ===== Ruleta (simple tipo ticker) =====
    bool           m_ruletaActiva   = true;
    bool           m_ruletaGirando  = false;
    bool           m_ruletaBloqueadaHastaEntrar = false;
    sf::Texture    m_texPantallaR;
    sf::Sprite     m_sprPantallaR;
    sf::Font       m_fontR;
    sf::Text       m_txtR;
    std::vector<std::string> m_opcionesR { "Arte", "Politica", "Ciencia", "Historia" };

    // temporización del shuffle
    float m_ruletaTiempoTotal = 0.f;
    float m_ruletaDuracion    = 0.f;
    float m_tickAcum          = 0.f;
    float m_tickActual        = 0.f;
    float m_tickRapido        = 0.05f;
    float m_tickLento         = 0.28f;

    // índices (del vector m_opcionesR, ¡ojo! solo visual)
    int   m_idxActual   = 0;
    int   m_idxAnterior = -1;
    int   m_idxObjetivo = 0;

    // rng + clock local (porque actualizar() no recibe dt)
    std::mt19937 m_rng;
    sf::Clock    m_clk;

    // ===== Mensajes in-game =====
    sf::Text     m_msgText;
    float        m_msgTimer = 0.f;

    // Cooldown para que la acción de puerta no se dispare en bucle mientras sigues dentro
    float        m_puertaTriggerCooldown = 0.f;

    // ===== Triggers tipo ejemplo =====
    sf::RectangleShape m_areaSalida;     // zona para salir de Biblioteca
    sf::RectangleShape m_areaPuerta1;
    sf::RectangleShape m_areaPuerta2;
    sf::RectangleShape m_areaPuerta3;
    sf::RectangleShape m_areaPuerta4;

    // Lógica copia-del-ejemplo
    void interaccionPuertas();
    void interaccionRuleta(); // si lo quieres usar por proximidad (opcional)

    // --- al final de la sección private: (añade esto) ---

    // ===== Elección de equipo (Racionalistas vs Empiristas) =====
    void verificarFinMinijuegos();           // llamada en actualizar()
    void iniciarEleccionEquipo();            // prepara sprites/UX
    void dibujarEleccion(sf::RenderTarget&); // overlay + sprites
    void manejarClickEleccion(const sf::Vector2f& worldPos);

    bool         m_eleccionActiva = false;
    int          m_equipoElegido  = -1;      // -1 = sin elegir, 0 = racionalistas, 1 = empiristas

    sf::Texture  m_texRacionalistas;
    sf::Texture  m_texEmpiristas;
    sf::Sprite   m_sprRacionalistas;
    sf::Sprite   m_sprEmpiristas;

    sf::RectangleShape m_overlayEleccion;    // fondo semitransparente
    sf::Text     m_txtTituloEleccion;

    sf::FloatRect m_hitRacionalistas;        // bounds clickables
    sf::FloatRect m_hitEmpiristas;

    sf::Text m_txtNombreRacionalistas;
    sf::Text m_txtNombreEmpiristas;

    // Hover y escalado dinámico
    bool m_hoverRacionalistas = false;
    bool m_hoverEmpiristas    = false;

    void actualizarHoverEleccion(const sf::Vector2f& worldPos);

    // Bases de estandartes (para que el hover respete lo inicializado)
    sf::Vector2f m_baseCenterR{}, m_baseCenterE{};
    sf::Vector2f m_baseScaleR{1.f,1.f}, m_baseScaleE{1.f,1.f};
    float        m_hoverFactor = 1.1f;

    // Posiciones fijas de los nombres (no cambian con el hover)
    sf::Vector2f m_namePosR{}, m_namePosE{};

    sf::RectangleShape m_hudBox;
    sf::Text           m_txtNombre;
    sf::Texture        m_texCorazon;
    std::vector<sf::Sprite> m_corazones;
    int m_vidasCache = -1;

    // Rebuild rápido cuando cambien las vidas
    void actualizarHUD();

    bool puedeSalirBiblioteca() const;          // true si ya no quedan categorías
    int  minijuegosRestantes() const;
};

#endif
