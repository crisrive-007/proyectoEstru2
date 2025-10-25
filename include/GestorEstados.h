#ifndef GESTORESTADOS_H
#define GESTORESTADOS_H

#include <SFML/Graphics.hpp>
#include <stack>
#include <memory>
#include "Estado.h"

class Estado;
class Personaje;

class GestorEstados
{
    public:
        GestorEstados(sf::RenderWindow& window, Personaje& personaje);
        void empujarEstado(std::unique_ptr<Estado> nuevoEstado);
        void sacarEstado();
        void cambiarEstado(std::unique_ptr<Estado> nuevoEstado);

        Estado* obtenerEstadoActual();

        void manejarEventos();
        void actualizar();
        void dibujar();
        virtual ~GestorEstados();

    protected:

    private:
        std::stack<std::unique_ptr<Estado>> estados;

        sf::RenderWindow& window;

        Personaje& personaje;
};

#endif // GESTORESTADOS_H
