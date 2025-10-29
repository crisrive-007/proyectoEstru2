#ifndef BOTONOPCION_H
#define BOTONOPCION_H
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class BotonOpcion
{
    public:
        BotonOpcion() : text(nullptr) {
            shape.setOutlineThickness(4.f);
            shape.setOutlineColor(sf::Color::Black);
        }

        void configurar(const sf::Vector2f& pos, const sf::Vector2f& size,
                        const sf::Color& fill, const std::string& etiqueta, sf::Font& font) {
            shape.setPosition(pos);
            shape.setSize(size);
            base = fill;
            shape.setFillColor(base);

            text.reset(new sf::Text(font));
            text->setString(etiqueta);
            text->setCharacterSize(26);
            text->setFillColor(sf::Color::White);
            centrarTexto();
        }

        void setTexto(const std::string& s){
            if (text) {
                text->setString(s);
                centrarTexto();
            }
        }

        bool contiene(sf::Vector2f p) const {
            return shape.getGlobalBounds().contains(p);
        }

        bool manejarEvento(const sf::Event& e, const sf::RenderWindow& w) {
            if (const auto* moved = e.getIf<sf::Event::MouseMoved>()) {
                auto mp = w.mapPixelToCoords(moved->position);
                hovered = contiene(sf::Vector2f(mp));
            }
            if (const auto* pressed = e.getIf<sf::Event::MouseButtonPressed>()) {
                if (pressed->button == sf::Mouse::Button::Left) {
                    auto mp = w.mapPixelToCoords(pressed->position);
                    if (contiene(sf::Vector2f(mp))) return true;
                }
            }
            return false;
        }

        void draw(sf::RenderTarget& t){
            shape.setFillColor(hovered ? sf::Color(base.r*0.8f, base.g*0.8f, base.b*0.8f) : base);
            t.draw(shape);
            if (text) t.draw(*text);
        }

    private:
        void centrarTexto(){
            if (!text) return;
            auto b = text->getLocalBounds();
            text->setOrigin({b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f});
            text->setPosition(shape.getPosition() + shape.getSize() / 2.f);
        }

        sf::RectangleShape shape;
        std::unique_ptr<sf::Text> text;
        sf::Color base{70, 70, 70};
        bool hovered{false};
};

#endif // BOTONOPCION_H
