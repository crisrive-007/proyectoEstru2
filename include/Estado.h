#ifndef ESTADO_H
#define ESTADO_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "Personaje.h"
#include "GestorEstados.h"

class GestorEstados;

class Estado
{
    public:
        GestorEstados* gestor;
        Personaje& personaje;
        Estado(GestorEstados* gestor, Personaje& personaje) : gestor(gestor), personaje(personaje) {}
        virtual ~Estado() {}

        virtual void manejarEventos(sf::RenderWindow& window) = 0;
        virtual void actualizar() = 0;
        virtual void dibujar(sf::RenderWindow& window) = 0;

    protected:

    private:
};

#endif // ESTADO_H
