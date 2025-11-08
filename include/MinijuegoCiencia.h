#ifndef MINIJUEGOCIENCIA_H
#define MINIJUEGOCIENCIA_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <string>
#include <vector>
#include <queue>
#include "Estado.h"
#include "Personaje.h"
#include "Pregunta.h"

class MinijuegoCiencia : public Estado
{
public:
    enum class Fase {Pregunta, Lanzamiento, Feedback, Fin };

    MinijuegoCiencia(GestorEstados* g, sf::RenderWindow& w, Personaje& p);

    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;

    void iniciar();
    virtual ~MinijuegoCiencia();

private:
    static bool containsPoint(const sf::FloatRect& r, const sf::Vector2f& p) {
        const float x1 = r.position.x, y1 = r.position.y;
        const float x2 = x1 + r.size.x, y2 = y1 + r.size.y;
        return (p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2);
    }

    void cargarAssets();
    void armarPreguntas();
    void mostrarPregunta();
    void procesarRespuesta(int idxOpcion);
    void siguientePregunta();
    void salirDelMinijuego();

    // cuerda/anim
    void iniciarCuerda(const sf::Vector2f& origen, const sf::Vector2f& destino);
    void actualizarCuerda(float dt);
    void dibujarCuerda(sf::RenderWindow& window) const;
    void startAnimLanzamiento(bool loop);
    void actualizarAnimPescador(float dt);

private:
    sf::RenderWindow& m_window;
    Personaje&        m_personaje;

    // Texto
    sf::Font  m_font;
    sf::Text  m_txtPregunta;
    sf::Text  m_txtA, m_txtB, m_txtC, m_txtD, m_txtFeedback;
    sf::Text  m_txtHUD;                 // <-- HUD Magikarps

    // Sprites
    sf::Texture m_texFondo, m_texDialog, m_texMagikarp;
    sf::Sprite  m_sprFondo, m_sprDialog, m_sprMagikarp;

    sf::Texture m_texPescador;     // sprite sheet 4 frames
    sf::Sprite  m_sprPescador;
    std::vector<sf::IntRect> m_framesPesc;
    sf::Vector2i m_frameSize {32, 32}; // personaje.png es 128x32 => 4 frames de 32x32
    int   m_frameIdx  = 0;
    float m_frameTime = 0.f;
    float m_frameDur  = 0.09f;     // 90 ms
    bool  m_animLanzando = false;  // true mientras lanza
    bool  m_animLoop     = false;  // no loopeamos (false)

    // Cuerda
    bool         m_cuerdaActiva = false;
    float        m_cuerdaT      = 0.f;        // 0..1 progreso de extensión
    float        m_cuerdaVel    = 2.2f;       // velocidad de despliegue (ajusta)
    sf::Vector2f m_cuerdaA;                   // origen (mano del pescador)
    sf::Vector2f m_cuerdaB;                   // destino (centro del círculo)
    sf::VertexArray m_ropeVA;                 // polyline para dibujar
    int           m_objetivoIdx = -1;

    // Círculos de pesca
    std::array<sf::CircleShape,4> m_circulos;

    // Audio
    sf::SoundBuffer m_bufBgm, m_bufFish, m_bufFail;
    sf::Sound       m_bgm, m_sndFish, m_sndFail;

    // Estado de preguntas
    std::queue<Pregunta> m_pregs;
    Pregunta m_pregActual;
    int   m_opcCorrecta = 0;
    Fase  m_fase = Fase::Pregunta;

    // Puntaje / progreso
    int   m_pescados = 0;          // <-- Magikarps pescados
    const int m_total = 5;         // total de preguntas

    // Feedback / animación
    bool         m_showMagikarp = false;
    sf::Vector2f m_magiPos;
    float        m_magiVy = -90.f;
    sf::Clock    m_clock;
};

#endif // MINIJUEGOCIENCIA_H
