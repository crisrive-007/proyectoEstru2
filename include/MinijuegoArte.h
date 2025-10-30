#ifndef MINIJUEGO_ARTE_H
#define MINIJUEGO_ARTE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <string>
#include <vector>
#include "Estado.h"
#include "Personaje.h"

// Si ya tienes tu BancoPreguntas con esta firma, usa ese;
// si no, dejamos un struct local de respaldo:
struct Pregunta {
    std::string enunciado;
    std::array<std::string,4> opciones;
    int correcta = 0; // 0=A, 1=B, 2=C, 3=D
};

class MinijuegoArte : public Estado {
public:
    enum class EstadoCombate { Jugando, Gano, Perdio };
    enum class Subestado {Pregunta, Feedback};

    MinijuegoArte(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje);

    // Ciclo de Estado (misma forma que en Biblioteca)
    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;

    // Control
    void iniciarCombate();   // resetea HUD + pone primera pregunta
    bool terminado() const { return m_estado != EstadoCombate::Jugando; }

    void startBGM();
    void stopBGM();
    void onWin();

    void salirDelMinijuego();
    void onLose();

private:
    // Helpers SFML3
    static bool containsPoint(const sf::FloatRect& r, const sf::Vector2f& p) {
        const float x1 = r.position.x, y1 = r.position.y;
        const float x2 = x1 + r.size.x, y2 = y1 + r.size.y;
        return (p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2);
    }

    void cargarAssets();
    void armarPreguntas();        // Puedes reemplazar con BancoPreguntas
    void mostrarPregunta(int idx);
    void procesarRespuesta(int idxOpcion);
    void siguientePregunta();

    // Referencias
    sf::RenderWindow& m_window;
    Personaje&        m_personaje;

    // Textos
    sf::Font   m_font;
    sf::Text   m_txtPregunta;
    sf::Text   m_txtOpcA, m_txtOpcB, m_txtOpcC, m_txtOpcD;

    // Sprites y texturas
    sf::Texture m_texFondo, m_texDialog, m_texPika, m_texChari;
    sf::Sprite  m_sprFondo, m_sprDialog, m_sprPika, m_sprChari;

    sf::Texture m_texBtnA, m_texBtnB, m_texBtnC, m_texBtnD;
    sf::Sprite  m_btnA,    m_btnB,    m_btnC,    m_btnD;

    // Vida jugador (5 segmentos → life5 lleno … life0 vacío)
    std::array<sf::Texture,6> m_texLife; // 0..5
    sf::Sprite  m_sprLife;

    // Estado del combate
    int m_vidaJugador = 5;
    EstadoCombate m_estado = EstadoCombate::Jugando;
    Subestado m_subestado = Subestado::Pregunta;

    int idxSeleccion = -1;

    sf::SoundBuffer m_bufBgm;
    sf::Sound m_bgm;

    sf::SoundBuffer m_bufAdvance;
    sf::Sound m_sndAdvance;

    sf::SoundBuffer m_bufVictory;
    sf::Sound m_sndVictory;

    // Preguntas
    std::vector<Pregunta> m_preguntas;
    int m_idxPregunta = 0;
    int m_respuestaCorrecta = 0;

    // Reloj
    sf::Clock m_clock;
    float m_shakePika  = 0.f;
    float m_shakeChari = 0.f;
    sf::Vector2f m_pikaBase, m_chariBase;
};

#endif
