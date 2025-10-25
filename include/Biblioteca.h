#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_set>
#include "TileMap.h"
#include "Personaje.h"
#include "MapaPrincipal.h"
#include "Estado.h"

class Biblioteca : public Estado {
private:
    sf::RenderWindow& m_window;
    Personaje& m_personaje;

    std::vector<int> m_tilesBase;
    std::unordered_set<int> m_tilesValidos;
    unsigned int m_ancho;
    unsigned int m_alto;
    TileMap m_tilemapBase;

    // Área de salida (trigger para volver al mapa principal)
    sf::RectangleShape m_areaSalida;

public:
    Biblioteca(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje);
    ~Biblioteca();

    void inicializarDatosMapa();
    void ejecutarMapa();
    void dibujar();
    void setTilesValidos(const std::unordered_set<int>& nuevosTilesValidos);

    void ejecutarBiblioteca();
    void manejarEventos();

    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;

    std::vector<int>& getTilesBase() { return m_tilesBase; }
    unsigned int getAncho() const { return m_ancho; }
    unsigned int getAlto() const { return m_alto; }

    bool debeSalir() const;
};

#endif
