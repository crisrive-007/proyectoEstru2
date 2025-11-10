#include "Menu.h"
#include "GestorEstados.h"
#include "MapaPrincipal.h"
#include "SaveGameManager.h"
#include <algorithm>
#include <iostream>
#include <memory>

namespace {
constexpr float BTN_W = 420.f;
constexpr float BTN_H = 70.f;
constexpr float GAP   = 24.f;
constexpr float TITLE_SIZE = 52.f;
constexpr float LABEL_SIZE = 28.f;
}

Menu::Menu(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje, const sf::Font* fontOpt)
: Estado(gestor, personaje)
, m_window(window)
, m_personaje(personaje)
, m_title(m_ownedFont)
, m_buttons{ sf::RectangleShape(), sf::RectangleShape() }
, m_labels{ sf::Text(m_ownedFont), sf::Text(m_ownedFont) }
, m_textoNombre(m_ownedFont)
, m_feedback(m_ownedFont)
{
    // Fuente
    if (fontOpt) {
        m_font = fontOpt;
    } else {
        if (!m_ownedFont.openFromFile("assets/Pokemon_GB.ttf")) {
            std::cerr << "⚠️ No se pudo cargar assets/Pokemon_GB.ttf\n";
        }
        m_font = &m_ownedFont;
    }

    // Título
    m_title.setFont(*m_font);
    m_title.setString("Menu Principal");
    m_title.setCharacterSize(static_cast<unsigned>(TITLE_SIZE));
    m_title.setFillColor(sf::Color::White);

    // Botones
    const std::array<std::string, NUM_OPCIONES> textos = {"Nueva Partida", "Cargar Partida"};
    for (int i = 0; i < NUM_OPCIONES; ++i) {
        m_buttons[i].setSize({BTN_W, BTN_H});
        m_buttons[i].setFillColor(sf::Color(30, 30, 30));
        m_buttons[i].setOutlineThickness(3.f);
        m_buttons[i].setOutlineColor(sf::Color(90, 90, 90));
        m_buttons[i].setOrigin({BTN_W * 0.5f, BTN_H * 0.5f});

        m_labels[i].setFont(*m_font);
        m_labels[i].setString(textos[i]);
        m_labels[i].setCharacterSize(static_cast<unsigned>(LABEL_SIZE));
        m_labels[i].setFillColor(sf::Color(230, 230, 230));
    }

    // Entrada de nombre
    m_cajaNombre.setSize({600.f, 70.f});
    m_cajaNombre.setFillColor(sf::Color(25, 25, 25, 220));
    m_cajaNombre.setOutlineColor(sf::Color::White);
    m_cajaNombre.setOutlineThickness(3.f);
    m_cajaNombre.setOrigin({300.f, 35.f});
    m_cajaNombre.setPosition({m_window.getSize().x * 0.5f, m_window.getSize().y * 0.6f});

    m_textoNombre.setFont(*m_font);
    m_textoNombre.setCharacterSize(32);
    m_textoNombre.setFillColor(sf::Color::White);

    layout();
    applyStyles();
}

void Menu::setTitle(const std::string& titulo) {
    m_title.setString(titulo);
    layout();
}

void Menu::setButtonText(Opcion op, const std::string& txt) {
    if (op < 0 || op >= NUM_OPCIONES) return;
    m_labels[op].setString(txt);
    layout();
}

void Menu::layout() {
    const auto size = m_window.getView().getSize();

    // Título centrado
    const auto tb = m_title.getLocalBounds();
    m_title.setOrigin({tb.position.x + tb.size.x * 0.5f, tb.position.y + tb.size.y * 0.5f});
    m_title.setPosition({size.x * 0.5f, size.y * 0.25f});

    // Botones centrados
    float startY = m_title.getPosition().y + 90.f;
    for (int i = 0; i < NUM_OPCIONES; ++i) {
        float y = startY + i * (BTN_H + GAP) + BTN_H * 0.5f;
        m_buttons[i].setPosition({size.x * 0.5f, y});

        const auto lb = m_labels[i].getLocalBounds();
        m_labels[i].setOrigin({lb.position.x + lb.size.x * 0.5f, lb.position.y + lb.size.y * 0.5f});
        m_labels[i].setPosition(m_buttons[i].getPosition());
    }

    // Input nombre
    m_cajaNombre.setPosition({m_window.getSize().x * 0.5f, m_window.getSize().y * 0.6f});
    const auto nb = m_textoNombre.getLocalBounds();
    m_textoNombre.setOrigin({nb.position.x + nb.size.x * 0.5f, nb.position.y + nb.size.y * 0.5f});
    m_textoNombre.setPosition(m_cajaNombre.getPosition());
}

void Menu::applyStyles() {
    for (int i = 0; i < NUM_OPCIONES; ++i) {
        if (i == m_selected) {
            m_buttons[i].setOutlineColor(sf::Color(180, 180, 180));
            m_buttons[i].setFillColor(sf::Color(50, 50, 50));
            m_labels[i].setFillColor(sf::Color::White);
        } else {
            m_buttons[i].setOutlineColor(sf::Color(90, 90, 90));
            m_buttons[i].setFillColor(sf::Color(30, 30, 30));
            m_labels[i].setFillColor(sf::Color(220, 220, 220));
        }
    }
}

bool Menu::isMouseOver(const sf::RectangleShape& shape) const {
    auto mouse = sf::Mouse::getPosition(m_window);
    sf::Vector2f mpos(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
    return shape.getGlobalBounds().contains(mpos);
}

void Menu::selectNext(int dir) {
    m_selected = (m_selected + dir + NUM_OPCIONES) % NUM_OPCIONES;
    applyStyles();
}

void Menu::activateCurrent() {
    if (m_selected == NuevaPartida) {
        iniciarInputNombre();
    } else {
        crearYEmpujarMapaCargarPartida();
    }
}

void Menu::iniciarInputNombre() {
    m_pidiendoNombre = true;
    m_nombreJugador.clear();
    m_textoNombre.setString("_");
    m_title.setString("Ingresa tu nombre:");
    layout();
}

void Menu::manejarInputNombre(const sf::Event& ev) {
    if (!ev.is<sf::Event::TextEntered>()) return;

    auto u = ev.getIf<sf::Event::TextEntered>()->unicode;

    if (u == '\r' || u == '\n') {
        if (!m_nombreJugador.empty()) {
            m_personaje.setNombre(m_nombreJugador);
            m_personaje.reiniciarVidas(); // arranque limpio si quieres
            crearYEmpujarMapaNuevaPartida(); // abre mapa en el mismo frame
        } else {
            mostrarFeedback("⚠️ Escribe un nombre.");
        }
        m_pidiendoNombre = false;
        m_title.setString("Menu Principal");
        layout();
        return;
    }
    if (u == 8) { // backspace
        if (!m_nombreJugador.empty())
            m_nombreJugador.pop_back();
    } else if (u >= 32 && u <= 126 && m_nombreJugador.size() < 16) {
        m_nombreJugador.push_back(static_cast<char>(u));
    }
    m_textoNombre.setString(m_nombreJugador + "_");
    const auto nb = m_textoNombre.getLocalBounds();
    m_textoNombre.setOrigin({nb.position.x + nb.size.x * 0.5f, nb.position.y + nb.size.y * 0.5f});
    m_textoNombre.setPosition(m_cajaNombre.getPosition());
}

void Menu::manejarEventos(sf::RenderWindow& window) {
    // Igual que Biblioteca: el propio estado se encarga de pollEvent y despachar lógica
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { window.close(); return; }

        if (m_pidiendoNombre) { manejarInputNombre(*ev); continue; }

        if (auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            if (k->scancode == sf::Keyboard::Scan::Up)   selectNext(-1);
            if (k->scancode == sf::Keyboard::Scan::Down) selectNext(+1);
            if (k->scancode == sf::Keyboard::Scan::Enter) activateCurrent();
        } else if (ev->is<sf::Event::MouseMoved>()) {
            for (int i = 0; i < NUM_OPCIONES; ++i) {
                if (isMouseOver(m_buttons[i])) {
                    if (m_selected != i) { m_selected = i; applyStyles(); }
                }
            }
        } else if (auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button == sf::Mouse::Button::Left) {
                for (int i = 0; i < NUM_OPCIONES; ++i) {
                    if (isMouseOver(m_buttons[i])) { m_selected = i; applyStyles(); activateCurrent(); break; }
                }
            }
        } else if (ev->is<sf::Event::Resized>()) {
            layout();
        }
    }
}

void Menu::actualizar() {
    // animaciones/light logic si quieres
}

void Menu::dibujar(sf::RenderWindow& window) {
    window.setView(window.getDefaultView());
    window.draw(m_title);

    if (m_pidiendoNombre) {
        window.draw(m_cajaNombre);
        window.draw(m_textoNombre);
    } else {
        for (int i = 0; i < NUM_OPCIONES; ++i) {
            window.draw(m_buttons[i]);
            window.draw(m_labels[i]);
        }
    }
}

void Menu::guardarPartida() {
    bool ok = SaveGame::guardarJuego(m_personaje, ProgresoJuego::get());
    if (ok) mostrarFeedback("Partida guardada.");
    else    mostrarFeedback("Error al guardar.");
}

void Menu::crearYEmpujarMapaNuevaPartida() {
    auto estadoMapa = std::make_unique<MapaPrincipal>(gestor, m_window, m_personaje);
    estadoMapa->ejecutarMapa();
    gestor->empujarEstado(std::move(estadoMapa));
    mostrarFeedback("Nueva partida.");
}

void Menu::crearYEmpujarMapaCargarPartida() {
    ProgresoJuego& prog = ProgresoJuego::get();
    if (SaveGame::cargarJuego(m_personaje, prog)) {
        auto estadoMapa = std::make_unique<MapaPrincipal>(gestor, m_window, m_personaje);
        estadoMapa->ejecutarMapa();
        gestor->empujarEstado(std::move(estadoMapa));
        mostrarFeedback("Partida cargada.");
    } else {
        mostrarFeedback("No hay partida para cargar.");
    }
}

void Menu::mostrarFeedback(const std::string& msg) {
    m_feedback.setString(msg);
    m_feedbackClock.restart();
    m_mostrarFeedback = true;
}
