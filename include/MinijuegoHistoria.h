#ifndef MINIJUEGOHISTORIA_H
#define MINIJUEGOHISTORIA_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <vector>
#include <string>
#include <queue>
#include <optional>

#include "Estado.h"
#include "Personaje.h"
#include "Pregunta.h"

class MinijuegoHistoria : public Estado {
public:
    enum class Fase { Pregunta = 0, Feedback, Fin };

    MinijuegoHistoria(GestorEstados* g, sf::RenderWindow& w, Personaje& p);

    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;

    void iniciar();
    void salir();
    virtual ~MinijuegoHistoria();

private:
    // Assets / construcción de escena
    void cargarAssets();
    void armarPreguntas();     // llena la cola
    void mostrarPregunta();    // usa m_pregActual
    void resolverClick(int idxBaya);
    void siguientePregunta();  // pop de la cola → m_pregActual

    void limpiarOpciones();    // borra textos A/B/C/D y letras bajo bayas
    void actualizarHUD();

    // Referencias externas
    sf::RenderWindow& m_window;
    Personaje&        m_personaje;

    // Assets
    sf::Texture m_texFondo, m_texDialog, m_texSnorlax;
    std::array<sf::Texture,4> m_texBayas;
    sf::Font    m_font;

    // Sprites
    sf::Sprite m_sprFondo, m_sprDialog, m_sprSnorlax;
    std::vector<sf::Sprite> m_bayas;   // 4 bayas

    // Textos
    sf::Text m_txtPregunta;
    std::vector<sf::Text> m_txtOpciones; // A/B/C/D en el cuadro
    std::vector<sf::Text> m_txtLetras;   // A, B, C, D bajo cada baya

    // NUEVO: HUD
    sf::Text m_txtHUD;                   // “Bayas comidas: N”

    // Sonidos
    sf::SoundBuffer m_bufBgm, m_bufEat, m_bufVomit, m_bufVictory;
    sf::Sound       m_bgm, m_sndEat, m_sndVomit, m_sndVictory;

    // Lógica de juego
    std::queue<Pregunta>    m_pregs;      // cola FIFO
    std::optional<Pregunta> m_pregActual; // pregunta activa
    int   m_correcta  = 0;   // índice correcto [0..3]
    int   m_seleccion = -1;  // última selección
    Fase  m_fase      = Fase::Pregunta;

    // NUEVO: progreso
    int   m_bayasComidas = 0; // contador de aciertos
    const int m_total     = 5;
};

#endif // MINIJUEGOHISTORIA_H
