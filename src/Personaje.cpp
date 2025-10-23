#include "Personaje.h"
#include <iostream>
#include <string>

const std::string Personaje::QUIETO_PATH = "assets/Player/idle.png";
const std::string Personaje::CAMINAR_PATH = "assets/Player/run.png";
const std::string Personaje::ATACAR_PATH = "assets/Player/combat_idle.png";
const std::string Personaje::HERIDO_PATH = "assets/Player/hurt.png";
const std::string Personaje::CELEBRAR_PATH = "assets/Player/emote.png";
const float Personaje::animSpeed = 0.1f;

Personaje::Personaje(float vel, const std::unordered_set<int>& tilesValidosParam)
    : sprite(quieto), velocidad(vel), ultima(Abajo), tilesValidos(tilesValidosParam) {

    cargarTodasLasTexturas();

    currentFrame = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(frameWidth, frameHeight));

    sprite = sf::Sprite(quieto);
    sprite.setTextureRect(currentFrame);

    setEstado(Quieto);

    sprite.setPosition({400.f, 300.f});

    // Ya no se inicializa tilesValidos por defecto, se usa el parámetro
    std::cout << "✅ Personaje creado con " << tilesValidos.size() << " tiles válidos" << std::endl;
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

void Personaje::setTilesValidos(const std::unordered_set<int>& nuevosTilesValidos) {
    tilesValidos = nuevosTilesValidos;
    std::cout << "✅ Tiles válidos actualizados: " << tilesValidos.size() << " tiles" << std::endl;
}

bool Personaje::esPosicionValida(const sf::Vector2f& posicion, const std::vector<int>& tiles,
                               const std::vector<int>& objetos, unsigned int width, unsigned int height) const {
    // Convertir posición mundial a coordenadas de tile (considerando el tamaño de tile 16x16)
    unsigned int tileX = static_cast<unsigned int>(posicion.x / 16);
    unsigned int tileY = static_cast<unsigned int>(posicion.y / 16);

    // Verificar límites del mapa
    if (tileX >= width || tileY >= height) {
        return false;
    }

    // Calcular índice en el arreglo
    unsigned int index = tileY * width + tileX;

    // Verificar condiciones
    int tileValue = tiles[index];
    int objetoValue = objetos[index];

    // El personaje puede caminar si el tile está en tilesValidos Y el objeto es 0
    bool tileValido = (tilesValidos.find(tileValue) != tilesValidos.end());
    bool objetoValido = (objetoValue == 2678 || objetoValue == 0);

    return tileValido && objetoValido;
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

void Personaje::mover(const std::vector<int>& tiles, const std::vector<int>& objetos, unsigned int mapWidth, unsigned int mapHeight) {
    bool seMueve = false;
    sf::Vector2f movimiento(0.f, 0.f);

    // Calcular movimiento deseado
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        movimiento.y = -velocidad;
        ultima = Arriba;
        seMueve = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        movimiento.y = velocidad;
        ultima = Abajo;
        seMueve = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        movimiento.x = -velocidad;
        ultima = Izquierda;
        seMueve = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        movimiento.x = velocidad;
        ultima = Derecha;
        seMueve = true;
    }

    // Normalizar movimiento diagonal
    if (movimiento.x != 0.f && movimiento.y != 0.f) {
        movimiento *= 0.7071f; // 1/sqrt(2)
    }

    // Aplicar movimiento con verificación de colisiones
    if (movimiento.x != 0.f) {
        sf::Vector2f nuevaPosX = sprite.getPosition();
        nuevaPosX.x += movimiento.x;

        // Verificar hitbox futura en X
        sf::FloatRect hitboxFuturaX = obtenerHitbox();
        hitboxFuturaX.position.x += movimiento.x;

        // Verificar las cuatro esquinas de la hitbox
        bool esquinaSuperiorIzquierda = esPosicionValida(
            {hitboxFuturaX.position.x, hitboxFuturaX.position.y},
            tiles, objetos, mapWidth, mapHeight
        );
        bool esquinaSuperiorDerecha = esPosicionValida(
            {hitboxFuturaX.position.x + hitboxFuturaX.size.x, hitboxFuturaX.position.y},
            tiles, objetos, mapWidth, mapHeight
        );
        bool esquinaInferiorIzquierda = esPosicionValida(
            {hitboxFuturaX.position.x, hitboxFuturaX.position.y + hitboxFuturaX.size.y},
            tiles, objetos, mapWidth, mapHeight
        );
        bool esquinaInferiorDerecha = esPosicionValida(
            {hitboxFuturaX.position.x + hitboxFuturaX.size.x, hitboxFuturaX.position.y + hitboxFuturaX.size.y},
            tiles, objetos, mapWidth, mapHeight
        );

        if (esquinaSuperiorIzquierda && esquinaSuperiorDerecha &&
            esquinaInferiorIzquierda && esquinaInferiorDerecha) {
            sprite.move({movimiento.x, 0.f});
        }
    }

    if (movimiento.y != 0.f) {
        sf::Vector2f nuevaPosY = sprite.getPosition();
        nuevaPosY.y += movimiento.y;

        // Verificar hitbox futura en Y
        sf::FloatRect hitboxFuturaY = obtenerHitbox();
        hitboxFuturaY.position.y += movimiento.y;

        // Verificar las cuatro esquinas de la hitbox
        bool esquinaSuperiorIzquierda = esPosicionValida(
            {hitboxFuturaY.position.x, hitboxFuturaY.position.y},
            tiles, objetos, mapWidth, mapHeight
        );
        bool esquinaSuperiorDerecha = esPosicionValida(
            {hitboxFuturaY.position.x + hitboxFuturaY.size.x, hitboxFuturaY.position.y},
            tiles, objetos, mapWidth, mapHeight
        );
        bool esquinaInferiorIzquierda = esPosicionValida(
            {hitboxFuturaY.position.x, hitboxFuturaY.position.y + hitboxFuturaY.size.y},
            tiles, objetos, mapWidth, mapHeight
        );
        bool esquinaInferiorDerecha = esPosicionValida(
            {hitboxFuturaY.position.x + hitboxFuturaY.size.x, hitboxFuturaY.position.y + hitboxFuturaY.size.y},
            tiles, objetos, mapWidth, mapHeight
        );

        if (esquinaSuperiorIzquierda && esquinaSuperiorDerecha &&
            esquinaInferiorIzquierda && esquinaInferiorDerecha) {
            sprite.move({0.f, movimiento.y});
        }
    }

    // Actualizar estado de animación
    if (seMueve && actual != Caminar) {
        setEstado(Caminar);
    } else if (!seMueve && actual == Caminar) {
        setEstado(Quieto);
    }

    sprite.setScale({1.f, 1.f});
    sprite.setOrigin({0.f, 0.f});
}

/*void Personaje::actualizarAnimacion() {
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
}*/

void Personaje::actualizarEnBiblioteca() {
    sf::Vector2f movimiento(0.f, 0.f); // vector temporal de movimiento

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        movimiento.y = -velocidad;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        movimiento.y = velocidad;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        movimiento.x = -velocidad;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        movimiento.x = velocidad;
    }

    // Normalizar movimiento diagonal (para que no se mueva más rápido en diagonal)
    if (movimiento.x != 0.f && movimiento.y != 0.f)
        movimiento *= 0.7071f;

    // Aplicar movimiento
    sprite.move(movimiento);
}



void Personaje::actualizar(const std::vector<int>& tiles, const std::vector<int>& objetos, unsigned int mapWidth, unsigned int mapHeight)
{
     mover(tiles, objetos, mapWidth, mapHeight);
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
    float hitboxWidth = bounds.size.x * 0.5f;   // 50% del ancho
    float hitboxHeight = bounds.size.y * 0.3f;  // 30% de la altura (parte inferior)

    float hitboxX = bounds.position.x + (bounds.size.x - hitboxWidth) / 2.0f;  // Centrar horizontalmente
    float hitboxY = bounds.position.y + bounds.size.y - hitboxHeight;  // Posicionar en la parte inferior

    return sf::FloatRect(sf::Vector2f(hitboxX, hitboxY), sf::Vector2f(hitboxWidth, hitboxHeight));
}

sf::Vector2f Personaje::getPosition() const {
    return sprite.getPosition();
}

void Personaje::setPosition(int x, int y) {
    sprite.setPosition(sf::Vector2f(x, y));
}

void Personaje::actualizarEnInterior() {
        // Versión simplificada de actualizar sin colisiones con tiles
        // Solo maneja el movimiento básico
        float deltaTime = 1.0f / 60.0f; // O obtener el deltaTime real

        sf::Vector2f movimiento(0.f, 0.f);
        float velocidad = 100.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        movimiento.y = -velocidad;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        movimiento.y = velocidad;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        movimiento.x = -velocidad;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        movimiento.x = velocidad;
    }

        movimiento *= deltaTime;
        sprite.move(movimiento);

        // Aquí podrías añadir límites para que no se salga de la biblioteca
        sf::Vector2f pos = sprite.getPosition();
        if (pos.x < 0) sprite.setPosition({0, pos.y});
        if (pos.y < 0) sprite.setPosition({pos.x, 0});
        if (pos.x > 800) sprite.setPosition({800, pos.y}); // Ajustar según el tamaño
        if (pos.y > 600) sprite.setPosition({pos.x, 600}); // Ajustar según el tamaño
    }

Personaje::~Personaje()
{
    //dtor
}
