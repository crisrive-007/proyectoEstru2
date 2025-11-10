#ifndef GIMNASIO_H
#define GIMNASIO_H

#pragma once
#include <SFML/Graphics.hpp>
#include "Estado.h"
#include "Combate.h"
#include <memory>

// --- Helpers de colisión (SFML 3 usa position/size) ---
static inline bool rectsIntersect(const sf::FloatRect& a, const sf::FloatRect& b) {
    return !(a.position.x + a.size.x <= b.position.x ||
             b.position.x + b.size.x <= a.position.x ||
             a.position.y + a.size.y <= b.position.y ||
             b.position.y + b.size.y <= a.position.y);
}

// Zona sólida de un NPC: solo "piernas" (mitad inferior), con pequeño margen
static inline sf::FloatRect npcSolidRect(const sf::Sprite& spr, float inflate = 2.f) {
    auto gb = spr.getGlobalBounds(); // tiene .position y .size
    // mitad inferior
    gb.position.y += gb.size.y * 0.50f;
    gb.size.y     *= 0.50f;

    // inflar/contraer
    gb.position.x -= inflate;
    gb.position.y -= inflate;
    gb.size.x     += 2.f * inflate;
    gb.size.y     += 2.f * inflate;
    return gb;
}

class GestorEstados;
class Personaje;

class Gimnasio : public Estado
{
    public:
        Gimnasio(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje);

        void manejarEventos(sf::RenderWindow& window) override;
        void actualizar() override;
        void dibujar(sf::RenderWindow& window) override;
        void ejecutarMapa();

        virtual ~Gimnasio();

    protected:

    private:
        // --- Core ---
        sf::RenderWindow&  m_window;
        Personaje&         m_personaje;

        // --- Mapa ---
        sf::Texture        m_texFondo;
        sf::Sprite         m_sprFondo;
        const sf::Vector2f MAP_ORIG {192.f, 108.f};
        const sf::Vector2f MAP_SIZE {1536.f, 864.f};

        // --- HUD (mismo esquema que en Biblioteca) ---
        sf::RectangleShape       m_hudBox;
        sf::Font                 m_font;
        sf::Text                 m_txtNombre;
        sf::Texture              m_texCorazon;
        std::vector<sf::Sprite>  m_corazones;
        int                      m_vidasCache = -1;
        void actualizarHUD();

        // --- NPCs en grupo ---
        struct NPC {
            sf::Texture tex;
            sf::Sprite  spr;
            bool        hasTexture = false;
            sf::RectangleShape fallback;
            std::string dialogo;

            NPC() : spr(tex) {}
        };
        NPC                m_npc;
        bool m_retoDisponible = false;
        sf::FloatRect      m_triggerNPC;     // área para activar diálogos
        int                m_dialogIdx = 0;    // rota frases 0..3
        bool               m_dialogoActivo = false;

        // Caja de diálogo
        sf::RectangleShape m_boxDialogo;
        sf::Text           m_txtDialogo;

        // Helpers NPC
        void setupNPC();
        bool jugadorCercaDelNPC() const;
        void iniciarCombate();
        std::vector<sf::FloatRect> buildNpcObstacles() const;

        // --- Colisiones base del mapa ---
        std::vector<sf::FloatRect>      m_colisiones;
        std::vector<sf::RectangleShape> m_dbgColisiones;
        sf::Vector2f                    m_prevPosJugador{};
        bool                            m_debugColisiones = true;

        // Helpers de colisiones
        void cargarColisionesMapa();  // crea TODAS las hitboxes del Gimnasio
        void aplicarColisiones();     // rebota al jugador a la pos previa
        static bool intersecta(const sf::FloatRect& A, const sf::FloatRect& B);
        sf::RectangleShape m_areaSalida;
};

#endif // GIMNASIO_H
