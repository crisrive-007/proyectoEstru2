#ifndef MINIJUEGOPOLITICA_H
#define MINIJUEGOPOLITICA_H

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <array>
#include <string>
#include "Estado.h"
#include "Personaje.h"

struct PreguntaPB {
    std::string enunciado;
    std::array<std::string,4> opciones;
    int correcta = 0; // 0..3
};

class MinijuegoPolitica : public Estado {
public:
    enum class Fase { Pregunta = 0, Feedback, Fin };

    MinijuegoPolitica(GestorEstados* g, sf::RenderWindow& w, Personaje& p);

    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;

    void iniciar();
    void salir();

private:
    // SFML 3: FloatRect usa left/top/width/height
    static bool containsPoint(const sf::FloatRect& r, const sf::Vector2f& p) {
        return (p.x >= r.position.x) && (p.x <= r.position.y + r.size.x) &&
               (p.y >= r.position.y) && (p.y <= r.position.y + r.size.y);
    }

    void cargarAssets();
    void armarPreguntas();
    void mostrarPregunta(int idx);
    void resolverClick(int idxPokebola);
    void siguientePregunta();

private:
    // Refs
    sf::RenderWindow& m_window;
    Personaje&        m_personaje;

    // Assets
    sf::Texture  m_texFondo, m_texDialog, m_texPokebola, m_texEevee, m_texGengar;
    sf::Font     m_font;

    // Sprites fijos
    sf::Sprite   m_sprFondo, m_sprDialog;

    // UI texto superior
    sf::Text                 m_txtPregunta;
    std::vector<sf::Text>    m_txtOpc;    // 4 líneas (A-D)
    std::vector<sf::Text>    m_txtLetra;  // letras A/B/C/D centradas sobre cada bola

    // Bolas y revelos
    std::vector<sf::Sprite>  m_bolas;     // 4 pokébolas
    std::vector<sf::Sprite>  m_revelos;   // 4 Eevee/Gengar (alpha 0 hasta mostrar)

    // Sonidos
    sf::SoundBuffer m_bufCorrect, m_bufWrong, m_bufVictory;
    sf::Sound       m_sndCorrect, m_sndWrong, m_sndVictory;

    // Lógica
    std::vector<PreguntaPB> m_pregs;
    int   m_idx       = 0;
    int   m_correcta  = 0;
    int   m_seleccion = -1;
    Fase  m_fase      = Fase::Pregunta;
    std::array<int,4> m_overlayTipo{{-1,-1,-1,-1}}; // -1 nada, 0 Eevee, 1 Gengar

    // Layout / escalas
    float m_scaleBola   = 20.0f; // escala base pokébolas
    float m_scaleRevelo = 5.0f;  // escala base eevee/gengar
};

#endif // MINIJUEGOPOLITICA_H
