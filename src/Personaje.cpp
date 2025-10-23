#include "Personaje.h"
#include <iostream>
#include <string>

const std::string QUIETO_PATH = "assets/Player/idle.png";
const std::string CAMINAR_PATH = "assets/Player/run.png";
const std::string ATACAR_PATH = "assets/Player/combat_idle.png";
const std::string HERIDO_PATH = "assets/Player/hurt.png";
const std::string CELEBRAR_PATH = "assets/Player/emote.png";

Personaje::Personaje(float vel)
    : sprite(quieto), velocidad(vel), ultima(Abajo) {

    cargarTodasLasTexturas();

    currentFrame = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(frameWidth, frameHeight));

    sprite = sf::Sprite(quieto);
    sprite.setTextureRect(currentFrame);

    setEstado(Quieto);

    sprite.setPosition({400.f, 300.f});
}

void Personaje::cargarTodasLasTexturas() {
    if (!quieto.loadFromFile(QUIETO_PATH)) {
        std::cerr << "Error al cargar idle.png" << std::endl;
    }
    if (!caminar.loadFromFile(CAMINAR_PATH)) {
        std::cerr << "Error al cargar walk.png" << std::endl;
    }
    if (!atacar.loadFromFile(ATACAR_PATH)) {
        std::cerr << "Error al cargar combat_idle.png" << std::endl;
    }
    if (!herido.loadFromFile(HERIDO_PATH)) {
        std::cerr << "Error al cargar hurt.png" << std::endl;
    }
    if (!celebrar.loadFromFile(CELEBRAR_PATH)) {
        std::cerr << "Error al cargar emote.png" << std::endl;
    }
}

void Personaje::setEstado(Estado nuevoEstado) {
    actual = nuevoEstado;

    switch (nuevoEstado) {
        case Quieto:
            sprite.setTexture(quieto);
            break;
        case Caminar:
            sprite.setTexture(caminar);
            break;
        case Atacar:
            sprite.setTexture(atacar);
            break;
        case Herido:
            sprite.setTexture(herido);
            break;
        case Celebrar:
            sprite.setTexture(celebrar);
            break;
        default:
            sprite.setTexture(quieto);
    }
    currentFrame.position.x = 0;
    sprite.setTextureRect(currentFrame);
}

int Personaje::getFilaIndex() const {
    return ultima;
}

void Personaje::mover() {
    bool seMueve = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        sprite.move({0.f, -velocidad}); ultima = Arriba; seMueve = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        sprite.move({0.f, velocidad}); ultima = Abajo; seMueve = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        sprite.move({-velocidad, 0.f}); ultima = Izquierda; seMueve = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        sprite.move({velocidad, 0.f}); ultima = Derecha; seMueve = true;
    }

    if (seMueve && actual != Caminar) {
        setEstado(Caminar);
    } else if (!seMueve && actual == Caminar) {
        setEstado(Quieto);
    }

    sprite.setScale({1.f, 1.f});
    sprite.setOrigin({0.f, 0.f});
}

void Personaje::actualizarAnimacion() {
    currentFrame.position.y = getFilaIndex() * frameHeight;

    if (animClock.getElapsedTime().asSeconds() > animSpeed) {

        int maxFrames = 1;

        switch (actual) {
            case Caminar:
                maxFrames = 8;
                break;
            case Quieto:
                maxFrames = 2;
                break;
            case Atacar:
                maxFrames = 2;
                break;
            default:
                maxFrames = 1;
        }

        currentFrame.position.x += frameWidth;
        if (currentFrame.position.x >= frameWidth * maxFrames) {
            currentFrame.position.x = 0;
        }

        sprite.setTextureRect(currentFrame);
        animClock.restart();
    }

    sprite.setTextureRect(currentFrame);
}

void Personaje::actualizar()
 {
     mover();
     actualizarAnimacion();
 }
void Personaje::dibujar(sf::RenderWindow& ventana) {
    ventana.draw(sprite);
}

sf::Vector2f Personaje::obtenerPosicion() const {
    return sprite.getPosition();
}

void Personaje::establecerPosicion(sf::Vector2f posicion) {
    sprite.setPosition(posicion);
}

void Personaje::limitarBordes(float anchoVentana, float altoVentana) {
    sf::FloatRect limites = sprite.getGlobalBounds();

    if (limites.position.x < 0) {
        sprite.setPosition({0.f, limites.position.y});
    }
    if (limites.position.x + limites.size.x > anchoVentana) {
        sprite.setPosition({anchoVentana - limites.size.x, limites.position.y});
    }
    if (limites.position.y < 0) {
        sprite.setPosition({limites.position.x, 0.f});
    }
    if (limites.position.y + limites.size.y > altoVentana) {
        sprite.setPosition({limites.position.x, altoVentana - limites.size.y});
    }
}

sf::FloatRect Personaje::obtenerLimites() const {
    return sprite.getGlobalBounds();
}

sf::FloatRect Personaje::obtenerHitbox() const
{
    sf::FloatRect bounds = sprite.getGlobalBounds();

    // Ajustar la hitbox para que sea más precisa (solo los pies/parte inferior del personaje)
    float hitboxWidth = bounds.size.x * 1.f;   // 50% del ancho
    float hitboxHeight = bounds.size.y * 1.f;  // 30% de la altura (parte inferior)

    float hitboxX = bounds.position.x + (bounds.size.x - hitboxWidth) / 2.0f;  // Centrar horizontalmente
    float hitboxY = bounds.position.y + bounds.size.y - hitboxHeight;  // Posicionar en la parte inferior

    return sf::FloatRect(sf::Vector2f(hitboxX, hitboxY), sf::Vector2f(hitboxWidth, hitboxHeight));
}

Personaje::~Personaje()
{
    //dtor
}
