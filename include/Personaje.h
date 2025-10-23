#ifndef PERSONAJE_H
#define PERSONAJE_H
#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

class Personaje
{
    public:
        // PRIMERO: Declarar los enums
        enum Direccion {Abajo = 2, Izquierda = 1, Derecha = 3, Arriba = 0};
        enum Estado {Quieto, Caminar, Atacar, Herido, Celebrar};

        // DESPUÉS: Métodos y constructor
        Personaje(float vel);
        void mover();
        void actualizarAnimacion();
        void dibujar(sf::RenderWindow& ventana);
        void limitarBordes(float anchoVentana, float altoVentana);
        sf::FloatRect obtenerLimites() const;
        sf::FloatRect obtenerHitbox() const;
        void setEstado(Estado nuevoEstado);  // Ahora Estado ya está declarado
        sf::Vector2f obtenerPosicion() const;
        void establecerPosicion(sf::Vector2f posicion);
        void actualizar();
        virtual ~Personaje();

        // Variables públicas
        Direccion ultima = Abajo;
        Estado actual = Quieto;

    protected:

    private:
        sf::Texture quieto;
        sf::Texture caminar;
        sf::Texture atacar;
        sf::Texture herido;
        sf::Texture celebrar;
        sf::Sprite sprite;
        float velocidad;
        sf::IntRect currentFrame;
        const int frameWidth = 64;
        const int frameHeight = 64;
        sf::Clock animClock;
        const float animSpeed = 0.15f;
        void cargarTodasLasTexturas();
        int getFilaIndex() const;
};

#endif // PERSONAJE_H
