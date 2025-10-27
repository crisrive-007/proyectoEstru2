#include "Ruleta.h"
#include <iostream>
#include <random> // Para generación de números aleatorios

Ruleta::Ruleta() : m_textura(), m_texturaCentro(), m_sprite(m_textura), m_spriteCentro(m_texturaCentro), m_circuloColision(), m_velocidadRotacion(0.0f) {
}

bool Ruleta::cargar(const std::string& rutaTexturaRuleta) {
    std::cout << "Intentando cargar: " << rutaTexturaRuleta << std::endl;

    // Cargar la textura de la Ruleta (igual que antes)
    if (!m_textura.loadFromFile(rutaTexturaRuleta)) {
        std::cerr << "ERROR: No se pudo cargar la textura de la ruleta" << std::endl;
        return false;
    }

    m_sprite.setTexture(m_textura, true);
    m_sprite.setScale(sf::Vector2f(0.3f, 0.3f));

    sf::Vector2u tamanoRuleta = m_textura.getSize();
    m_sprite.setOrigin({tamanoRuleta.x / 2.0f, tamanoRuleta.y / 2.0f});

    std::cout << "✓ Textura Ruleta cargada exitosamente!" << std::endl;
    std::cout << "  - Tamaño: " << tamanoRuleta.x << "x" << tamanoRuleta.y << " píxeles" << std::endl;
    std::cout << "  - Origen: (" << tamanoRuleta.x / 2.0f << ", " << tamanoRuleta.y / 2.0f << ")" << std::endl;

    // --- NUEVO: Cargar y configurar el Sprite del Centro ---
    std::cout << "Intentando cargar textura del centro: " << std::endl;
    if (!m_texturaCentro.loadFromFile("assets/Spinwheel/arrow.png")) {
        std::cerr << "ERROR: No se pudo cargar la textura del centro de la ruleta" << std::endl;
        // Si el centro no carga, decidimos si fallar o solo registrar un error.
        // Aquí decidimos continuar, pero el sprite del centro estará vacío.
        // Para este ejemplo, si falla la ruleta central, fallamos la carga total.
        return false;
    }

    m_spriteCentro.setTexture(m_texturaCentro, true);
    // Ajustar la escala del centro. Puedes querer que sea más pequeño que la ruleta.
    m_spriteCentro.setScale(sf::Vector2f(0.1f, 0.1f));

    sf::Vector2u tamanoCentro = m_texturaCentro.getSize();
    // Establecer el origen al centro de la textura del centro
    m_spriteCentro.setOrigin({tamanoCentro.x / 2.0f, tamanoCentro.y / 2.0f});

    std::cout << "✓ Textura Centro cargada exitosamente!" << std::endl;
    std::cout << "  - Tamaño Centro: " << tamanoCentro.x << "x" << tamanoCentro.y << " píxeles" << std::endl;
    std::cout << "  - Origen Centro: (" << tamanoCentro.x / 2.0f << ", " << tamanoCentro.y / 2.0f << ")" << std::endl;
    // --------------------------------------------------------

    return true;
}

void Ruleta::actualizar(float deltaTime) {
    if (m_velocidadRotacion > 0.0f) {
        m_sprite.rotate(sf::degrees(m_velocidadRotacion * deltaTime));
        m_velocidadRotacion -= 100.0f * deltaTime;
        if (m_velocidadRotacion < 0.0f) {
            m_velocidadRotacion = 0.0f;
        }
    }
}

void Ruleta::dibujar(sf::RenderWindow& window) const {
    window.draw(m_sprite);
    window.draw(m_spriteCentro);
    window.draw(m_circuloColision);
}

void Ruleta::setPosition(float x, float y) {
    m_sprite.setPosition(sf::Vector2f(x, y));
    m_spriteCentro.setPosition(sf::Vector2f(x, y));
}

void Ruleta::iniciarGiro(float velocidadInicial) {
    // Crear un generador de números aleatorios
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Definir el rango de velocidad aleatoria (puedes ajustar estos valores)
    std::uniform_real_distribution<float> dis(velocidadInicial * 0.8f, velocidadInicial * 1.2f);

    // Asignar una velocidad aleatoria
    m_velocidadRotacion = dis(gen);

    std::cout << "Velocidad de giro aleatoria: " << m_velocidadRotacion << std::endl;
}

sf::Vector2f Ruleta::getPosition() const {
    return m_sprite.getPosition();
}

const sf::CircleShape& Ruleta::getCirculoColision() const {
    return m_circuloColision;
}
