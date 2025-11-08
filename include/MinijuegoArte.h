#ifndef MINIJUEGO_ARTE_H
#define MINIJUEGO_ARTE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <string>
#include <vector>
#include <cmath>
#include <queue>

#include "Estado.h"
#include "Personaje.h"
#include "Pregunta.h"

class MinijuegoArte : public Estado {
public:
    enum class Fase { EsperaSeleccion, Lanzamiento, Feedback, Fin };

    MinijuegoArte(GestorEstados* g, sf::RenderWindow& w, Personaje& p);
    ~MinijuegoArte() override = default;

    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;

    void iniciar();

private:
    static bool containsPoint(const sf::FloatRect& r, const sf::Vector2f& p) {
        const float x1 = r.position.x, y1 = r.position.y;
        const float x2 = x1 + r.size.x, y2 = y1 + r.size.y;
        return (p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2);
    }

    void cargarAssets();
    void armarPreguntas();
    void cargarPreguntaActual();   // carga desde la cola al "m_actual"
    void procesarSeleccion(int idxDiana); // dispara shuriken hacia idxDiana
    void resolverImpacto();        // decide correcto/incorrecto y prepara feedback
    void siguiente();              // pasa a la siguiente pregunta o fin
    void terminar(const std::string& mensaje); // set Fin
    void salirDelMinijuego();

    void startAnimLanzamiento(bool loop);
    void actualizarAnimGreninja(float dt);

    // contexto
    sf::RenderWindow& m_window;
    Personaje&        m_personaje;

    // estado general
    Fase  m_fase = Fase::EsperaSeleccion;
    int   m_dianaObjetivo = -1;   // diana clickeada
    int   m_correctas = 0;
    int   m_fallosSeguidos = 0;
    bool  m_ultimoCorrecto = false;

    // preguntas (cola)
    std::queue<Pregunta> m_cola;
    Pregunta m_actual;

    // fuentes y textos
    sf::Font m_font;
    sf::Text m_txtPregunta;
    sf::Text m_txtA, m_txtB, m_txtC, m_txtD;
    sf::Text m_txtHUD;      // “Aciertos: x | Fallos seguidos: y”
    sf::Text m_txtFeedback; // “¡Correcto!” / “Incorrecto…”
    sf::Text m_txtFin;      // mensaje final

    // sprites/texturas
    sf::Texture m_texFondo, m_texDialog, m_texGreninja, m_texShuriken, m_texDiana, m_texExplosion;
    sf::Sprite  m_sprFondo, m_sprDialog, m_sprGreninja, m_sprShuriken;
    std::vector<sf::Sprite> m_dianas;

    // Contador de fallos totales (para decidir win/lose al final)
    int   m_fallosTotales = 0;

    // Letras sobre cada diana
    std::vector<sf::Text> m_txtLetras;


    std::vector<sf::IntRect> m_framesGreninja;
    sf::Vector2i m_frameSize {120, 90}; // ancho/alto de cada frame en el sheet
    int   m_frameIdx  = 0;
    float m_frameTime = 0.f;
    float m_frameDur  = 0.09f;     // 90 ms por frame
    bool  m_animLanzando = false;  // true mientras lanza (vuelo del shuriken)
    bool  m_animLoop     = false;

    // posiciones y animaciones
    std::array<sf::Vector2f,4> m_posDianas;
    sf::Vector2f m_posGreninja;
    sf::Vector2f m_posShuriken;
    sf::Vector2f m_velShuriken;
    float        m_speedShuriken = 1200.f;
    bool         m_shurikenEnVuelo = false;

    // anim de "explosión"
    bool   m_explota = false;
    int    m_dianaQueExplota = -1;
    float  m_exploTimer = 0.f;     // segundos
    float  m_exploDur = 0.35f;     // duración
    float  m_exploScaleMax = 1.8f; // escala máx del flash
    sf::CircleShape m_flash;       // flash si no hay hoja de explosión

    // audio
    sf::SoundBuffer m_bufBgm, m_bufThrow, m_bufCorrect, m_bufWrong;
    sf::Sound       m_bgm, m_sndThrow, m_sndCorrect, m_sndWrong;

    // reloj
    sf::Clock m_clk;

    // util
    void setTextoCentro(sf::Text& t, float x, float y) {
        auto b = t.getGlobalBounds();
        t.setPosition({x - b.size.x/2.f, y - b.size.y/2.f});
    }
};

#endif
