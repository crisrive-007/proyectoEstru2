#include "SaveGameManager.h"
#include "Personaje.h"
#include "ProgresoJuego.h"
#include <fstream>
#include <cstdint>

namespace {
#pragma pack(push,1)
struct BinProgresoV1 {
    int32_t version = 1;
    uint8_t bibl, gim, kant, desc;
};
#pragma pack(pop)

static bool guardarProgreso(const ProgresoJuego& p, const std::string& ruta){
    BinProgresoV1 d{};
    d.bibl = p.bibliotecaCleared; d.gim = p.gimnasioCleared;
    d.kant = p.kantCleared;       d.desc = p.descartesCleared;
    std::ofstream out(ruta, std::ios::binary);
    if(!out) return false;
    out.write(reinterpret_cast<const char*>(&d), sizeof(d));
    return static_cast<bool>(out);
}
static bool cargarProgreso(ProgresoJuego& p, const std::string& ruta){
    BinProgresoV1 d{};
    std::ifstream in(ruta, std::ios::binary);
    if(!in) return false;
    in.read(reinterpret_cast<char*>(&d), sizeof(d));
    if(!in || d.version!=1) return false;
    p.bibliotecaCleared=d.bibl; p.gimnasioCleared=d.gim;
    p.kantCleared=d.kant; p.descartesCleared=d.desc;
    return true;
}
}

namespace SaveGame {
bool guardarJuego(const Personaje& pj, const ProgresoJuego& prog,
                  const std::string& rp, const std::string& rg){
    bool okPj = pj.guardarEnBinario(rp);
    bool okPg = guardarProgreso(prog, rg);
    return okPj && okPg;
}
bool cargarJuego(Personaje& pj, ProgresoJuego& prog,
                 const std::string& rp, const std::string& rg){
    bool okPj = pj.cargarDesdeBinario(rp);
    bool okPg = cargarProgreso(prog, rg);
    return okPj && okPg;
}
}
