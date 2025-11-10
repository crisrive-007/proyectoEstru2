#ifndef MENU_H
#define MENU_H

#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include "Estado.h"

class Menu final : public Estado {
public:
    enum Opcion { NuevaPartida = 0, CargarPartida = 1, NUM_OPCIONES = 2 };

    Menu(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje, const sf::Font* fontOpt = nullptr);
    ~Menu() override = default;

    // Contrato de Estado
    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;

    // Personalización
    void setTitle(const std::string& titulo);
    void setButtonText(Opcion op, const std::string& txt);

private:
    // Referencias
    sf::RenderWindow& m_window;
    Personaje&        m_personaje;

    // Recursos
    sf::Font        m_ownedFont;
    const sf::Font* m_font = nullptr;

    // UI
    sf::Text m_title;
    std::array<sf::RectangleShape, NUM_OPCIONES> m_buttons;
    std::array<sf::Text,            NUM_OPCIONES> m_labels;

    // Input de nombre
    bool        m_pidiendoNombre = false;
    std::string m_nombreJugador;
    sf::Text    m_textoNombre;
    sf::RectangleShape m_cajaNombre;

    // Feedback guardado/cargado
    sf::Text    m_feedback;
    sf::Clock   m_feedbackClock;
    bool        m_mostrarFeedback = false;

    // Estado de selección
    int m_selected = 0;

    // Helpers
    void layout();
    void applyStyles();
    void selectNext(int dir);
    void activateCurrent();
    bool isMouseOver(const sf::RectangleShape& shape) const;
    void iniciarInputNombre();
    void manejarInputNombre(const sf::Event& ev);

    // Transiciones / persistencia
    void crearYEmpujarMapaNuevaPartida();
    void crearYEmpujarMapaCargarPartida();
    void guardarPartida();

    void mostrarFeedback(const std::string& msg);
};

#endif // MENU_H
