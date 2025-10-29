#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_set>
#include "TileMap.h"
#include "Personaje.h"
#include "MapaPrincipal.h"
#include "Estado.h"
#include "Ruleta.h"
#include "MinijuegoArte.h"
#include "BancoPreguntas.h"

class Biblioteca : public Estado {
private:
    sf::RenderWindow& m_window;
    Personaje&        m_personaje;

    // Tilemap
    std::vector<int>              m_tilesBase;
    std::unordered_set<int>       m_tilesValidos;
    unsigned int                  m_ancho;
    unsigned int                  m_alto;
    TileMap                       m_tilemapBase;

    // Áreas
    sf::RectangleShape            m_areaSalida;
    std::array<sf::RectangleShape,4> puertas{};
    std::array<bool,4>            colisionandoAntes{false, false, false, false};
    std::vector<sf::RectangleShape> m_areasExtra; // opcional, para debug

    // UI/objetos
    Ruleta                        m_ruleta;
    sf::Clock                     m_clock;

    // Minijuego (objeto directo, no unique_ptr)
    MinijuegoArte                 m_arteJuego;
    bool                          m_minijuegoActivo;

    // Salida de este estado
    bool                          m_debeSalir = false;
public:
    Biblioteca(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje);
    ~Biblioteca();

    // Ciclo de Estado
    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;

    // Utilidades
    void inicializarDatosMapa();
    void ejecutarMapa(); // carga del tilemap
    void interaccionRuleta();
    void iniciarMinijuegoPokePreguntas();
    void setTilesValidos(const std::unordered_set<int>& nuevosTilesValidos) { m_tilesValidos = nuevosTilesValidos; }

    // Salir
    bool debeSalir() const { return m_debeSalir; }
    void debeSalir(bool v) { m_debeSalir = v; }

    // Accesores
    std::vector<int>& getTilesBase() { return m_tilesBase; }
    unsigned int getAncho() const { return m_ancho; }
    unsigned int getAlto() const { return m_alto; }
};

#endif
