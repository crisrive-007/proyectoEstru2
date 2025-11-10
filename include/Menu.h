#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <array>
#include <string>

class Menu {
public:
    enum Opcion { NuevaPartida = 0, CargarPartida = 1, NUM_OPCIONES = 2 };

    explicit Menu(sf::RenderWindow& window, const sf::Font* fontOpt = nullptr);

    // Callbacks
    std::function<void(const std::string&)> onNuevaPartida; // ← recibe nombre
    std::function<void()>                    onCargarPartida;

    // Loop
    void handleEvent(const sf::Event& ev);
    void update();
    void draw();

    // Visibilidad
    void setVisible(bool v) { m_visible = v; }
    bool isVisible() const { return m_visible; }

    // Personalización
    void setTitle(const std::string& titulo);
    void setButtonText(Opcion op, const std::string& txt);

private:
    // *** MANTENER ESTE ORDEN ***
    sf::RenderWindow& m_window;
    bool m_visible = true;

    // Recursos
    sf::Font        m_ownedFont;     // si no pasas font
    const sf::Font* m_font = nullptr;

    // UI
    sf::Text m_title;
    std::array<sf::RectangleShape, NUM_OPCIONES> m_buttons;
    std::array<sf::Text,            NUM_OPCIONES> m_labels;

    // Entrada de nombre
    bool        m_pidiendoNombre = false;
    std::string m_nombreJugador;
    sf::Text    m_textoNombre;
    sf::RectangleShape m_cajaNombre;

    // Estado
    int m_selected = 0;

    // Internos
    void layout();
    void applyStyles();
    void selectNext(int dir);
    void activateCurrent();
    bool isMouseOver(const sf::RectangleShape& shape) const;
    void iniciarInputNombre();
    void manejarInputNombre(const sf::Event& ev);
};

#endif
