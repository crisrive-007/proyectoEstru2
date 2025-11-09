#include "MinijuegoPolitica.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// Zona de juego: debajo del cuadro de diálogo
struct ZonaJuego {
    float left, top, width, height, cx, cy;
};

static inline ZonaJuego calcularZonaJuego(const sf::RenderWindow& win, const sf::Sprite& dialog) {
    auto wsz = win.getSize();
    auto db  = dialog.getGlobalBounds();
    float top = db.size.y + 20.f;            // 20px de margen bajo el cuadro
    float left = 0.f;
    float width = float(wsz.x);
    float height = float(wsz.y) - top - 30.f; // 30px margen inferior
    return {left, top, width, height, left + width*0.5f, top + height*0.5f};
}

// ==== helper SFML 3: crear IntRect con (pos, size) ====
static inline sf::IntRect IRect(int x, int y, int w, int h) {
    return sf::IntRect({x, y}, {w, h});
}

// ==== helper: word wrap de texto según ancho ====
static std::string wrapText(const std::string& s, sf::Font& font, unsigned size, float maxWidth) {
    sf::Text probe(font); probe.setFont(font); probe.setCharacterSize(size);
    std::string out, word, line;
    auto flushLine = [&]{ if(!out.empty()) out += "\n"; out += line; line.clear(); };
    std::istringstream iss(s);
    while (iss >> word) {
        std::string tryLine = line.empty() ? word : line + " " + word;
        probe.setString(tryLine);
        if (probe.getGlobalBounds().size.x > maxWidth) {
            if (line.empty()) { // palabra más larga que el ancho
                if(!out.empty()) out += "\n";
                out += tryLine;  // la ponemos tal cual
                line.clear();
            } else {
                flushLine();
                line = word;
            }
        } else {
            line = tryLine;
        }
    }
    if (!line.empty()) flushLine();
    return out;
}

MinijuegoPolitica::MinijuegoPolitica(GestorEstados* g, sf::RenderWindow& w, Personaje& p)
: Estado(g,p), m_window(w), m_personaje(p),
  m_sprFondo(m_texFondo), m_sprDialog(m_texDialog),
  m_sprFosil(m_texFosil), m_sprZubat(m_texZubat), m_sprSandslash(m_texSandslash),
  m_txtA(m_font), m_txtB(m_font), m_txtC(m_font), m_txtD(m_font),
  m_txtFeedback(m_font), m_txtFin(m_font), m_txtHUD(m_font), m_txtPregunta(m_font),
  m_bgm(m_bufBgm), m_sndCorrect(m_bufCorrect), m_sndWrong(m_bufWrong), m_sndDig(m_bufDig)
{
    cargarAssets();
    armarPreguntas();
    iniciar();
}

// ==== assets ====
void MinijuegoPolitica::cargarAssets() {
    auto loadT = [&](sf::Texture& t, const std::string& path){
        if (!t.loadFromFile(path)) std::cerr << "[ERROR] No se pudo cargar: " << path << "\n";
    };

    loadT(m_texFondo,     "assets/MinijuegoPolitica/fondo.png");
    loadT(m_texDialog,    "assets/MinijuegoPolitica/dialog.png");
    loadT(m_texMonticulo, "assets/MinijuegoPolitica/monticulo.png");
    loadT(m_texFosil,     "assets/MinijuegoPolitica/fosil.png");
    loadT(m_texZubat,     "assets/MinijuegoPolitica/zubat.png");
    loadT(m_texSandslash, "assets/MinijuegoPolitica/sandslash_sheet.png");

    // fuente
    if (!m_font.openFromFile("assets/Pokemon_GB.ttf"))
        std::cerr << "[ERROR] No se pudo cargar la fuente Pokemon_GB.ttf\n";

    if (m_bufBgm.loadFromFile("assets/MinijuegoPolitica/bgm.mp3")) {
        m_bgm.setBuffer(m_bufBgm);
        m_bgm.setLooping(true);
        std::cout << "[DEBUG] BGM cargado\n";
    } else {
        std::cerr << "[ERROR] No se pudo cargar BGM\n";
    }

    if (m_bufCorrect.loadFromFile("assets/MinijuegoPolitica/correct.mp3")) {
        m_sndCorrect.setBuffer(m_bufCorrect);
        std::cout << "[DEBUG] Sonido de acierto cargado\n";
    }

    if (m_bufWrong.loadFromFile("assets/MinijuegoPolitica/wrong.mp3")) {
        m_sndWrong.setBuffer(m_bufWrong);
        std::cout << "[DEBUG] Sonido de error cargado\n";
    }

    // fondo y diálogo
    configurarFondo();
    m_sprDialog.setTexture(m_texDialog, true);
    m_sprDialog.setScale({12.f, 7.f});
    m_sprDialog.setPosition({0.f, 0.f});

    // === Posiciones (vector) ===
    m_posMonticulos.clear();
    m_posMonticulos.emplace_back(210.f,  660.f);
    m_posMonticulos.emplace_back(640.f,  660.f);
    m_posMonticulos.emplace_back(1030.f, 660.f);
    m_posMonticulos.emplace_back(1450.f, 660.f);

    // === Montículos (vector) ===
    m_monticulos.clear();
    for (const auto& pos : m_posMonticulos) {
        sf::Sprite spr(m_texMonticulo);
        spr.setTexture(m_texMonticulo, true);
        spr.setScale({0.2f,0.2f});
        spr.setPosition(pos);
        m_monticulos.push_back(spr);
    }

    m_monticuloActivo.assign(m_monticulos.size(), true);

    // animación de sandslash: 3 frames horizontales
    m_texSandslash.setSmooth(false);
    auto sz = m_texSandslash.getSize();
    if (sz.x >= 3 && sz.y >= 1) {
        m_frameSize = { int(sz.x/3), int(sz.y) };
    }
    m_framesSandslash.clear();
    for (int i = 0; i < 3; ++i)
        m_framesSandslash.emplace_back(IRect(i * m_frameSize.x, 0, m_frameSize.x, m_frameSize.y));

    // === Sandshrew/Sandslash al centro de la zona de juego ===
    m_sprSandslash.setTexture(m_texSandslash, true);
    m_sprSandslash.setTextureRect(m_framesSandslash[0]);
    m_sprSandslash.setScale({4.f,4.f});

    {
        const ZonaJuego zona2 = calcularZonaJuego(m_window, m_sprDialog);
        auto gb = m_sprSandslash.getGlobalBounds();
        m_sprSandslash.setPosition({ zona2.cx - gb.size.x * 0.5f, zona2.top + zona2.height * 0.48f - gb.size.y });
        m_posInicialSandslash = m_sprSandslash.getPosition();   // <-- guarda posición inicial
    }

    // fósil oculto
    m_sprFosil.setTexture(m_texFosil, true);
    m_sprFosil.setColor(sf::Color(255,255,255,0));

    // zubat oculto
    m_sprZubat.setTexture(m_texZubat, true);
    m_sprZubat.setScale({4.f,4.f});
    m_sprZubat.setColor(sf::Color(255,255,255,0));
}

void MinijuegoPolitica::configurarFondo() {
    m_sprFondo.setTexture(m_texFondo, true);
    m_sprFondo.setPosition({0.f, 240.f});           // bajo el cuadro de diálogo
    const auto ws = m_window.getSize();
    const auto ts = m_texFondo.getSize();
    if (ts.x && ts.y) {
        float scaleX = float(ws.x)/ts.x;  // llenar ancho de ventana
        float targetH = 835.f;            // alto de zona de juego (como MinijuegoArte)
        float scaleY = targetH/ts.y;
        m_sprFondo.setScale({scaleX, scaleY});
    }
}

// ==== banco de preguntas (cola FIFO) ====
void MinijuegoPolitica::armarPreguntas() {
    while (!m_cola.empty()) m_cola.pop();

    m_cola.push({"Durante el renacimiento, el modelo de gobierno es uno de los siguientes:", {"Monarquia absoluta","Tirania republicana","Democracia participativa","Liberalismo politico"}, 0});
    m_cola.push({"De los siguientes acontecimientos, selecciones el que inicia el período moderno:", {"Toma de Constantinopla","Tratado de paz de Westfalia","Toma de la Bastilla","La ruta de la seda"}, 1});
    m_cola.push({"Durante el siglo XV, la sociedad se estratifica en tres estamentos definidos:", {"Clase media, baja y alta","Nobleza, clero y estado llano","Artesanos, guardianes y gobernantes","Reyes, nobleza y pecheros"}, 1});
    m_cola.push({"Aparece el realismo politico, que se basaba en un orden establecido, explicacion de un sistema y recomendaciones de como gobernar:", {"Tomas Moro","Jean Bodin","Nicolas Maquiavelo","Erasmo de Rotterdam"}, 2});
    m_cola.push({"Terminada la edad media, en el contexto de la politica resulta que:", {"La Iglesia resalta su poder","La Iglesia pierde el papel rector en la politica","La Iglesia evangelica se posiciona en la politica","La politica desaparece"}, 1});
}

// ==== iniciar ====
void MinijuegoPolitica::iniciar() {
    m_pendienteFin = false;
    m_correctas = 0;
    m_fallos = 0;
    m_excavando = false;
    m_zubatActivo = false;
    m_timerZubat = 0.f;
    m_fase = Fase::EsperaSeleccion;

    m_sprSandslash.setTextureRect(m_framesSandslash[0]);
    m_sprFosil.setColor(sf::Color(255,255,255,0));
    m_sprZubat.setColor(sf::Color(255,255,255,0));

    if (m_cola.empty()) armarPreguntas();

    // todos visibles otra vez al iniciar
    for (size_t i=0;i<m_monticulos.size();++i) {
        m_monticuloActivo[i] = true;
        m_monticulos[i].setColor(sf::Color::White);
    }

    m_bgm.play();
    cargarPreguntaActual();
}

// ==== UI del cuadro de diálogo y opciones ====
void MinijuegoPolitica::layoutDialogUI() {
    const float padX = 120.f;  // margen texto
    const float padY = 48.f;
    const float ancho = 1600.f;

    // Enunciado con wrap
    const unsigned ENUN_SIZE = 28;
    m_txtPregunta.setFont(m_font);
    m_txtPregunta.setCharacterSize(ENUN_SIZE);
    m_txtPregunta.setFillColor(sf::Color::Black);
    m_txtPregunta.setString(wrapText(m_actual.enunciado, m_font, ENUN_SIZE, ancho));
    m_txtPregunta.setPosition({padX, padY});

    // Opciones alineadas debajo
    const unsigned OPT_SIZE = 26;
    auto setup = [&](sf::Text& t){
        t.setFont(m_font); t.setCharacterSize(OPT_SIZE);
        t.setFillColor(sf::Color::Black);
        t.setOutlineColor(sf::Color::White);
        t.setOutlineThickness(2.f);
    };
    setup(m_txtA); setup(m_txtB); setup(m_txtC); setup(m_txtD);
    m_txtA.setString("A) " + m_actual.opciones[0]);
    m_txtB.setString("B) " + m_actual.opciones[1]);
    m_txtC.setString("C) " + m_actual.opciones[2]);
    m_txtD.setString("D) " + m_actual.opciones[3]);

    float oy = padY + 90.f, dy = 40.f;
    m_txtA.setPosition({padX, oy + dy*0});
    m_txtB.setPosition({padX, oy + dy*1});
    m_txtC.setPosition({padX, oy + dy*2});
    m_txtD.setPosition({padX, oy + dy*3});

    // Letras A-D sobre montículos (vector) centradas y a +offsetY
    m_txtLetras.clear();
    const char* L = "ABCD";
    const float offsetY = -26.f; // altura sobre el borde superior del montículo

    for (size_t i = 0; i < m_monticulos.size(); ++i) {
        if (!m_monticuloActivo[i]) continue;
        sf::Text t(m_font);
        t.setFont(m_font);
        t.setString(std::string(1, L[i]));
        t.setCharacterSize(28);
        t.setFillColor(sf::Color::Black);
        t.setOutlineColor(sf::Color::White);
        t.setOutlineThickness(2.f);

        auto lb = t.getGlobalBounds();
        auto mb2 = m_monticulos[i].getGlobalBounds();
        const float cx = mb2.position.x + mb2.size.x * 0.5f;
        const float x  = cx - lb.size.x * 0.5f;
        const float y  = mb2.position.y + offsetY;

        t.setPosition({x, y});
        m_txtLetras.push_back(t);
    }

    // HUD
    m_txtHUD.setFont(m_font);
    m_txtHUD.setCharacterSize(22);
    m_txtHUD.setFillColor(sf::Color::Black);
    m_txtHUD.setString("Fósiles: " + std::to_string(m_correctas) + "/5");
    m_txtHUD.setPosition({50.f, 980.f});
}

void MinijuegoPolitica::cargarPreguntaActual() {
    if (m_cola.empty()) {
        terminar("fin"); // El string no se muestra; lo sustituimos más abajo.
        return;
    }

    m_excavando = false;
    m_sprSandslash.setTextureRect(m_framesSandslash[0]);
    m_sprSandslash.setPosition(m_posInicialSandslash);
    m_sprFosil.setColor(sf::Color(255,255,255,0));
    m_zubatActivo = false;
    m_sprZubat.setColor(sf::Color(255,255,255,0));
    m_idxSeleccion = -1;

    // Reaparecen todos
    for (size_t i = 0; i < m_monticulos.size(); ++i) {
        m_monticuloActivo[i] = true;
        m_monticulos[i].setColor(sf::Color::White);
    }

    // Siguiente de la cola
    m_actual = m_cola.front();

    // Reconstruye UI (enunciado + opciones + letras)
    layoutDialogUI();

    // Asegura que no quede ningún feedback viejo
    m_txtFeedback.setString("");
}

// ==== interacción ====
void MinijuegoPolitica::procesarSeleccion(int idx) {
    m_idxSeleccion = idx;
    const bool correcto = (idx == m_actual.correcta);
    auto mb = m_monticulos[idx].getGlobalBounds();
    // punto un poco al frente del montículo
    m_objetivoActual = { mb.position.x + mb.size.x*0.35f, mb.position.y };
    resolverResultado(correcto);
}

void MinijuegoPolitica::resolverResultado(bool correcto) {
    // Siempre limpiamos las opciones y letras al responder
    limpiarOpciones();

    if (correcto) {
        m_correctas++;
        m_sndCorrect.play();
        m_sndDig.play();
        startAnimExcavar(m_objetivoActual);

        // Mostrar feedback en el ENUNCIADO (como pediste)
        m_txtPregunta.setString("Respuesta correcta, has encontrado un fosil");
    } else {
        m_fallos++;
        m_sndWrong.play();

        // Feedback en el ENUNCIADO
        m_txtPregunta.setString("Respuesta incorrecta, ha aparecido un Zubat");

        // Zubat durante ~2s
        m_zubatActivo = true;
        m_timerZubat = 0.f;
        m_sprZubat.setColor(sf::Color::White);
        m_sprZubat.setPosition({ m_objetivoActual.x + 40.f, m_objetivoActual.y - 90.f });

        // Ocultamos solo el montículo clicado (para “desaparecerlo”)
        if (m_idxSeleccion >= 0 && m_idxSeleccion < (int)m_monticulos.size()) {
            m_monticuloActivo[m_idxSeleccion] = false;
            m_monticulos[m_idxSeleccion].setColor(sf::Color(255,255,255,0));
            // OJO: NO llamamos a layoutDialogUI() aquí, porque
            // esa función reescribe m_txtPregunta con el enunciado original.
        }
    }

    // Avanzamos la cola + HUD
    if (!m_cola.empty()) m_cola.pop();
    m_txtHUD.setString("Fósiles: " + std::to_string(m_correctas) + "/5");

    // ¿terminó (5 correctas) o ya no hay preguntas?
    m_pendienteFin = (m_correctas >= 5) || m_cola.empty();

    // Pasamos a fase de feedback: Enter continúa
    m_fase = Fase::Feedback;
}

// ==== anim ====
void MinijuegoPolitica::startAnimExcavar(const sf::Vector2f& posMonticulo) {
    m_excavando = true;
    m_frameIdx = 0;
    m_frameTime = 0.f;

    // mover sandslash frente al montículo y ocultar fósil hasta “pum”
    auto gb = m_sprSandslash.getGlobalBounds();
    m_sprSandslash.setPosition({ posMonticulo.x - gb.size.x*0.3f, posMonticulo.y - 38.f });
    m_sprSandslash.setTextureRect(m_framesSandslash[0]);
    m_sprFosil.setColor(sf::Color(255,255,255,0));
}

void MinijuegoPolitica::actualizarAnimSandslash(float dt) {
    if (!m_excavando) return;
    m_frameTime += dt;
    if (m_frameTime >= m_frameDur) {
        m_frameTime = 0.f;
        m_frameIdx++;
        if (m_frameIdx >= (int)m_framesSandslash.size()) {
            // ciclo corto de cavado completo: revelar fósil
            m_excavando = false;
            m_frameIdx = 0;
            m_sprSandslash.setTextureRect(m_framesSandslash[0]);
            m_sprFosil.setColor(sf::Color::White);
            m_sprFosil.setPosition({ m_objetivoActual.x + 18.f, m_objetivoActual.y - 10.f });
            m_fase = Fase::Feedback;
        } else {
            m_sprSandslash.setTextureRect(m_framesSandslash[m_frameIdx]);
        }
    }
}

// ==== bucles ====
void MinijuegoPolitica::actualizar() {
    const float dt = m_clk.restart().asSeconds();
    actualizarAnimSandslash(dt);

    if (m_zubatActivo) {
        m_timerZubat += dt;
        if (m_timerZubat > 2.f) {
            m_zubatActivo = false;
            m_sprZubat.setColor(sf::Color(255,255,255,0));
        }
    }
}

void MinijuegoPolitica::dibujar(sf::RenderWindow& w) {
    w.setView(w.getDefaultView());
    // orden correcto
    w.draw(m_sprFondo);
    w.draw(m_sprDialog);
    for (size_t i=0; i<m_monticulos.size(); ++i) {
        if (!m_monticuloActivo[i]) continue;
        w.draw(m_monticulos[i]);
    }
    w.draw(m_sprSandslash);
    if (m_sprFosil.getColor().a > 0) w.draw(m_sprFosil);
    if (m_zubatActivo) w.draw(m_sprZubat);

    // textos
    w.draw(m_txtPregunta);
    w.draw(m_txtA); w.draw(m_txtB); w.draw(m_txtC); w.draw(m_txtD);
    for (auto& t : m_txtLetras) w.draw(t);
    w.draw(m_txtHUD);

    if (!m_txtFeedback.getString().isEmpty()) {
        auto b = m_txtFeedback.getGlobalBounds();
        m_txtFeedback.setPosition({960.f - b.size.x/2.f, 780.f});
        w.draw(m_txtFeedback);
    }

    /*if (m_fase == Fase::Fin) {
        setTextoCentro(m_txtFin, 960.f, 540.f);
        w.draw(m_txtFin);
    }*/
}

void MinijuegoPolitica::manejarEventos(sf::RenderWindow& w) {
    while (auto ev = w.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { w.close(); return; }

        if (auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::Enter) {
                if (m_fase == Fase::Feedback) {
                    if (m_pendienteFin) {
                        // Mensaje final EXACTO en m_txtPregunta
                        limpiarOpciones();
                        if (m_correctas >= 5) {
                            m_txtPregunta.setString("Felicidades has encontrado todos los fosiles.\nHas ganado una vida extra.");
                            m_personaje.ganarVida();
                        } else {
                            m_txtPregunta.setString("Mala suerte, no ganas una vida extra");
                        }

                        // Pasamos a Fin. Enter saldrá del minijuego.
                        m_fase = Fase::Fin;
                        m_pendienteFin = false;
                        m_bgm.stop();
                    } else {
                        // Más preguntas → siguiente
                        cargarPreguntaActual();
                        m_fase = Fase::EsperaSeleccion;
                    }
                } else if (m_fase == Fase::Fin) {
                    salirDelMinijuego();
                }
            }
        }

        if (auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (m->button == sf::Mouse::Button::Left && m_fase == Fase::EsperaSeleccion) {
                sf::Vector2f mp{ float(m->position.x), float(m->position.y) };
                for (int i=0; i<(int)m_monticulos.size(); ++i) {
                    if (!m_monticuloActivo[i]) continue; // <-- ignora inactivos
                    if (containsPoint(m_monticulos[i].getGlobalBounds(), mp)) {
                        procesarSeleccion(i);
                        break;
                    }
                }
            }
        }
    }
}

// ==== utils ====
void MinijuegoPolitica::terminar(const std::string& msg) {
    m_txtFin.setFont(m_font);
    m_txtFin.setCharacterSize(30);
    m_txtFin.setFillColor(sf::Color::Black);
    m_txtFin.setOutlineColor(sf::Color::White);
    m_txtFin.setOutlineThickness(2.f);
    m_txtFin.setString(msg + "\n(ENTER para continuar)");
    m_bgm.stop();
    m_fase = Fase::Fin;
}

void MinijuegoPolitica::salirDelMinijuego() {
    gestor->sacarEstado();
    personaje.setPosition(880, 295); // ajusta si lo necesitas
}

void MinijuegoPolitica::limpiarOpciones() {
    m_txtA.setString("");
    m_txtB.setString("");
    m_txtC.setString("");
    m_txtD.setString("");
    m_txtLetras.clear();
}
