#ifndef SAVEGAMEMANAGER_H
#define SAVEGAMEMANAGER_H
#include <string>
class Personaje;
struct ProgresoJuego;

namespace SaveGame {
bool guardarJuego(const Personaje& pj, const ProgresoJuego& prog,
                  const std::string& rutaPersonaje="save_personaje.bin",
                  const std::string& rutaProgreso ="save_progreso.bin");
bool cargarJuego(Personaje& pj, ProgresoJuego& prog,
                 const std::string& rutaPersonaje="save_personaje.bin",
                 const std::string& rutaProgreso ="save_progreso.bin");
}
#endif
