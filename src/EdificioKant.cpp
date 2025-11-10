#include "EdificioKant.h"
#include "GestorEstados.h"
#include "ProgresoJuego.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

static inline sf::Vector2f centerOf(const sf::Sprite& s) {
    auto r = s.getGlobalBounds(); // SFML 3: .position y .size
    return { r.position.x + r.size.x * 0.5f, r.position.y + r.size.y * 0.5f };
}
static inline float dist(sf::Vector2f a, sf::Vector2f b) {
    float dx = a.x - b.x, dy = a.y - b.y;
    return std::sqrt(dx*dx + dy*dy);
}

static inline bool intersecta(const sf::FloatRect& A, const sf::FloatRect& B) {
    const float ax1 = A.position.x;
    const float ay1 = A.position.y;
    const float ax2 = ax1 + A.size.x;
    const float ay2 = ay1 + A.size.y;

    const float bx1 = B.position.x;
    const float by1 = B.position.y;
    const float bx2 = bx1 + B.size.x;
    const float by2 = by1 + B.size.y;

    return (ax1 < bx2) && (ax2 > bx1) && (ay1 < by2) && (ay2 > by1);
}

EdificioKant::EdificioKant(GestorEstados* gestor, sf::RenderWindow& window, Personaje& personaje)
: Estado(gestor, personaje)
, m_window(window)
, m_personaje(personaje)
, m_sprFondo(m_texFondo), m_sprLibroPract(m_texLibroPract), m_sprLibroPura(m_texLibroPura), m_sprNPC(m_texNPC), m_sprSolar(m_texSolar)
, m_enunciado(m_font, "", 20)
, m_fb(m_font, "", 20)
, m_hint(m_font)
, m_opts{{
    sf::Text(m_font, "", 20),
    sf::Text(m_font, "", 20),
    sf::Text(m_font, "", 20),
    sf::Text(m_font, "", 20)
}}
{
    if (!ProgresoJuego::get().puedeEntrar(ProgresoJuego::Nivel::Kant)) {
        std::cout << "🔒 Edificio Kant bloqueado: termina el Gimnasio primero.\n";
        gestor->sacarEstado();
        return;
    }
    // === FONDO DEL AULA ===
    if (!m_texFondo.loadFromFile("assets/Tilesets/aulaKant.png")) {
        std::cerr << "Error: no se pudo cargar assets/Tilesets/aulaKant.png\n";
    }
    m_sprFondo.setTexture(m_texFondo, true);

    auto sz = m_texFondo.getSize();
    auto winSize = m_window.getSize();
    float scale = std::min(
        static_cast<float>(winSize.x) / static_cast<float>(sz.x),
        static_cast<float>(winSize.y) / static_cast<float>(sz.y)
    );
    m_sprFondo.setScale({scale, scale});
    sf::FloatRect fondoB = m_sprFondo.getGlobalBounds();
    m_sprFondo.setPosition({
        (winSize.x - fondoB.size.x) / 2.f,
        (winSize.y - fondoB.size.y) / 2.f
    });

    // === SPRITES INTERACTIVOS ===
    if (!m_texLibroPura.loadFromFile("assets/EdificioKant/libro_pura.png"))
        std::cerr << "Error: libro_pura.png no encontrado\n";
    if (!m_texLibroPract.loadFromFile("assets/EdificioKant/libro_practica.png"))
        std::cerr << "Error: libro_practica.png no encontrado\n";
    m_texSolar.loadFromFile("assets/EdificioKant/sistemaSolar.png");

    m_sprLibroPura.setTexture(m_texLibroPura, true);
    m_sprLibroPract.setTexture(m_texLibroPract, true);
    m_sprSolar.setTexture(m_texSolar, true);

    // Posiciones y escala
    m_sprLibroPura.setPosition({505.f, 420.f});
    m_sprLibroPract.setPosition({1170.f, 420.f});
    m_sprLibroPura.setScale({0.5f, 0.5f});
    m_sprLibroPract.setScale({0.5f, 0.5f});

    // === SISTEMA SOLAR: foto al inicio ===
    cargarSolarEstatico();
    m_sprSolar.setPosition({870.f, 60.f});
    m_sprSolar.setScale({0.4f, 0.4f});

    // === NPC (opcional) ===
    if (m_texNPC.loadFromFile("assets/EdificioKant/npcKant.png")) {
        m_sprNPC.setTexture(m_texNPC, true);
        m_sprNPC.setPosition({930.f, 195.f});
        m_sprNPC.setScale({2.f, 2.f});
    }

    // === FUENTE ===
    if (!m_font.openFromFile("assets/Pokemon_GB.ttf")) {
        std::cerr << "Error: no se pudo cargar assets/Pokemon_GB.ttf\n";
    }

    // Personaje
    m_personaje.establecerPosicion({920.f, 900.f});
    m_personaje.setScale(2.f, 2.f);

    cargarSurtidoKant(); // compat

    m_hint = sf::Text(m_font, "SPACE", 16);
    m_hint.setFillColor(sf::Color::White);              // Blanco sólido
    m_hint.setOutlineColor(sf::Color::Black);           // Contorno negro
    m_hint.setOutlineThickness(2.f);

    m_areaSalida.setSize({190.f, 32.f});
    m_areaSalida.setPosition({868.f, 980.f}); // ajusta según tu mapa
    m_areaSalida.setFillColor(sf::Color(255, 0, 0, 120));

    cargarColisionesMapa();
}

// === BANCO DE PREGUNTAS (compat, no usado al inicio) ===
void EdificioKant::cargarSurtidoKant() {}

// === SISTEMA DE PREGUNTAS ===
void EdificioKant::siguientePregunta() {
    if (m_cola.empty()) {
        m_mostrarQuiz = false;
        m_feedback = true;

        if (m_origen == InteraccionKant::LibroPura)   m_puraCompletado  = true;
        if (m_origen == InteraccionKant::LibroPract)  m_practCompletado = true;

        if (m_puraCompletado && m_practCompletado) {
            m_fb.setString("¡Ambos libros abiertos! Ve al sistema solar para culminar el aula. ENTER.");
        } else {
            m_fb.setString("Libro completado. Explora el otro libro o consulta el sistema solar. ENTER.");
        }
        return;
    }

    m_actual = m_cola.front(); m_cola.pop();
    m_enunciado.setString(m_actual.enunciado);
    for (int i = 0; i < 4; ++i)
        m_opts[i].setString(std::to_string(i + 1) + ") " + m_actual.opciones[i]);

    m_sel = -1;
    m_feedback = false;
    m_mostrarQuiz = true;
}

// === EVENTOS ===
void EdificioKant::manejarEventos(sf::RenderWindow& window) {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { window.close(); continue; }

        if (auto* key = ev->getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Escape) {
                gestor->sacarEstado(); return;
            }

            // ACTIVAR SOLO SI ESTÁ CERCA Y PRESIONA SPACE (no hay clics)
            if (!m_mostrarQuiz && key->code == sf::Keyboard::Key::Space) {
                if (m_cercano == InteraccionKant::NPC) {
                    mostrarDialogoNPC();
                    continue;
                }

                // Si no habló con NPC, no puede usar libros aún
                if (!m_npcHablado) {
                    m_feedback = true;
                    m_origen = InteraccionKant::Ninguna;
                    m_fb.setString("Primero habla con Kant (acercate y presiona SPACE).");
                    continue;
                }

                // Libros: solo si están habilitados y no han sido completados
                if (m_cercano == InteraccionKant::LibroPura) {
                    if (!m_librosHabilitados) {
                        m_feedback = true; m_origen = InteraccionKant::Ninguna;
                        m_fb.setString("Aun no puedes usar los libros. Habla con Kant primero.");
                    } else if (m_puraCompletado) {
                        m_feedback = true; m_origen = InteraccionKant::Ninguna;
                        m_fb.setString("Este libro ya fue completado.");
                    } else {
                        m_origen = InteraccionKant::LibroPura;
                        cargarPreguntasLibroPura();
                        siguientePregunta();
                    }
                    continue;
                }
                if (m_cercano == InteraccionKant::LibroPract) {
                    if (!m_librosHabilitados) {
                        m_feedback = true; m_origen = InteraccionKant::Ninguna;
                        m_fb.setString("Aun no puedes usar los libros. Habla con Kant primero.");
                    } else if (m_practCompletado) {
                        m_feedback = true; m_origen = InteraccionKant::Ninguna;
                        m_fb.setString("Este libro ya fue completado.");
                    } else {
                        m_origen = InteraccionKant::LibroPract;
                        cargarPreguntasLibroPract();
                        siguientePregunta();
                    }
                    continue;
                }
                if (m_cercano == InteraccionKant::Solar) {
                    m_origen = InteraccionKant::Solar;
                    mostrarPistaSolar();
                    continue;
                }
            }
        }

        // Durante el quiz
        if (m_mostrarQuiz) {
            if (auto* key = ev->getIf<sf::Event::KeyPressed>()) {
                if (!m_feedback) {
                    if (key->code == sf::Keyboard::Key::Num1) m_sel = 0;
                    if (key->code == sf::Keyboard::Key::Num2) m_sel = 1;
                    if (key->code == sf::Keyboard::Key::Num3) m_sel = 2;
                    if (key->code == sf::Keyboard::Key::Num4) m_sel = 3;

                    if (m_sel != -1) {
                        m_feedback = true;
                        m_fb.setString(m_sel == m_actual.correcta
                            ? "¡Correcto! Has aplicado bien la razon."
                            : "Incorrecto... Reflexiona antes de juzgar."
                        );
                    }
                } else if (key->code == sf::Keyboard::Key::Enter) {
                    siguientePregunta();
                }
            }
        }

        // ENTER cuando hay feedback / dialogo sin quiz visible
        if (!m_mostrarQuiz) {
            if (auto* key = ev->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Enter && m_feedback) {
                    // Si hay diálogo activo, avanzar una línea
                    if (m_dialogActivo) {
                        bool termino = advanceDialog(); // si es el final del Solar y m_finalSolar==true, sale del aula
                        (void)termino;
                        return;
                    }
                    // Si no es diálogo por líneas (feedback suelto), cerrar mensaje
                    m_feedback = false;
                    m_origen = InteraccionKant::Ninguna;
                    return;
                }
            }
        }
    }
}

void EdificioKant::actualizar() {
    // 1) Guardar posición previa ANTES de procesar input/movimiento
    m_prevPosJugador = m_personaje.getPosition();

    if (!m_mostrarQuiz)
        m_personaje.actualizarSinTiles(m_window.getSize().x, m_window.getSize().y);

    // 2) Clamp al área visible del aula
    sf::FloatRect aula = m_sprFondo.getGlobalBounds();
    {
        sf::FloatRect hb = m_personaje.obtenerHitbox();
        sf::Vector2f pos = m_personaje.getPosition();
        const float minX = aula.position.x;
        const float minY = aula.position.y;
        const float maxX = aula.position.x + aula.size.x - hb.size.x;
        const float maxY = aula.position.y + aula.size.y - hb.size.y;
        pos.x = std::clamp(pos.x, minX, maxX);
        pos.y = std::clamp(pos.y, minY, maxY);
        m_personaje.setPosition(pos.x, pos.y);
    }

    aplicarColisiones();

    // Detección de cercanía e iluminación suave
    m_cercano = detectarCercania();
    auto tint = [](sf::Sprite& s, bool on){
        s.setColor(on ? sf::Color(255,255,255,220) : sf::Color(255,255,255,255));
    };
    tint(m_sprLibroPura,  m_cercano == InteraccionKant::LibroPura);
    tint(m_sprLibroPract, m_cercano == InteraccionKant::LibroPract);
    tint(m_sprSolar,      m_cercano == InteraccionKant::Solar);

    actualizarSolarAnim();
    m_hintBob += 3.0f * (1.f/60.f);

    // === Trigger de salida ===
    interaccionSalida();
}

// === CAJA DE DIÁLOGO (más abajo) ===
sf::FloatRect EdificioKant::dibujarCajaDialogo(sf::RenderWindow& window) {
    const float W = static_cast<float>(window.getSize().x);
    const float H = static_cast<float>(window.getSize().y);
    const float boxW = W - 2.f * m_dialogMargin;
    const float boxH = m_dialogHeight;
    const float boxX = m_dialogMargin;
    const float boxY = H - boxH - m_dialogMargin;

    sf::RectangleShape box({boxW, boxH});
    box.setPosition({boxX, boxY});
    box.setFillColor(sf::Color(0, 0, 0, 190));
    // borde fino opcional:
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(255,255,255,40));
    window.draw(box);

    return sf::FloatRect({boxX, boxY}, {boxW, boxH}); // SFML 3
}

// === DIBUJADO PRINCIPAL ===
void EdificioKant::dibujar(sf::RenderWindow& window) {
    window.draw(m_sprFondo);
    window.draw(m_sprLibroPura);
    window.draw(m_sprLibroPract);
    window.draw(m_sprSolar);
    if (m_texNPC.getSize().x > 0) window.draw(m_sprNPC);

    if (m_debugColisiones) {
        for (const auto& r : m_dbgColisiones) window.draw(r);
        window.draw(m_areaSalida);
    }

    m_personaje.dibujar(window);

    // Hint contextual: si estás cerca y no hay quiz ni feedback activo
    if (!m_mostrarQuiz && !m_feedback && m_cercano != InteraccionKant::Ninguna) {
        sf::Sprite* target = nullptr;
        if (m_cercano == InteraccionKant::LibroPura && !m_puraCompletado)   target = &m_sprLibroPura;
        if (m_cercano == InteraccionKant::LibroPract && !m_practCompletado) target = &m_sprLibroPract;
        if (m_cercano == InteraccionKant::Solar)                             target = &m_sprSolar;
        if (m_cercano == InteraccionKant::NPC)                               target = &m_sprNPC;

        if (target) {
            auto b = target->getGlobalBounds();
            float cx = b.position.x + b.size.x * 0.5f;
            float cy = b.position.y - 18.f + std::sin(m_hintBob) * 4.f;
            m_hint.setPosition({cx - m_hint.getGlobalBounds().size.x * 0.5f, cy});
            window.draw(m_hint);
        }
    }

    if (m_mostrarQuiz) {
        sf::FloatRect r = dibujarCajaDialogo(window);
        float x = r.position.x + m_dialogPad;
        float y = r.position.y + m_dialogPad;

        // Enunciado
        m_enunciado.setPosition({x, y});
        m_enunciado.setFillColor(sf::Color::White);
        window.draw(m_enunciado);

        // Opciones
        y += 44.f;
        for (int i = 0; i < 4; ++i) {
            m_opts[i].setPosition({x, y});
            if (m_feedback) {
                if (i == m_actual.correcta)      m_opts[i].setFillColor(sf::Color(60, 200, 60));
                else if (i == m_sel)             m_opts[i].setFillColor(sf::Color(220, 60, 60));
                else                              m_opts[i].setFillColor(sf::Color::White);
            } else {
                m_opts[i].setFillColor(sf::Color::White);
            }
            window.draw(m_opts[i]);
            y += 28.f;
        }

        if (m_feedback) {
            m_fb.setFillColor(sf::Color(230, 230, 230));
            m_fb.setPosition({x, r.position.y + r.size.y - m_dialogPad - 26.f});
            window.draw(m_fb);
        }
    }
    else if (m_feedback) {
        sf::FloatRect r = dibujarCajaDialogo(window);
        float x = r.position.x + m_dialogPad;
        float y = r.position.y + m_dialogPad;

        m_fb.setFillColor(sf::Color::White);
        m_fb.setPosition({x, y});
        window.draw(m_fb);
    }

    window.draw(m_areaSalida);
}

// === BANCOS POR LIBRO ===
void EdificioKant::cargarPreguntasLibroPura() {
    while (!m_cola.empty()) m_cola.pop();
    m_cola.push({
        "Para Kant, conocemos lo que las cosas nos permiten a traves de los sentidos (lo fenomenico):",
        {"Conocimiento Noumenico","Conocimiento Fenomenico","Conocimiento Empirico","Conocimiento Racional"},
        1
    });
    m_cola.push({
        "El lema de la Ilustracion 'Sapere aude' significa:",
        {"Sopesa tus acciones","Atrevete a saber por ti mismo","Saber a la fuerza","Someterse al conocimiento"},
        1
    });
    m_cola.push({
        "Kant cambia el centro del conocimiento del objeto al sujeto; a esto se le llama:",
        {"Subjetivismo","Prejuicio","Giro Copernicano","Suerte"},
        2
    });
}

void EdificioKant::cargarPreguntasLibroPract() {
    while (!m_cola.empty()) m_cola.pop();
    m_cola.push({
        "El mandato cuya obligacion depende de premio o castigo es el:",
        {"Imperativo Hipotetico","Imperativo Categorico","Ambos","Ninguno"},
        0
    });
    m_cola.push({
        "La postura conciliadora que integra empirismo y racionalismo en Kant es:",
        {"Racionalismo","Empirismo","Criticismo","Escepticismo"},
        2
    });
    m_cola.push({
        "Obra que define su epistemologia (ojo: aqui es un distractor etico):",
        {"Critica de la razon practica","Critica de la razon pura","Critica del juicio","Critica fenomenologica"},
        1
    });
}

// === PISTA / FINAL SOLAR ===
void EdificioKant::mostrarPistaSolar() {
    // Si aun no terminó ambos libros, pista breve
    if (!m_puraCompletado || !m_practCompletado) {
        startDialog(DialogOwner::Solar, {
            "Observa el Sistema Solar: como Copernico, mueve tu atencion del objeto al sujeto.",
            "Completa ambos libros para comprender el Giro Copernicano.",
            "Vuelve cuando hayas unido la Razon Pura y la Razon Practica."
        });
        // No es final todavía
        m_finalSolar = false;
        return;
    }

    // Cambiar a animación de frames si no estaba activa
    if (!m_solarAnimActivo) {
        cargarSolarFrames("assets/EdificioKant/solar", "solar", 1, 130);
        m_sprSolar.setPosition({870.f, 60.f});
        m_sprSolar.setScale({0.4f, 0.4f});
    }

    // Diálogo 3 — Final (Sistema Solar) -> al terminar, salir del aula
    m_finalSolar = true;
    startDialog(DialogOwner::Solar, {
        "Mira como los planetas giran... antes creiamos que el Sol giraba alrededor de nosotros.",
        "Tambien creimos que el conocimiento giraba en torno al objeto... pero es el sujeto quien\nmueve el universo del pensar.",
        "Has comprendido que conocer y obrar son uno solo: el deber nace de la libertad, y la libertad\ndel entendimiento.",
        "Has restaurado el Logos. El Aula vuelve a brillar... y con ella, el pensamiento.",
        "Recuerda: El conocimiento sin moral es ciego; la moral sin conocimiento es vacia.",
        "Ve ahora... Sapere Aude: atrevete a saber."
    });
}

// === SOLAR: foto estática ===
void EdificioKant::cargarSolarEstatico() {
    if (!m_texSolarStatic.loadFromFile("assets/EdificioKant/sistemaSolar.jpg")) {
        m_texSolarStatic.loadFromFile("assets/EdificioKant/sistemaSolar.png");
    }
    m_sprSolar.setTexture(m_texSolarStatic, true);
}

// === SOLAR: frames solar1..solarN ===
void EdificioKant::cargarSolarFrames(const std::string& carpeta,
                                     const std::string& prefijo,
                                     int start, int end) {
    m_solarFrames.clear();
    m_solarIndex = 0;

    for (int i = start; i <= end; ++i) {
        std::ostringstream oss; oss << carpeta << "/" << prefijo << i << ".png";
        sf::Texture tex;
        if (tex.loadFromFile(oss.str())) {
            m_solarFrames.push_back(std::move(tex));
        } else {
            std::cout << "[KANT] Falta frame: " << oss.str() << "\n";
        }
    }

    m_solarAnimActivo = !m_solarFrames.empty();
    if (m_solarAnimActivo) {
        m_sprSolar.setTexture(m_solarFrames.front(), true);
    }
}

void EdificioKant::actualizarSolarAnim() {
    if (!m_solarAnimActivo || m_solarFrames.empty()) return;
    const float frameTime = 1.f / m_solarFps;
    if (m_solarClock.getElapsedTime().asSeconds() >= frameTime) {
        m_solarClock.restart();
        m_solarIndex = (m_solarIndex + 1) % m_solarFrames.size();
        m_sprSolar.setTexture(m_solarFrames[m_solarIndex], true);
    }
}

// === Detección de cercanía ===
InteraccionKant EdificioKant::detectarCercania() const {
    sf::Vector2f p = m_personaje.getPosition(); // tu getter actual

    // Centros
    sf::Vector2f cPura  = centerOf(m_sprLibroPura);
    sf::Vector2f cPract = centerOf(m_sprLibroPract);
    sf::Vector2f cSolar = centerOf(m_sprSolar);
    sf::Vector2f cNPC   = centerOf(m_sprNPC);

    // 1) Antes de hablar con el NPC, SOLO el NPC es interactuable
    if (!m_npcHablado) {
        if (dist(p, cNPC) <= m_interactDist) return InteraccionKant::NPC;
        return InteraccionKant::Ninguna;
    }

    // 2) Si ya habló con el NPC, los libros quedan habilitados (según su estado)
    //    El sistema solar sigue disponible (pista/final), pero la salida es por el flow final.
    if (dist(p, cPura)  <= m_interactDist && !m_puraCompletado)  return InteraccionKant::LibroPura;
    if (dist(p, cPract) <= m_interactDist && !m_practCompletado) return InteraccionKant::LibroPract;
    if (dist(p, cSolar) <= m_interactDist)                       return InteraccionKant::Solar;

    // Si estás cerca de libros ya completados, no se consideran "interactuables"
    // pero podrías mostrar un hint distinto si quisieras (aquí devolvemos Ninguna).
    if (dist(p, cPura)  <= m_interactDist && m_puraCompletado)   return InteraccionKant::Ninguna;
    if (dist(p, cPract) <= m_interactDist && m_practCompletado)  return InteraccionKant::Ninguna;

    // NPC sigue estando, pero ya no es obligatorio; igual puede hablarse
    if (dist(p, cNPC) <= m_interactDist) return InteraccionKant::NPC;

    return InteraccionKant::Ninguna;
}

void EdificioKant::mostrarDialogoNPC() {
    // Diálogo 1 — Introducción (antes de habilitar libros)
    if (!m_npcHablado) {
        startDialog(DialogOwner::NPC, {
            "Ah... por fin alguien ha cruzado el umbral del Aula de la Razon.",
            "Durante siglos he custodiado este lugar, donde el pensamiento se examina a si mismo.",
            "El mundo se ha fracturado: confunden conocimiento con opinion, moral con conveniencia.",
            "El Logos, la fuerza que une verdad y deber, esta debilitado.",
            "Para restaurarlo, une las dos mitades de la razon: la Razon Pura y la Razon Practica.",
            "Solo cuando comprendas ambas, el cosmos del pensamiento volvera a girar.",
            "Busca los dos libros: uno guarda las leyes del conocer; el otro, las del obrar.",
            "Responde con sinceridad y reflexion. Aqui no se gana con rapidez, sino con entendimiento.",
            "Ahora ve... las paginas esperan al que piensa por si mismo."
        });
        return;
    }

    // Diálogo 2 — Si ya completó ambos libros y vuelve a hablar con Kant (opcional)
    if (m_puraCompletado && m_practCompletado && !m_finalSolar) {
        startDialog(DialogOwner::NPC, {
            "Has unido el saber con el deber.",
            "Muchos logran conocer... pocos obran segun lo que conocen.",
            "Acercate al Sistema Solar: alli comprenderas el Giro Copernicano de la Razon."
        });
        return;
    }

    // Diálogo breve en cualquier otro momento
    startDialog(DialogOwner::NPC, {
        "Los libros estan ante ti. La razon no se hereda: se conquista."
    });
}

void EdificioKant::startDialog(DialogOwner owner, std::initializer_list<std::string> lines) {
    m_dialogLines.assign(lines.begin(), lines.end());
    m_dialogIndex = 0;
    m_dialogOwner = owner;
    m_dialogActivo = true;

    m_mostrarQuiz = false;
    m_feedback = true;          // usamos el cuadro negro existente
    m_origen = (owner == DialogOwner::NPC) ? InteraccionKant::NPC : InteraccionKant::Solar;

    // Poner la primera línea en m_fb
    if (!m_dialogLines.empty()) {
        m_fb.setString(m_dialogLines[m_dialogIndex]);
    } else {
        m_fb.setString("");
        m_dialogActivo = false;
        m_feedback = false;
        m_dialogOwner = DialogOwner::None;
    }
}

// Avanza una línea. Si termina, devuelve true.
bool EdificioKant::advanceDialog() {
    if (!m_dialogActivo) return true;
    if (m_dialogIndex + 1 < m_dialogLines.size()) {
        m_dialogIndex++;
        m_fb.setString(m_dialogLines[m_dialogIndex]);
        return false;
    }
    // diálogo terminó
    m_dialogActivo = false;
    m_feedback = false;

    // Acciones al finalizar según el dueño del diálogo
    if (m_dialogOwner == DialogOwner::NPC) {
        // Al finalizar el diálogo 1 (primera vez), habilitar libros
        if (!m_npcHablado) {
            m_npcHablado = true;
            m_librosHabilitados = true;
        }
    } else if (m_dialogOwner == DialogOwner::Solar) {
        // Si es el diálogo final del Solar, salimos del aula
        if (m_finalSolar) {
            m_fb.setString("Has restaurado el Logos.\nPuedes salir del aula cuando desees.");
        }
    }

    m_dialogOwner = DialogOwner::None;
    return true;
}

void EdificioKant::interaccionSalida() {
    const sf::FloatRect personaje = m_personaje.obtenerHitbox();
    const sf::FloatRect puerta    = m_areaSalida.getGlobalBounds();

    if (!intersecta(personaje, puerta)) return;

    if (!m_finalSolar) {
        // Aún no se permite salir → rebotar un poquito
        m_personaje.setPosition(m_prevPosJugador.x, m_prevPosJugador.y);
        m_feedback = true;
        m_fb.setString("Termina el aula (habla con Kant, libros y Sistema Solar) para salir.");
        return;
    }

    // Flow completado → salir del aula
    ProgresoJuego::get().marcarCleared(ProgresoJuego::Nivel::Kant);
    gestor->sacarEstado();
    // Reubica al jugador fuera del edificio (ajusta a tu mundo exterior)
    m_personaje.setPosition(840.f, 605.f);
}

// === EdificioKant.cpp ===
// Reemplaza completamente tu cargarColisionesMapa() por esto:
void EdificioKant::cargarColisionesMapa() {
    m_colisiones.clear();
    m_dbgColisiones.clear();

    // Todo relativo al fondo real en pantalla
    const sf::FloatRect gb = m_sprFondo.getGlobalBounds();

    auto W = [&](float x, float y, float w, float h) -> sf::FloatRect {
        return sf::FloatRect({gb.position.x + x, gb.position.y + y}, {w, h});
    };
    auto push = [&](const sf::FloatRect& r){
        m_colisiones.push_back(r);
        sf::RectangleShape s;
        s.setPosition(r.position);
        s.setSize(r.size);
        s.setFillColor(sf::Color(0,255,255,60));
        s.setOutlineColor(sf::Color(0,120,255,200));
        s.setOutlineThickness(1.5f);
        m_dbgColisiones.push_back(s);
    };

    // Dimensión lógica del aula (como en Biblioteca): 1536 × 864
    const float MW = 1536.f, MH = 864.f;

    // --- PAREDES ---
    // Top fino (debajo del vitral) – evita invadir el pasillo superior
    const float WALL_TOP_H = 58.f;
    push(W(0.f, 0.f, MW - 300.f, 210.f));

    // Esquinas (vitrales laterales altos)
    push(W(0.f, 0.f, 118.f, 200.f));
    push(W(MW - 498.f, 0.f, 118.f, 200.f));

    // Laterales (finos, toda la altura útil)
    push(W(0.f, 40.f, 22.f, MH + 100.f));
    push(W(MW - 398.f, 40.f, 22.f, MH + 100.f));

    // Zócalo inferior con hueco central (puerta roja)
    const float BASE_H = 26.f;
    const float DOOR_W = 190.f;                 // coincide con tu m_areaSalida
    const float cx = MW * 0.5f;
    push(W(0.f, MH - BASE_H + 149, cx - DOOR_W * 0.5f, BASE_H));
    push(W(600.f, MH - BASE_H + 149, MW - (cx + DOOR_W * 0.5f), BASE_H));

    // --- MESA/TARIMA DEL PROFE (la amarilla) ---
    // Ajustado para no tapar el pasillo superior (coincide con el sprite de Kant)
    push(W(470.f, 290.f, 200.f, 125.f));

    // --- PUPITRES (3 filas × 4 columnas) ---
    // Tamaño realista mesa+silla sin invadir pasillo central
    const float DESK_W = 109.f, DESK_H = 140.f;
    // Columnas (izq-ext, izq-int, der-int, der-ext) tomando la imagen como referencia
    const float C1 = 88.f, C2 = 278.f, C3 = 753.f, C4 = 943.f;
    // Filas (superior, media, inferior)
    const float R1 = 421.f, R2 = 619.f, R3 = 805.f;

    auto fila = [&](float y){
        push(W(C1, y, DESK_W, DESK_H));
        push(W(C2, y, DESK_W, DESK_H));
        push(W(C3, y, DESK_W, DESK_H));
        push(W(C4, y, DESK_W, DESK_H));
    };
    fila(R1); fila(R2); fila(R3);

    // --- BLOQUES GRANDES SUPERIORES (libros + sistema solar) ---
    // Solo lo que realmente estorba al paso, no una franja ancha
    // Libro Pura (tu sprite está en ~505,420 con scale 0.5) -> caja contenida
    //push(W(495.f, 410.f, 130.f, 120.f));
    // Sistema Solar (foto/frames bajo el pizarrón). Empuja un poco hacia arriba.
    //push(W(840.f, 110.f, 220.f, 110.f));
    // Libro Práctica
    //push(W(1160.f, 410.f, 130.f, 120.f));
}

void EdificioKant::aplicarColisiones() {
    const sf::FloatRect playerNow = m_personaje.obtenerHitbox();
    for (const auto& box : m_colisiones) {
        if (intersecta(playerNow, box)) {
            m_personaje.setPosition(m_prevPosJugador.x, m_prevPosJugador.y);
            return;
        }
    }
}

bool EdificioKant::intersecta(const sf::FloatRect& A, const sf::FloatRect& B) {
    const float ax1 = A.position.x;
    const float ay1 = A.position.y;
    const float ax2 = ax1 + A.size.x;
    const float ay2 = ay1 + A.size.y;

    const float bx1 = B.position.x;
    const float by1 = B.position.y;
    const float bx2 = bx1 + B.size.x;
    const float by2 = by1 + B.size.y;

    return (ax1 < bx2) && (ax2 > bx1) && (ay1 < by2) && (ay2 > by1);
}

EdificioKant::~EdificioKant() {}
