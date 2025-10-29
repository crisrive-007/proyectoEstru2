#ifndef MINIJUEGOARTE_H
#define MINIJUEGOARTE_H

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <queue>
#include <array>
#include <string>

struct Pregunta {
    std::string pregunta;
    std::array<std::string, 4> opciones;
    int correcta = 0;
};

class MinijuegoArte {
public:
    enum class Estado {Jugando, Gano, Perdio};

    explicit MinijuegoArte(sf::RenderWindow& window);

    void manejarEventos(const sf::Event& ev);
    void actualizar(float dt);
    void dibujar();

    void iniciarCombate();

    void reset();
    bool terminado() const ;
    Estado estado() const;

private:
    void cargarAssets();
    void armarPreguntas();
    void mostrarPreguntaActual();
    void chequearClick(const sf::Vector2f& mousePos);
    void procesarRespuesta(int idxOpcion);
    void siguientePregunta();

    sf::RenderWindow& m_window;

    sf::Font m_font;
    //sf::SoundBuffer m_selBuf;
    //sf::Sound m_selSnd;

    sf::Texture m_texBg, m_texDialog, m_texPika, m_texChari;
    sf::Texture m_texBtnA, m_texBtnB, m_texBtnC, m_texBtnD;
    sf::Texture m_texHP;

    sf::Sprite m_bg, m_dialog, m_pika, m_chari;
    sf::Sprite m_btnA, m_btnB, m_btnC, m_btnD;
    sf::Sprite m_hpPlayer, m_hpEnemy;

    static constexpr int m_hpFrames = 6;
    int m_idxHPPlayer = 0;
    int m_idxHPEnemy = 0;

    sf::Text m_txtPregunta;
    sf::Text m_txtA, m_txtB, m_txtC, m_txtD;
    sf::Text m_txtFeedback;

    sf::FloatRect m_rectDialog;
    sf::FloatRect m_rectLineaA, m_rectLineaB, m_rectLineaC, m_rectLineaD;

    std::queue<Pregunta> m_queue;
    std::optional<Pregunta> m_actual;
    Estado m_estado = Estado::Jugando;
    bool m_bloquearInput = false;
    float m_timerFeedback = 0.f;
};

#endif // MINIJUEGOARTE_H
