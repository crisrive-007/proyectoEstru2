#ifndef EDIFICIOKANT_H
#define EDIFICIOKANT_H

#pragma once
#include "Estado.h"
#include <queue>
#include <array>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Pregunta.h"

enum class InteraccionKant { Ninguna, LibroPura, LibroPract, Solar, NPC };

class EdificioKant : public Estado
{
public:
    EdificioKant(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje);
    void manejarEventos(sf::RenderWindow& window) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& window) override;
    virtual ~EdificioKant();

private:
    // Mantén el orden para evitar -Wreorder
    sf::RenderWindow& m_window;
    Personaje&        m_personaje;

    // Fondo y sprites interactivos
    sf::Texture m_texFondo, m_texLibroPura, m_texLibroPract, m_texSolar, m_texNPC;
    sf::Sprite  m_sprFondo, m_sprLibroPura, m_sprLibroPract, m_sprSolar, m_sprNPC;

    // UI de diálogo (sin títulos superiores)
    sf::Font m_font;
    sf::Text m_enunciado, m_fb;
    std::array<sf::Text, 4> m_opts;

    // Quiz
    std::queue<Pregunta> m_cola;
    Pregunta m_actual{};
    bool m_mostrarQuiz = false;
    bool m_feedback    = false;
    int  m_sel         = -1;

    // Interacción y progreso
    InteraccionKant m_origen  = InteraccionKant::Ninguna;  // de dónde viene el quiz actual
    InteraccionKant m_cercano = InteraccionKant::Ninguna;  // qué objeto está cerca
    bool m_puraCompletado  = false;
    bool m_practCompletado = false;

    // Bancos por objeto
    void cargarPreguntasLibroPura();   // epistemología
    void cargarPreguntasLibroPract();  // ética
    void mostrarPistaSolar();          // pista/final

    void cargarSurtidoKant(); // compat
    void siguientePregunta();

    // Caja de diálogo (más abajo)
    sf::FloatRect dibujarCajaDialogo(sf::RenderWindow& window);

    // Layout del cuadro y padding
    float m_dialogHeight = 200.f;
    float m_dialogMargin = 20.f;
    float m_dialogPad    = 16.f;

    // --- Proximidad ---
    float m_interactDist = 120.f; // radio de activación
    InteraccionKant detectarCercania() const;

    // --- Solar: estático + animado por frames ---
    sf::Texture m_texSolarStatic;     // foto al inicio
    std::vector<sf::Texture> m_solarFrames; // frames del "gif"
    std::size_t m_solarIndex = 0;
    sf::Clock   m_solarClock;
    float       m_solarFps = 8.f;     // velocidad de animación
    bool        m_solarAnimActivo = false;

    // Carga y animación
    void cargarSolarEstatico();
    void cargarSolarFrames(const std::string& carpeta, const std::string& prefijo,
                           int start, int end); // nombres: prefijo1.png ... prefijoN.png
    void actualizarSolarAnim();

    // Controla que la salida solo ocurra tras el diálogo del Sistema Solar
    bool m_finalSolar = false;
    // 2) Flags de gating
    bool m_npcHablado = false;         // habló con el NPC
    bool m_librosHabilitados = false;  // se habilitan tras hablar con NPC

    // 3) Ya existe m_finalSolar (déjalo). Añade la firma del diálogo del NPC:
    void mostrarDialogoNPC();

    sf::Text m_hint;       // texto pequeño "SPACE"
    float    m_hintBob = 0.f;

    std::vector<std::string> m_npcLines;  // líneas del diálogo
    int  m_npcIdx = -1;

    bool avanzarDialogoNPC();

    // ===== Diálogos (cola de líneas con avance por ENTER) =====
    enum class DialogOwner { None, NPC, Solar };
    std::vector<std::string> m_dialogLines;
    std::size_t m_dialogIndex = 0;
    DialogOwner m_dialogOwner = DialogOwner::None;
    bool m_dialogActivo = false; // estamos mostrando diálogo "por líneas"

    // Helpers de diálogo
    void startDialog(DialogOwner owner, std::initializer_list<std::string> lines);
    bool advanceDialog(); // avanza 1 línea; devuelve true si el diálogo terminó

    sf::RectangleShape m_areaSalida; // zona para salir del aula
    void interaccionSalida();

    std::vector<sf::FloatRect> m_colisiones;
    std::vector<sf::RectangleShape> m_dbgColisiones;
    sf::Vector2f m_prevPosJugador{};
    bool m_debugColisiones = true; // F3 toggle

    // Construcción y aplicación
    void cargarColisionesMapa();
    void aplicarColisiones();
    static bool intersecta(const sf::FloatRect& A, const sf::FloatRect& B);
};

#endif // EDIFICIOKANT_H
