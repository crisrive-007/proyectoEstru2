#ifndef SAVEGAMEMANAGER_H
#define SAVEGAMEMANAGER_H

#include <string>
#include <fstream>
#include "ProgresoJuego.h"
#include "Personaje.h"

struct DatosJuego {
    bool bibliotecaCleared = false;
    bool gimnasioCleared   = false;
    bool kantCleared       = false;
    bool descartesCleared  = false;
};

class SaveGame {
private:
    static const std::string RUTA_ARCHIVO;

public:
    static bool guardarJuego(const Personaje& personaje, const ProgresoJuego& progreso);

    static bool cargarJuego(Personaje& personaje, ProgresoJuego& progreso);
};

#endif // SAVEGAMEMANAGER_H
