#ifndef RULETA_H
#define RULETA_H

#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>
#include <string>
#include <random>

class Ruleta
{
    public:
        Ruleta();
        // Carga el sprite de la “pantalla” y la fuente del texto
        bool cargar(const std::string& rutaPantalla, const std::string& rutaFuente);

        // Define la lista de opciones (en el orden en que se “barajarán”)
        void setOpciones(const std::vector<std::string>& opciones);

        void resetearOpciones();
        void eliminarOpcion(const std::string& texto);

        // Posiciona la pantalla (centra el texto en el interior)
        void setPosition(sf::Vector2f pos);

        // Control
        void iniciarGiro(float durMinSeg = 2.0f, float durMaxSeg = 3.5f); // arranca, elige random y empieza el “shuffle”
        void actualizar(float dt);  // animación del shuffle + frenado
        void dibujar(sf::RenderTarget& target) const;

        // Estado
        bool estaGirando() const { return m_rodando; }
        bool enReposo()   const { return !m_rodando; }

        // Resultado final (válido cuando enReposo()==true)
        int  getIndiceResultado() const { return m_indiceActual; }
        std::string getTextoResultado() const;

        // Dimensiones de la pantalla (para colocar en tu layout)
        sf::FloatRect getBounds() const { return m_sprPantalla.getGlobalBounds(); }
        void setEscalaPantalla(sf::Vector2f scl);

        // Personalización visual rápida
        void setColorTexto(const sf::Color& c);
        void setOutlineTexto(float grosor, const sf::Color& c);
        void setTamanioTexto(unsigned tam);

    protected:

    private:
        // Easing cúbico para el frenado (0..1 -> 0..1)
        static float easeOutCubic(float t);

        // Recalcula posición del texto para que esté centrado en la “pantalla”
        void centrarTexto();

        // Visual
        sf::Texture     m_texPantalla;
        sf::Sprite      m_sprPantalla;
        sf::Font        m_font;
        sf::Text        m_txt;

        std::vector<std::string> m_opcionesOriginales;
        std::deque<std::string>  m_opcionesActuales;

        // Shuffle & frenado
        bool   m_rodando = false;
        int    m_indiceActual = 0;
        int    m_indiceObjetivo = 0;

        float  m_tiempoTotal = 0.f;     // tiempo acumulado desde que empezó
        float  m_duracion = 0.f;         // cuánto durará el “shuffle” antes de permitir detener justo en objetivo
        float  m_tickAcum = 0.f;         // acumulador para avanzar índice por “ticks”
        float  m_tickActual = 0.f;       // periodo actual (se alarga progresivamente)

        // Config de ticks
        float  m_tickRapido = 0.05f;     // al inicio (rápido)
        float  m_tickLento  = 0.28f;     // al final (lento)

        // Aleatorio
        std::mt19937 m_rng;
};
#endif // RULETA_H
