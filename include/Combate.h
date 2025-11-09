#ifndef COMBATE_H
#define COMBATE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <string>
#include <vector>
#include <queue>
#include <random>

#include "Estado.h"
#include "Personaje.h"
#include "Pregunta.h"

// Punto dentro del rectángulo global del sprite usando coordenadas normalizadas [0..1]
static inline sf::Vector2f anchorOn(const sf::Sprite& s, float nx, float ny) {
    const auto gb = s.getGlobalBounds(); // SFML3: gb.position / gb.size
    return { gb.position.x + gb.size.x * nx, gb.position.y + gb.size.y * ny };
}

// --- Reemplaza toda la definición anterior de FXStrip por esta ---
struct FXStrip {
    sf::Texture tex;
    std::unique_ptr<sf::Sprite> spr;  // <- ya no es por valor
    int frames = 0;
    int cur = 0;
    float acc = 0.f;
    float frameTime = 0.06f; // 60ms por frame
    sf::Vector2i frameSz{0,0};
    bool playing = false;

    bool load(const std::string& path, int framesCount) {
        if (!tex.loadFromFile(path)) return false;
        frames = std::max(1, framesCount);
        auto sz = tex.getSize();
        frameSz = { int(sz.x), int(sz.y / unsigned(frames)) };

        spr = std::make_unique<sf::Sprite>(tex);           // <- construimos aquí
        spr->setTextureRect(sf::IntRect({0,0}, frameSz));
        spr->setOrigin({ frameSz.x * 0.5f, frameSz.y * 0.5f });
        spr->setScale({10.f,10.f});

        playing = false; cur = 0; acc = 0.f;
        return true;
    }

    void play(const sf::Vector2f& pos) {
        if (!spr) return;
        spr->setPosition(pos);
        cur = 0; acc = 0.f; playing = true;
        spr->setTextureRect(sf::IntRect({0, cur * frameSz.y}, frameSz));
    }

    void update(float dt) {
        if (!playing || !spr) return;
        acc += dt;
        while (acc >= frameTime) {
            acc -= frameTime;
            ++cur;
            if (cur >= frames) { playing = false; break; }
            spr->setTextureRect(sf::IntRect({0, cur * frameSz.y}, frameSz));
        }
    }

    void draw(sf::RenderWindow& w) const {
        if (playing && spr) w.draw(*spr);
    }
};

class Combate : public Estado
{
    public:
        enum class EstadoCombate { Jugando, Gano, Perdio };
        enum class Subestado {Pregunta, Feedback};
        enum class Turno { Jugador, Npc };

        Combate(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje);

        // Ciclo de Estado (misma forma que en Biblioteca)
        void manejarEventos(sf::RenderWindow& window) override;
        void actualizar() override;
        void dibujar(sf::RenderWindow& window) override;

        // Control
        void iniciarCombate();   // resetea HUD + pone primera pregunta
        bool terminado() const { return m_estado != EstadoCombate::Jugando; }

        void startBGM();
        void stopBGM();
        void onWin();

        void salirDelMinijuego();
        void onLose();

        virtual ~Combate();

    protected:

    private:
        // Helpers SFML3
        static bool containsPoint(const sf::FloatRect& r, const sf::Vector2f& p) {
            const float x1 = r.position.x, y1 = r.position.y;
            const float x2 = x1 + r.size.x, y2 = y1 + r.size.y;
            return (p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2);
        }

        void cargarAssets();
        void armarPreguntas();        // Puedes reemplazar con BancoPreguntas
        void mostrarPregunta(int idx);
        void procesarRespuesta(int idxOpcion);
        void siguientePregunta();

        // Referencias
        sf::RenderWindow& m_window;
        Personaje&        m_personaje;

        // Textos
        sf::Font   m_font;
        sf::Text   m_txtPregunta;
        sf::Text   m_txtOpcA, m_txtOpcB, m_txtOpcC, m_txtOpcD;

        // Sprites y texturas
        sf::Texture m_texFondo, m_texDialog, m_texPika, m_texChari;
        sf::Sprite  m_sprFondo, m_sprDialog, m_sprPika, m_sprChari;

        sf::Texture m_texBtnA, m_texBtnB, m_texBtnC, m_texBtnD;
        sf::Sprite  m_btnA,    m_btnB,    m_btnC,    m_btnD;

        // Vida jugador (5 segmentos → life5 lleno … life0 vacío)
        std::array<sf::Texture,6> m_texLife; // 0..5
        sf::Sprite  m_sprLife;

        // Estado del combate
        int m_vidaJugador = 5;
        EstadoCombate m_estado = EstadoCombate::Jugando;
        Subestado m_subestado = Subestado::Pregunta;

        int idxSeleccion = -1;

        sf::SoundBuffer m_bufBgm;
        sf::Sound m_bgm;

        sf::SoundBuffer m_bufAdvance;
        sf::Sound m_sndAdvance;

        sf::SoundBuffer m_bufVictory;
        sf::Sound m_sndVictory;

        // Preguntas
        std::vector<Pregunta> m_preguntas;
        int m_idxPregunta = 0;
        int m_respuestaCorrecta = 0;

        // Reloj
        sf::Clock m_clock;
        float m_shakePika  = 0.f;
        float m_shakeChari = 0.f;
        sf::Vector2f m_pikaBase, m_chariBase;
        FXStrip m_fxPika;
        FXStrip m_fxChari;

        // Anclas normalizadas sobre cada sprite (x,y en [0..1])
        sf::Vector2f m_anchorPika {0.35f, 0.55f};  // dónde le pega el fuego a Pikachu
        sf::Vector2f m_anchorChari{0.62f, 0.35f};  // dónde le pega el rayo a Charizard

        // Pequeños offsets en píxeles (corrigen según arte)
        sf::Vector2f m_fxOffsetPika  {  6.f,  22.f};  // para el Lanzallamas (sobre Pikachu)
        sf::Vector2f m_fxOffsetChari {-10.f,  -8.f};  // para el Impactrueno (sobre Charizard)

        // --- Turnos por queue ---
        std::queue<Turno> m_turnos;
        Turno m_proximoTurno = Turno::Jugador; // el siguiente en jugar
        Turno m_turnoActual  = Turno::Jugador;
        float m_npcWait = 0.f;             // temporizador de 3s para el NPC

        // RNG para el 75%
        std::mt19937 m_rng{ std::random_device{}() };
        std::uniform_real_distribution<float> m_dist{0.f, 1.f};

        // Vida NPC (mismo set de texturas)
        int m_vidaNpc = 4;
        //sf::Sprite m_sprLifeNpc;

        // Helpers de flujo
        void iniciarRonda();               // llena la cola: Jugador -> Npc
        void avanzarTurno();               // saca de la cola y prepara estado
        void procesarRespuestaTurno(int idxOpcion, Turno actor);
        void turnoNpcAuto();

        // ===== HUD estilo Gimnasio =====
        // Jugador
        sf::RectangleShape       m_hudBoxJ;
        sf::Font                 m_fontHud;         // misma fuente Pokemon_GB.ttf
        sf::Text                 m_txtNombreJ;
        sf::Texture              m_texCorazon;      // assets/vidas.png (mismo recurso)
        std::vector<sf::Sprite>  m_corazonesJ;
        int                      m_vidasCacheJ = -1;

        // NPC
        sf::RectangleShape       m_hudBoxN;
        sf::Text                 m_txtNombreN;
        std::vector<sf::Sprite>  m_corazonesN;
        int                      m_vidasCacheN = -1;

        // Reconstrucción tipo Gimnasio
        void actualizarHUDJugador();   // rehace corazones si cambia m_vidaJugador
        void actualizarHUDNpc();

        // Turno / UI de turno
        sf::Text m_lblTurno;              // “Tu turno” / “Turno del NPC (pensando...)”
        int      m_npcOpcion = -1;        // Opción que eligió el NPC (0..3), -1 si ninguna aún
        sf::RectangleShape m_resaltoNPC;  // Rectángulo para resaltar la opción del NPC

        // Helpers visuales
        void setLeyendaTurno();
        sf::FloatRect boundsOpcion(int idx) const; // devuelve bounds de A/B/C/D
        void dibujarResaltoNpc(sf::RenderWindow& window); // pinta el resalto si m_npcOpcion>=0

        sf::SoundBuffer m_bufEmpirista;
        sf::SoundBuffer m_bufRacionalista;
        sf::Sound m_sndEmpirista;
        sf::Sound m_sndRacionalista;
};

#endif // COMBATE_H
