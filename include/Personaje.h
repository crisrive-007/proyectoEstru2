#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_set>

class Personaje {
public:
    enum Estado { Quieto, Caminar, Atacar, Herido, Celebrar };
    enum Direccion {Arriba = 0, Abajo = 2, Izquierda = 1, Derecha = 3};

    // Constructor modificado para recibir tiles válidos
    Personaje(float vel, const std::unordered_set<int>& tilesValidosParam);

    void cargarTodasLasTexturas();
    void setEstado(Estado nuevoEstado);
    int getFilaIndex() const;

    bool esPosicionValida(const sf::Vector2f& posicion, const std::vector<int>& tiles,
                        const std::vector<int>& objetos, unsigned int width, unsigned int height) const;
    bool esPosicionValida(const sf::Vector2f& posicion, const std::vector<int>& tiles,
                         unsigned int width, unsigned int height) const;

    void mover();
    void actualizarAnimacion();
    void mover(const std::vector<int>& tiles, const std::vector<int>& objetos,
              unsigned int mapWidth, unsigned int mapHeight);
    void mover(const std::vector<int>& tiles, unsigned int mapWidth, unsigned int mapHeight);
    void actualizarEnBiblioteca();
    void actualizar(const std::vector<int>& tiles, const std::vector<int>& objetos,
                   unsigned int mapWidth, unsigned int mapHeight);
    void actualizar(const std::vector<int>& tiles, unsigned int mapWidth, unsigned int mapHeight);
    void dibujar(sf::RenderWindow& ventana);

    sf::Vector2f obtenerPosicion() const;
    void establecerPosicion(sf::Vector2f posicion);
    void limitarBordes(float anchoVentana, float altoVentana);
    sf::FloatRect obtenerLimites() const;
    sf::FloatRect obtenerHitbox() const;
    void setTilesValidos(const std::unordered_set<int>& nuevosTilesValidos);
    void actualizarEnInterior();

    float getRadioColision();

    void setPosition(int x, int y);
    sf::Vector2f getPosition() const;

    /*static const std::string QUIETO_PATH;
    static const std::string CAMINAR_PATH;
    static const std::string ATACAR_PATH;
    static const std::string HERIDO_PATH;
    static const std::string CELEBRAR_PATH;*/

    ~Personaje();

private:
    sf::Texture quieto, caminar, atacar, herido, celebrar;
    sf::Sprite sprite;
    float velocidad;
    Estado actual;
    Direccion ultima;
    float radioColision;

    // Tiles válidos ahora se recibe como parámetro
    std::unordered_set<int> tilesValidos;

    // Constantes de animación
    static const int frameWidth = 64;
    static const int frameHeight = 64;
    static const float animSpeed;
    sf::IntRect currentFrame;
    sf::Clock animClock;

    static const std::string QUIETO_PATH;
    static const std::string CAMINAR_PATH;
    static const std::string ATACAR_PATH;
    static const std::string HERIDO_PATH;
    static const std::string CELEBRAR_PATH;
    //static const float animSpeed;
};

#endif // PERSONAJE_H
