#ifndef RULETA_H
#define RULETA_H

#include <SFML/Graphics.hpp>
#include <string>

class Ruleta
{
    public:
        Ruleta();
        bool cargar(const std::string& rutaTextura);
        void actualizar(float deltaTime);
        void dibujar(sf::RenderWindow& window) const;
        void setPosition(float x, float y);
        void iniciarGiro(float velocidadInicial);
        sf::Vector2f getPosition() const;
        const sf::CircleShape& getCirculoColision() const;

        sf::FloatRect getBounds() const;
        bool estaGirando() const;
        bool enReposo()const;
        float getAnguloGrados() const;
        int getIndiceResultado(int segmentos = 4) const;
        std::string getTextoResultado() const;

        void actualizarHitbox();

    protected:

    private:
        sf::Texture m_textura;
        sf::Sprite m_sprite;
        sf::Texture m_texturaCentro;
        sf::Sprite m_spriteCentro;
        float m_velocidadRotacion;
        sf::CircleShape m_circuloColision;
};
#endif // RULETA_H
