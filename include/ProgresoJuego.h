#ifndef PROGRESOJUEGO_H
#define PROGRESOJUEGO_H

#pragma once

struct ProgresoJuego {
    bool bibliotecaCleared = false;
    bool gimnasioCleared   = false;
    bool kantCleared       = false;
    bool descartesCleared  = false;

    enum class Nivel { Biblioteca, Gimnasio, Kant, Descartes };

    static ProgresoJuego& get() {
        static ProgresoJuego g;
        return g;
    }

    bool puedeEntrar(Nivel n) const {
        switch (n) {
            case Nivel::Biblioteca: return true; // siempre
            case Nivel::Gimnasio:   return bibliotecaCleared;
            case Nivel::Kant:       return gimnasioCleared;      // Kant y Descartes al mismo tiempo
            case Nivel::Descartes:  return gimnasioCleared;
        }
        return false;
    }

    void marcarCleared(Nivel n) {
        switch (n) {
            case Nivel::Biblioteca: bibliotecaCleared = true; break;
            case Nivel::Gimnasio:   gimnasioCleared   = true; break;
            case Nivel::Kant:       kantCleared       = true; break;
            case Nivel::Descartes:  descartesCleared  = true; break;
        }
    }
};

#endif // PROGRESOJUEGO_H
