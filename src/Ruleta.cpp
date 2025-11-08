#include "Ruleta.h"
#include <algorithm>
#include <chrono>
#include <random>

Ruleta::Ruleta() : m_sprPantalla(m_texPantalla), m_txt(m_font){
    std::random_device rd;
    m_rng.seed(rd());
}

bool Ruleta::cargar(const std::string& rutaPantalla, const std::string& rutaFuente) {
    if (!m_texPantalla.loadFromFile(rutaPantalla)) return false;
    m_sprPantalla.setTexture(m_texPantalla, true);
    m_sprPantalla.setOrigin({m_texPantalla.getSize().x / 2.f, m_texPantalla.getSize().y / 2.f});

    if (!m_font.openFromFile(rutaFuente)) return false;
    m_txt.setFont(m_font);
    m_txt.setCharacterSize(42);
    m_txt.setFillColor(sf::Color::White);
    m_txt.setOutlineThickness(2.f);
    m_txt.setOutlineColor(sf::Color::Black);
    m_txt.setString("—");
    centrarTexto();
    return true;
}

void Ruleta::setOpciones(const std::vector<std::string>& opciones) {
    m_opcionesOriginales = opciones;
    resetearOpciones();
}

void Ruleta::resetearOpciones() {
    m_opcionesActuales.clear();
    for (const auto& o : m_opcionesOriginales) m_opcionesActuales.push_back(o);
}

void Ruleta::setPosition(sf::Vector2f pos) {
    m_sprPantalla.setPosition(pos);
    centrarTexto();
}

void Ruleta::setEscalaPantalla(sf::Vector2f scl) {
    m_sprPantalla.setScale(scl);
    centrarTexto();
}

void Ruleta::setColorTexto(const sf::Color& c) {
    m_txt.setFillColor(c);
}

void Ruleta::setOutlineTexto(float grosor, const sf::Color& c) {
    m_txt.setOutlineThickness(grosor);
    m_txt.setOutlineColor(c);
}

void Ruleta::setTamanioTexto(unsigned tam) {
    m_txt.setCharacterSize(tam);
    centrarTexto();
}

void Ruleta::iniciarGiro(float durMinSeg, float durMaxSeg) {
    if (m_opcionesActuales.empty()) resetearOpciones();
    std::shuffle(m_opcionesActuales.begin(), m_opcionesActuales.end(), m_rng);

    std::uniform_real_distribution<float> dist(durMinSeg, durMaxSeg);
    m_duracion = dist(m_rng);
    m_tiempoTotal = 0.f;
    m_tickAcum = 0.f;
    m_tickActual = m_tickRapido;
    m_rodando = true;
}

void Ruleta::actualizar(float dt) {
    if (!m_rodando) return;

    m_tiempoTotal += dt;
    float t = std::clamp(m_tiempoTotal / m_duracion, 0.f, 1.f);
    m_tickActual = m_tickRapido + (m_tickLento - m_tickRapido) * easeOutCubic(t);

    m_tickAcum += dt;
    if (m_tickAcum >= m_tickActual && !m_opcionesActuales.empty()) {
        m_tickAcum = 0.f;
        m_txt.setString(m_opcionesActuales.front());
        centrarTexto();
        m_opcionesActuales.pop_front();
    }

    if (m_opcionesActuales.size() <= 1) {
        if (!m_opcionesActuales.empty())
            m_txt.setString(m_opcionesActuales.front());
        centrarTexto();
        m_rodando = false;
    }
}

void Ruleta::dibujar(sf::RenderTarget& target) const {
    target.draw(m_sprPantalla);
    target.draw(m_txt);
}

std::string Ruleta::getTextoResultado() const {
    return m_txt.getString();
}

float Ruleta::easeOutCubic(float t) {
    float inv = 1.f - t;
    return 1.f - inv * inv * inv;
}

void Ruleta::centrarTexto() {
    sf::FloatRect gb = m_txt.getLocalBounds();
    m_txt.setOrigin({gb.size.x / 2.f, gb.size.y / 2.f});
    m_txt.setPosition(m_sprPantalla.getPosition());
}

void Ruleta::eliminarOpcion(const std::string& texto) {
    if (m_rodando) return; // No se permite modificar mientras gira

    // --- 1️⃣ Eliminar del vector original ---
    auto itVec = std::remove(m_opcionesOriginales.begin(), m_opcionesOriginales.end(), texto);
    if (itVec != m_opcionesOriginales.end()) {
        m_opcionesOriginales.erase(itVec, m_opcionesOriginales.end());
    }

    // --- 2️⃣ Eliminar también del deque actual ---
    auto itDeque = std::remove(m_opcionesActuales.begin(), m_opcionesActuales.end(), texto);
    if (itDeque != m_opcionesActuales.end()) {
        m_opcionesActuales.erase(itDeque, m_opcionesActuales.end());
    }

    // --- 3️⃣ Si se eliminó el texto mostrado, mostrar la siguiente ---
    if (m_txt.getString() == texto) {
        if (!m_opcionesActuales.empty()) {
            m_txt.setString(m_opcionesActuales.front());
        } else {
            m_txt.setString("—"); // Si ya no hay opciones
        }
        centrarTexto();
    }
}

