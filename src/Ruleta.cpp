#include "Ruleta.h"
#include <iostream>
#include <random>

Ruleta::Ruleta() : m_textura(), m_texturaCentro(), m_sprite(m_textura), m_spriteCentro(m_texturaCentro), m_circuloColision(), m_velocidadRotacion(0.0f) {
}

bool Ruleta::cargar(const std::string& rutaTexturaRuleta) {
    if (!m_textura.loadFromFile(rutaTexturaRuleta)) {
        std::cerr << "ERROR: No se pudo cargar textura ruleta\n";
        return false;
    }

    m_sprite.setTexture(m_textura, true);
    m_sprite.setScale({0.3f, 0.3f});
    sf::Vector2u tam = m_textura.getSize();
    m_sprite.setOrigin({tam.x / 2.f, tam.y / 2.f});

    if (!m_texturaCentro.loadFromFile("assets/Spinwheel/arrow.png")) {
        std::cerr << "ERROR: No se pudo cargar arrow.png\n";
        return false;
    }

    m_spriteCentro.setTexture(m_texturaCentro, true);
    m_spriteCentro.setScale({0.1f, 0.1f});
    sf::Vector2u tamCentro = m_texturaCentro.getSize();
    m_spriteCentro.setOrigin({tamCentro.x / 2.f, tamCentro.y / 2.f});
    m_spriteCentro.setRotation(sf::degrees(0));

    // hitbox circular
    m_circuloColision.setFillColor(sf::Color(0,0,0,0));
    m_circuloColision.setOutlineThickness(2.f);
    m_circuloColision.setOutlineColor(sf::Color(255,255,255,60));
    actualizarHitbox();
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

void Ruleta::actualizarHitbox() {
    const auto tex = m_textura.getSize();
    const float escalaX = m_sprite.getScale().x;
    const float escalaY = m_sprite.getScale().y;
    const float w = tex.x * escalaX;
    const float h = tex.y * escalaY;
    const float radio = 0.5f * std::max(w, h);
    m_circuloColision.setRadius(radio);
    m_circuloColision.setOrigin({radio, radio});
    m_circuloColision.setPosition(m_sprite.getPosition());
}

sf::FloatRect Ruleta::getBounds() const {
    return m_sprite.getGlobalBounds(); // usa el sprite como AABB
}

bool Ruleta::estaGirando() const {
    return m_velocidadRotacion > 0.0f;
}

bool Ruleta::enReposo() const {
    return m_velocidadRotacion <= 0.0f;
}

float Ruleta::getAnguloGrados() const {
    return m_sprite.getRotation().asDegrees(); // SFML 3 devuelve Angle; .value son grados
}

int Ruleta::getIndiceResultado(int segmentos) const {
    // 0° está arriba (flecha)
    const float offsetDeg = 0.f; // si tu flecha apunta a otro lado, ajusta aquí (por ejemplo 90.f o -90.f)
    float ang = std::fmod(getAnguloGrados() + 360.f + offsetDeg, 360.f);
    float tam = 360.f / segmentos;

    // Invertimos sentido para coincidir con sentido visual de la textura
    int idx = static_cast<int>((360.f - ang) / tam) % segmentos;
    return idx;
}

std::string Ruleta::getTextoResultado() const {
    static const char* nombres[4] = {
        "Politica",     // Sector superior izquierdo (azul)
        "Ciencia", // Superior derecho (rojo)
        "Historia", // Inferior izquierdo (amarillo)
        "Arte"   // Inferior derecho (verde)
    };
    return nombres[getIndiceResultado(4)];
}
