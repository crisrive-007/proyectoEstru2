#include "Biblioteca.h"
#include <iostream>

Biblioteca::Biblioteca(Personaje& personaje)
    : fondo(texturaFondo), jugador(personaje), activa(false) {
    // Crear nueva ventana para la biblioteca
    ventanaBiblioteca.create(sf::VideoMode({1920u, 1080u}), "Biblioteca", sf::Style::Titlebar | sf::Style::Close);

    // Posicionar al jugador en la entrada de la biblioteca
    jugador.establecerPosicion(sf::Vector2f(100.0f, 500.0f));
}

bool Biblioteca::cargarRecursos() {
    // Cargar textura del fondo de la biblioteca
    if (!texturaFondo.loadFromFile("assets/Backgrounds/library.png")) {
        std::cerr << "Error: No se pudo cargar la textura del fondo de la biblioteca\n";
        return false;
    }

    // Configurar sprite del fondo
    fondo.setTexture(texturaFondo);
    fondo.setScale(sf::Vector2f(
        ventanaBiblioteca.getSize().x / texturaFondo.getSize().x,
        ventanaBiblioteca.getSize().y / texturaFondo.getSize().y)
    );

    return true;
}

void Biblioteca::ejecutar() {
    if (!cargarRecursos()) {
        return;
    }

    activa = true;

    while (activa && ventanaBiblioteca.isOpen()) {
        // Procesar eventos de la ventana
        while (auto event = ventanaBiblioteca.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                ventanaBiblioteca.close();
                activa = false;
                return;
            }

            if (event->is<sf::Event::KeyPressed>()) {
                if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape) {
                    ventanaBiblioteca.close();
                    activa = false;
                    return;
                }
            }
        }

        // Movimiento del jugador
        sf::Vector2f movimiento(0.f, 0.f);
        bool seMueve = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            movimiento.y = -3.0f, seMueve = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            movimiento.y = 3.0f, seMueve = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            movimiento.x = -3.0f, seMueve = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            movimiento.x = 3.0f, seMueve = true;

        // Normalizar movimiento diagonal
        if (movimiento.x != 0.f && movimiento.y != 0.f)
            movimiento *= 0.7071f;

        // Aplicar movimiento
        jugador.establecerPosicion(jugador.obtenerPosicion() + movimiento);

        // Limitar dentro de la ventana
        jugador.limitarBordes(1920.0f, 1080.0f);

        // Dibujar escena
        ventanaBiblioteca.clear();
        ventanaBiblioteca.draw(fondo);
        jugador.dibujar(ventanaBiblioteca);
        ventanaBiblioteca.display();
    }

    activa = false;
}
