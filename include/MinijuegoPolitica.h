#ifndef MINIJUEGO_POLITICA_H
#define MINIJUEGO_POLITICA_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <queue>
#include <string>
#include <vector>

#include "Estado.h"
#include "Personaje.h"
#include "Pregunta.h"

class MinijuegoPolitica : public Estado {
public:
    enum class Fase { EsperaSeleccion, Excavando, Feedback, Fin };

    MinijuegoPolitica(GestorEstados* g, sf::RenderWindow& w, Personaje& p);
    ~MinijuegoPolitica() override = default;

    void iniciar();
    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;

private:
    // ==== utilidades ====
    void cargarAssets();
    void configurarFondo();
    void armarPreguntas();
    void cargarPreguntaActual();
    void layoutDialogUI();                 // coloca y estiliza todos los textos
    void procesarSeleccion(int idx);
    void resolverResultado(bool correcto);
    void terminar(const std::string& msg);
    void salirDelMinijuego();
    void limpiarOpciones();

    // animación
    void startAnimExcavar(const sf::Vector2f& posMonticulo);
    void actualizarAnimSandslash(float dt);

    static bool containsPoint(const sf::FloatRect& r, const sf::Vector2f& p) {
        const float x1 = r.position.x, y1 = r.position.y;
        const float x2 = x1 + r.size.x, y2 = y1 + r.size.y;
        return (p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2);
    }

    // ==== contexto ====
    sf::RenderWindow& m_window;
    Personaje&        m_personaje;

    // ==== estado general ====
    Fase                   m_fase = Fase::EsperaSeleccion;
    std::queue<Pregunta>   m_cola;
    Pregunta               m_actual;
    int                    m_correctas = 0;
    int                    m_fallos    = 0;
    bool                   m_excavando = false;

    // ==== recursos gráficos ====
    sf::Texture m_texFondo, m_texDialog, m_texMonticulo, m_texFosil, m_texZubat, m_texSandslash;
    sf::Sprite  m_sprFondo, m_sprDialog, m_sprFosil, m_sprZubat, m_sprSandslash;

    // Ahora con vector 👇
    std::vector<sf::Sprite>   m_monticulos;
    std::vector<sf::Vector2f> m_posMonticulos;
    // Montículos activos/inactivos y posición inicial del monito
    std::vector<bool>  m_monticuloActivo;
    sf::Vector2f       m_posInicialSandslash{0.f,0.f};

    // animación sandslash
    std::vector<sf::IntRect> m_framesSandslash;
    sf::Vector2i  m_frameSize{32, 32};
    int           m_frameIdx  = 0;
    float         m_frameTime = 0.f;
    float         m_frameDur  = 0.11f;
    sf::Vector2f  m_objetivoActual{0.f,0.f};   // punto frente al montículo clicado
    int m_idxSeleccion = -1;
    bool m_pendienteFin = false;

    // ==== textos ====
    sf::Font m_font;
    sf::Text m_txtPregunta, m_txtA, m_txtB, m_txtC, m_txtD, m_txtFeedback, m_txtFin, m_txtHUD;
    std::vector<sf::Text> m_txtLetras;

    // ==== sonidos ====
    sf::SoundBuffer m_bufBgm, m_bufCorrect, m_bufWrong, m_bufDig;
    sf::Sound       m_bgm, m_sndCorrect, m_sndWrong, m_sndDig;

    // reloj / temporales
    sf::Clock m_clk;
    float     m_timerZubat = 0.f;
    bool      m_zubatActivo = false;

    // helpers
    void setTextoCentro(sf::Text& t, float x, float y) {
        auto b = t.getGlobalBounds();
        t.setPosition({x - b.size.x/2.f, y - b.size.y/2.f});
    }
};

#endif
