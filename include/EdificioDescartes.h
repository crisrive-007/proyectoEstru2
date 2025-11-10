#ifndef EDIFICIODESCARTES_H
#define EDIFICIODESCARTES_H

#include "Estado.h"
#include "Pregunta.h"
#include <SFML/Graphics.hpp>
#include <queue>
#include <vector>
#include <array>
#include <string>
#include <optional>

enum class FaseDesc { Evidencia, Analisis, Sintesis, Enumeracion, Fin };
enum class HitDesc  { Ninguno, NPC, M_Evidencia, M_Analisis, M_Sintesis, M_Enumeracion };

struct ObjEvidencia {
    std::string nombre;
    sf::Texture tex;
    sf::Sprite  spr;
    bool real = false;
    bool elegido = false;

    // Constructor explícito: Sprite se construye con la textura
    ObjEvidencia(const std::string& nombre_,
                 const std::string& filePath,
                 bool real_,
                 sf::Vector2f pos,
                 sf::Vector2f scale = {0.1f, 0.1f})
    : nombre(nombre_)
    , tex()
    , spr(tex)       // <-- clave: SFML3 exige textura al construir el sprite
    , real(real_)
    , elegido(false)
    {
        const bool ok = tex.loadFromFile(filePath);
        if (!ok) {
            std::cerr << "[DESCARTES] No se pudo cargar: " << filePath << "\n";
        }
        spr.setTexture(tex, true);
        spr.setPosition(pos);
        spr.setScale(scale);
    }

    // Evita copias (por seguridad con referencias internas); permite moves
    ObjEvidencia(const ObjEvidencia&) = delete;
    ObjEvidencia& operator=(const ObjEvidencia&) = delete;
    ObjEvidencia(ObjEvidencia&&) = default;
    ObjEvidencia& operator=(ObjEvidencia&&) = default;
};

class EdificioDescartes : public Estado {
public:
    EdificioDescartes(GestorEstados* g, sf::RenderWindow& win, Personaje& pj);
    ~EdificioDescartes() override = default;

    void manejarEventos(sf::RenderWindow& win) override;
    void actualizar() override;
    void dibujar(sf::RenderWindow& win) override;

private:
    // referencias base
    sf::RenderWindow& m_window;
    Personaje&        m_personaje;

    // escena
    sf::Texture m_texFondo, m_texNPC;
    sf::Sprite  m_sprFondo, m_sprNPC;

    // mesas (zonas clicables)
    sf::Texture m_texEvi, m_texAna, m_texSin, m_texEnum;
    sf::Sprite  m_sprEvi, m_sprAna, m_sprSin, m_sprEnum;

    // objetos mesa evidencia
    std::vector<ObjEvidencia> m_objetos;       // 4–6 objetos
    std::vector<std::string>  m_selEvidencia;  // nombres elegidos

    // inferencias producidas en análisis
    std::vector<std::string>  m_inferencias;

    // síntesis: pares correctos (objeto ↔ inferencia)
    std::vector<std::pair<std::string,std::string>> m_relacionesOK;

    // enumeración: orden correcto
    std::vector<std::string>  m_ordenCorrecto;

    // UI diálogo
    sf::Font m_font;
    sf::Text m_txt, m_fb, m_hint;
    std::array<sf::Text,4> m_opts;

    // quiz y flujos
    std::queue<Pregunta> m_cola;
    Pregunta             m_actual{};
    bool m_mostrarQuiz = false;
    bool m_feedback    = false;
    int  m_sel         = -1;

    // estado de progreso
    bool m_npcHablado = false;
    bool m_eviOK = false, m_anaOK = false, m_sinOK = false, m_enumOK = false;
    FaseDesc m_fase = FaseDesc::Evidencia;
    HitDesc  m_cerca = HitDesc::Ninguno;

    // diálogo NPC
    std::vector<std::string> m_dialog;
    size_t m_dIdx = 0;
    bool   m_dialogOn = false;

    // helpers
    sf::FloatRect dibujarCajaDialogo(sf::RenderWindow& w);
    HitDesc detectarCercania() const;
    void startDialog(std::initializer_list<std::string> lines);
    bool nextDialog();

    // Fase 1: Evidencia (selección de objetos)
    void cargarObjetosEvidencia();
    void mostrarDilemaEvidencia();
    void toggleObjetoCercano(); // marca/desmarca objetos con SPACE
    bool evidenciaResuelta() const;

    // Fase 2: Análisis (inferencia con objetos elegidos)
    void generarInferenciasDesdeEvidencia();
    void cargarPreguntasAnalisis();
    void siguientePregunta();

    // Fase 3: Síntesis (relacionar objeto↔inferencia)
    void prepararSintesis();
    void cargarPreguntasSintesis();

    // Fase 4: Enumeración (ordenar conceptos → conclusión)
    void prepararEnumeracion();
    void cargarPreguntasEnumeracion();

    // layout/anim
    float m_hintBob = 0.f;

    bool m_evidenciaActiva = false;

    // Panel de Evidencia (grid clicable)
    bool m_needLayoutEvi = false;                 // recalcular layout al abrir el panel
    std::vector<sf::Vector2f> m_eviGridPos;      // posiciones dentro del panel
    sf::FloatRect m_eviPanelRect;                // rect del panel actual

    // Dibuja un panel tipo diálogo y devuelve su rect
    sf::FloatRect dibujarPanelEvidencia(sf::RenderWindow& w);

    sf::RectangleShape m_areaSalida;
    void interaccionSalida();
    static bool intersecta(const sf::FloatRect& A, const sf::FloatRect& B);
};

#endif
