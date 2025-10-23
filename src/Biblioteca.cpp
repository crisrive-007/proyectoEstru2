#include "Biblioteca.h"
#include <iostream>

Biblioteca::Biblioteca(sf::RenderWindow& window, Personaje& personaje)
    : m_spriteFondo(m_texturaFondo), m_window(window), m_personaje(personaje) {

    std::cout << "🔹 DEBUG: Constructor Biblioteca iniciado" << std::endl;

    // Cargar la textura del fondo de la biblioteca
    if (!m_texturaFondo.loadFromFile("assets/Backgrounds/library.png")) {
        std::cerr << "❌ ERROR: No se pudo cargar la textura de la biblioteca" << std::endl;
        // Puedes cargar una textura por defecto o manejar el error como prefieras
    }

    // Configurar el sprite del fondo
    m_spriteFondo.setTexture(m_texturaFondo);

    // Escalar el sprite para que ocupe toda la ventana (opcional)
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = m_texturaFondo.getSize();

    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    m_spriteFondo.setScale({scaleX, scaleY});

    // Configurar el área de salida (por ejemplo, cerca de la puerta)
    m_areaSalida.setSize(sf::Vector2f(50, 50));
    m_areaSalida.setPosition({50, 300}); // Posición donde está la puerta
    m_areaSalida.setFillColor(sf::Color(0, 255, 0, 128)); // Verde semitransparente para debug

    std::cout << "✅ DEBUG: Biblioteca configurada correctamente" << std::endl;
}

Biblioteca::~Biblioteca() {
    std::cout << "🔹 DEBUG: Destructor Biblioteca" << std::endl;
}

void Biblioteca::ejecutarBiblioteca() {
    std::cout << "🔹 DEBUG: Ejecutando escena de biblioteca" << std::endl;

    // Posicionar al personaje en un punto específico de la biblioteca
    m_personaje.setPosition(100, 400);
}

void Biblioteca::actualizar() {
    // Actualizar el personaje (en la biblioteca no hay colisiones con tiles)
    m_personaje.actualizarEnInterior(); // Necesitarías implementar este método

    // Verificar si el personaje está en el área de salida
    sf::Vector2f centroPersonaje = m_personaje.getPosition();
    sf::Vector2f posArea = m_areaSalida.getPosition();
    sf::Vector2f tamArea = m_areaSalida.getSize();

    bool enAreaSalida = (centroPersonaje.x >= posArea.x &&
                        centroPersonaje.x <= posArea.x + tamArea.x &&
                        centroPersonaje.y >= posArea.y &&
                        centroPersonaje.y <= posArea.y + tamArea.y);

    if (enAreaSalida) {
        std::cout << "🚪 Personaje en área de salida de la biblioteca" << std::endl;
    }
}

void Biblioteca::dibujar() {
    // Dibujar el fondo de la biblioteca
    m_window.draw(m_spriteFondo);

    // Dibujar el área de salida (opcional, para debug)
    m_window.draw(m_areaSalida);

    // Dibujar el personaje
    m_personaje.dibujar(m_window);
}

/*bool Biblioteca::debeSalir() const {
    // Verificar si el personaje quiere salir (por ejemplo, presionando una tecla en el área de salida)
    sf::Vector2f centroPersonaje = m_personaje.getPosition();
    sf::Vector2f posArea = m_areaSalida.getPosition();
    sf::Vector2f tamArea = m_areaSalida.getSize();

    bool enAreaSalida = (centroPersonaje.x >= posArea.x &&
                        centroPersonaje.x <= posArea.x + tamArea.x &&
                        centroPersonaje.y >= posArea.y &&
                        centroPersonaje.y <= posArea.y + tamArea.y);

    return enAreaSalida && sf::Keyboard::isKeyPressed(sf::Keyboard:);
}*/
