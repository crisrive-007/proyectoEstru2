#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <SFML/Graphics.hpp>
#include "Personaje.h"

class Biblioteca {
private:
    sf::RenderWindow& m_window;
    Personaje& m_personaje;

    // Textura y sprite para el fondo de la biblioteca
    sf::Texture m_texturaFondo;
    sf::Sprite m_spriteFondo;

    // Área de salida (trigger para volver al mapa principal)
    sf::RectangleShape m_areaSalida;

public:
    Biblioteca(sf::RenderWindow& window, Personaje& personaje);
    ~Biblioteca();

    void ejecutarBiblioteca();
    void actualizar();
    void dibujar();

    // Método para verificar si el personaje quiere salir
    bool debeSalir() const;
};

#endif
