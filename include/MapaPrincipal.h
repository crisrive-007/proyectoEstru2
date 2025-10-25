#ifndef MAPAPRINCIPAL_H
#define MAPAPRINCIPAL_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_set>
#include "TileMap.h"
#include "Personaje.h"
#include "Biblioteca.h"
#include "Estado.h"

class MapaPrincipal : public Estado {
public:
    MapaPrincipal(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje);

    void inicializarDatosMapa();
    void ejecutarMapa();
    void dibujar();
    void setTilesValidos(const std::unordered_set<int>& nuevosTilesValidos);
    ~MapaPrincipal();

    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;

    bool verificarTransicion();

    std::vector<int>& getTilesBase() { return m_tilesBase; }
    std::vector<int>& getTilesObjetos() { return m_tilesObjetos; }
    unsigned int getAncho() const { return m_ancho; }
    unsigned int getAlto() const { return m_alto; }

private:
    sf::RenderWindow& m_window;
    Personaje& m_personaje; // Referencia al personaje del main
    std::vector<int> m_tilesBase;
    std::vector<int> m_tilesObjetos;
    std::unordered_set<int> m_tilesValidos;
    unsigned int m_ancho;
    unsigned int m_alto;
    TileMap m_tilemapBase;
    TileMap m_tilemapObjetos;
    sf::RectangleShape m_cuadradoBiblioteca;
};

#endif // MAPAPRINCIPAL_H
