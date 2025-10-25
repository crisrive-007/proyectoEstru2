#include "GestorEstados.h"
#include <iostream>
#include <utility>

GestorEstados::GestorEstados(sf::RenderWindow& window, Personaje& personaje) : window(window), personaje(personaje)
{
    //ctor
}

void GestorEstados::empujarEstado(std::unique_ptr<Estado> nuevoEstado) {
    estados.push(std::move(nuevoEstado));
}

void GestorEstados::sacarEstado() {
    if(!estados.empty()) {
        estados.pop();
    }
}

void GestorEstados::cambiarEstado(std::unique_ptr<Estado> nuevoEstado) {
    if(!estados.empty()) {
        estados.pop();
    }
    estados.push(std::move(nuevoEstado));
}

void GestorEstados::manejarEventos() {
    if(!estados.empty()) {
        estados.top()->manejarEventos(window);
    }
}

void GestorEstados::actualizar() {
    if(!estados.empty()) {
        estados.top()->actualizar();
    }
}

void GestorEstados::dibujar() {
    if(!estados.empty()) {
        estados.top()->dibujar(window);
    }
}

GestorEstados::~GestorEstados()
{
    //dtor
}
