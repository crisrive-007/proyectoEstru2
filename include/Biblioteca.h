#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_set>
#include <memory>
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
    sf::RectangleShape m_areaPuerta1;
    sf::RectangleShape m_areaPuerta2;
    sf::RectangleShape m_areaPuerta3;
    sf::RectangleShape m_areaPuerta4;
    // UI/objetos
    Ruleta                        m_ruleta;
    sf::Clock                     m_clock;


    // Salida de este estado
    bool                          m_debeSalir = false;

    static bool intersecta(const sf::FloatRect& A, const sf::FloatRect& B) {
        return (A.position.x < B.position.x + B.size.x) &&
               (A.position.x + A.size.x > B.position.x) &&
               (A.position.y < B.position.y + B.size.y) &&
               (A.position.y + A.size.y > B.position.y);
    }
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
    void interaccionPuertas();
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
