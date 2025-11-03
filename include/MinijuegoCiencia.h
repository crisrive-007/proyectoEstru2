#ifndef MINIJUEGOCIENCIA_H
#define MINIJUEGOCIENCIA_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <string>
#include <vector>
#include "Estado.h"
#include "Personaje.h"

struct Preguntas {
    std::string enunciado;
    std::array<std::string,4> opciones;
    int correcta = 0;
};

class MinijuegoCiencia : public Estado
{
    public:
        enum class Fase { Pregunta, Feedback, Fin };

        MinijuegoCiencia(GestorEstados* g, sf::RenderWindow& w, Personaje& p);

        void manejarEventos(sf::RenderWindow& window) override;
        void actualizar() override;
        void dibujar(sf::RenderWindow& window) override;

        void iniciar();
        virtual ~MinijuegoCiencia();

    protected:

    private:
        static bool containsPoint(const sf::FloatRect& r, const sf::Vector2f& p) {
            const float x1 = r.position.x, y1 = r.position.y;
            const float x2 = x1 + r.size.x, y2 = y1 + r.size.y;
            return (p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2);
        }

        void cargarAssets();
        void armarPreguntas();
        void mostrarPregunta(int idx);
        void procesarRespuesta(int idxOpcion);
        void siguientePregunta();
        void salirDelMinijuego();

    private:
        sf::RenderWindow& m_window;
        Personaje&        m_personaje;

        // Texto
        sf::Font  m_font;
        sf::Text  m_txtPregunta;
        sf::Text  m_txtA, m_txtB, m_txtC, m_txtD, m_txtFeedback;

        // Sprites
        sf::Texture m_texFondo, m_texDialog, m_texMagikarp;
        sf::Sprite  m_sprFondo, m_sprDialog, m_sprMagikarp;

        // Círculos de pesca
        std::array<sf::CircleShape,4> m_circulos;

        // Audio
        sf::SoundBuffer m_bufBgm, m_bufFish, m_bufFail;
        sf::Sound       m_bgm, m_sndFish, m_sndFail;

        // Estado
        std::vector<Preguntas> m_pregs;
        int   m_idxPregunta = 0;
        int   m_opcCorrecta = 0;
        Fase  m_fase = Fase::Pregunta;

        // Feedback / animación
        bool        m_acerto = false;
        bool        m_showMagikarp = false;
        sf::Vector2f m_magiPos;
        float       m_magiVy = -90.f;
        sf::Clock   m_clock;
};

#endif // MINIJUEGOCIENCIA_H
