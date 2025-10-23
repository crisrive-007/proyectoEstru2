#ifndef RULETA_H
#define RULETA_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <optional>

class Ruleta
{
    public:
        Ruleta();
        bool cargarRecursos();
        void girar();
        void actualizar();
        void calcularResultado();
        void dibujar(sf::RenderWindow& ventana);
        bool estaGirando() const;
        int getResultado() const;
        virtual ~Ruleta();

    protected:
    private:
        std::optional<sf::Sprite> spriteRuleta;
        sf::Texture textureRuleta;
        std::optional<sf::Sprite> flecha;
        sf::Texture textureFlecha;
        float anguloActual;
        float anguloObjetivo;
        float velocidad;
        bool girando;
        int resultado;
        std::vector<std::string> opciones;
        std::vector<sf::Color> colores;
        std::vector<float> angulosSecciones;
        sf::Font font;
        std::optional<sf::Text> textoResultado;
};
#endif // RULETA_H
