#include "BarraVida.h"

/*BarraVida::BarraVida(sf::Vector2f posicion, const std::vector<std::string>& rutasSprites)
    : m_maxVida(rutasSprites.size() > 0 ? rutasSprites.size() - 1 : 0),
      m_vidaActual(m_maxVida),
      m_sprite(temp)
{
    if (rutasSprites.empty()) {
        std::cerr << " 🛑  ERROR CRITICO: No se proporcionaron rutas de sprites para BarraVida." << std::endl;
        return;
    }

    m_texturasVida.resize(rutasSprites.size());
    for (size_t i = 0; i < rutasSprites.size(); ++i) {
        if (!m_texturasVida[i].loadFromFile(rutasSprites[i])) {
            std::cerr << " 🛑  ERROR CRITICO: No se pudo cargar la textura de la BarraVida: " << rutasSprites[i] << std::endl;
        }
    }

    m_sprite.setPosition(posicion);
    resetVida();
}

void BarraVida::actualizarSprite() {
    int indice = m_maxVida - m_vidaActual;

    if (indice >= 0 && indice < m_texturasVida.size()) {
        m_sprite.setTexture(m_texturasVida[indice], true);
    }
}

void BarraVida::perderVida() {
    if (m_vidaActual > 0) {
        m_vidaActual--;
        actualizarSprite();
    }
}

void BarraVida::resetVida() {
    m_vidaActual = m_maxVida;
    actualizarSprite();
}

bool BarraVida::estaDebilitado() const {
    return m_vidaActual <= 0;
}

void BarraVida::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_sprite, states);
}

BarraVida::~BarraVida()
{
    //dtor
}
*/
