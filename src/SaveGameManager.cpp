#include "SaveGameManager.h"
#include <iostream>

const std::string SaveGame::RUTA_ARCHIVO = "savegame.dat";
const std::string RUTA_PERSONAJE_DAT = "personaje.dat";

bool SaveGame::guardarJuego(const Personaje& personaje, const ProgresoJuego& progreso) {

    DatosJuego datos;
    datos.bibliotecaCleared = ProgresoJuego::get().bibliotecaCleared;
    datos.gimnasioCleared   = ProgresoJuego::get().gimnasioCleared;
    datos.kantCleared       = ProgresoJuego::get().kantCleared;
    datos.descartesCleared  = ProgresoJuego::get().descartesCleared;

    std::ofstream archivo(RUTA_ARCHIVO, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!archivo.is_open()) {
        std::cerr << "❌ SAVE ERROR: No se pudo abrir/crear " << RUTA_ARCHIVO << "\n";
        return false;
    }

    archivo.write(reinterpret_cast<const char*>(&datos), sizeof(DatosJuego));
    archivo.close();

    if (!personaje.guardarEnBinario(RUTA_PERSONAJE_DAT)) {
        std::cerr << "❌ SAVE ERROR: Fallo al guardar los datos del Personaje en " << RUTA_PERSONAJE_DAT << "\n";
        return false;
    }

    std::cout << "[SAVE] Datos de juego y personaje guardados exitosamente.\n";
    return true;
}

bool SaveGame::cargarJuego(Personaje& personaje, ProgresoJuego& progreso) {
    std::ifstream archivo(RUTA_ARCHIVO, std::ios::binary | std::ios::in);
    if (!archivo.is_open()) {
        std::cerr << "❌ LOAD ERROR: No se encontr el archivo de guardado: " << RUTA_ARCHIVO << "\n";
        return false;
    }

    DatosJuego datos;
    archivo.read(reinterpret_cast<char*>(&datos), sizeof(DatosJuego));
    archivo.close();

    ProgresoJuego::get().bibliotecaCleared = datos.bibliotecaCleared;
    ProgresoJuego::get().gimnasioCleared   = datos.gimnasioCleared;
    ProgresoJuego::get().kantCleared       = datos.kantCleared;
    ProgresoJuego::get().descartesCleared  = datos.descartesCleared;

    if (!personaje.cargarDesdeBinario(RUTA_PERSONAJE_DAT)) {
        std::cerr << "❌ LOAD ERROR: Fallo al cargar los datos del Personaje desde " << RUTA_PERSONAJE_DAT << "\n";
        return false;
    }

    std::cout << "[LOAD] Datos de juego y personaje cargados exitosamente.\n";
    return true;
}
