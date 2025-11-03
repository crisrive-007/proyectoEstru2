#ifndef MINIJUEGOHISTORIA_H
#define MINIJUEGOHISTORIA_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <vector>
#include <string>
#include "Estado.h"
#include "Personaje.h"

struct PreguntaS {
    std::string enunciado;
    std::array<std::string,4> opciones;
    int correcta;
};

class MinijuegoHistoria : public Estado
{
    public:
        enum class Fase { Pregunta = 0, Feedback, Fin };

        MinijuegoHistoria(GestorEstados* g, sf::RenderWindow& w, Personaje& p);

        void manejarEventos(sf::RenderWindow& window) override;
        void actualizar() override;
        void dibujar(sf::RenderWindow& window) override;

        void iniciar();
        void salir();
        virtual ~MinijuegoHistoria();

    protected:

    private:
        void cargarAssets();
        void armarPreguntas();
        void mostrarPregunta(int idx);
        void resolverClick(int idxBaya);
        void siguientePregunta();

        sf::RenderWindow& m_window;
        Personaje&        m_personaje;

        // Assets
        sf::Texture m_texFondo, m_texDialog, m_texSnorlax;
        std::array<sf::Texture,4> m_texBayas; // 4 tipos
        sf::Font    m_font;

        // Sprites
        sf::Sprite m_sprFondo, m_sprDialog, m_sprSnorlax;
        std::vector<sf::Sprite> m_bayas;

        // Textos
        sf::Text m_txtPregunta;
        std::vector<sf::Text> m_txtOpciones;
        std::vector<sf::Text> m_txtLetras;

        // Sonidos
        sf::SoundBuffer m_bufEat, m_bufVomit, m_bufVictory;
        sf::Sound       m_sndEat, m_sndVomit, m_sndVictory;

        // Lógica
        std::vector<PreguntaS> m_pregs;
        int m_idx       = 0;
        int m_correcta  = 0;
        int m_seleccion = -1;
        Fase m_fase     = Fase::Pregunta;
};

#endif // MINIJUEGOHISTORIA_H
