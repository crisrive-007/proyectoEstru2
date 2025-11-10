#include "EdificioDescartes.h"
#include "GestorEstados.h"
#include "ProgresoJuego.h"

#include <cmath>
#include <sstream>
#include <algorithm>

static inline sf::Vector2f center(const sf::Sprite& s){
    auto r = s.getGlobalBounds();
    return {r.position.x + r.size.x*0.5f, r.position.y + r.size.y*0.5f};
}
static inline float dist(sf::Vector2f a, sf::Vector2f b){
    float dx=a.x-b.x, dy=a.y-b.y; return std::sqrt(dx*dx+dy*dy);
}

EdificioDescartes::EdificioDescartes(GestorEstados* g, sf::RenderWindow& win, Personaje& pj)
: Estado(g, pj), m_window(win), m_personaje(pj), m_sprFondo(m_texFondo), m_sprNPC(m_texNPC),
  m_sprEvi(m_texEvi), m_sprAna(m_texAna), m_sprSin(m_texSin), m_sprEnum(m_texEnum),
  m_txt(m_font,"",20), m_fb(m_font,"",20), m_hint(m_font,"SPACE",16),
  m_opts{ sf::Text(m_font,"",20), sf::Text(m_font,"",20), sf::Text(m_font,"",20), sf::Text(m_font,"",20) }
{
    if (!ProgresoJuego::get().puedeEntrar(ProgresoJuego::Nivel::Descartes)) {
        std::cout << "🔒 Edificio Descartes bloqueado: termina el Gimnasio primero.\n";
        gestor->sacarEstado();
        return;
    }
    // Assets base
    m_texFondo.loadFromFile("assets/EdificioDescartes/lab_descartes.png");
    m_sprFondo.setTexture(m_texFondo,true);

    auto w = m_window.getSize(); auto t = m_texFondo.getSize();
    float s = std::min(float(w.x)/std::max(1u,t.x), float(w.y)/std::max(1u,t.y));
    m_sprFondo.setScale({s,s});
    auto gb = m_sprFondo.getGlobalBounds();
    m_sprFondo.setPosition({(w.x-gb.size.x)/2.f,(w.y-gb.size.y)/2.f});

    m_texNPC.loadFromFile("assets/EdificioDescartes/npc_descartes.png");
    m_sprNPC.setTexture(m_texNPC,true);
    m_sprNPC.setPosition({980.f, 210.f});
    m_sprNPC.setScale({2.f,2.f});

    m_texEvi.loadFromFile("assets/EdificioDescartes/mesa_evidencia.png");
    m_texAna.loadFromFile("assets/EdificioDescartes/mesa_analisis.png");
    m_texSin.loadFromFile("assets/EdificioDescartes/mesa_sintesis.png");
    m_texEnum.loadFromFile("assets/EdificioDescartes/mesa_enumeracion.png");
    m_sprEvi.setTexture(m_texEvi,true);   m_sprEvi.setPosition({800.f,380.f});
    m_sprAna.setTexture(m_texAna,true);   m_sprAna.setPosition({800.f,620.f});
    m_sprSin.setTexture(m_texSin,true);   m_sprSin.setPosition({1050.f,380.f});
    m_sprEnum.setTexture(m_texEnum,true); m_sprEnum.setPosition({1050.f,620.f});
    m_sprEvi.setScale({5.f,5.f}); m_sprAna.setScale({5.f,5.f});
    m_sprSin.setScale({5.f,5.f}); m_sprEnum.setScale({5.f,5.f});

    m_font.openFromFile("assets/Pokemon_GB.ttf");
    m_hint.setFillColor(sf::Color::White);
    m_hint.setOutlineColor(sf::Color::Black);
    m_hint.setOutlineThickness(2.f);

    m_personaje.establecerPosicion({965.f,900.f});
    m_personaje.setScale(2.f,2.f);

    m_areaSalida.setSize({130.f, 32.f});
    m_areaSalida.setPosition({930.f, 980.f}); // ajusta según tu fondo
    m_areaSalida.setFillColor(sf::Color(255, 0, 0, 120));

    // Dilema y objetos de evidencia
    cargarObjetosEvidencia();
    cargarColisionesMapa();
}

void EdificioDescartes::cargarObjetosEvidencia() {
    m_objetos.clear();
    m_objetos.reserve(6);

    m_objetos.emplace_back("Vela encendida",
        "assets/EdificioDescartes/obj_vela.png", true,  sf::Vector2f{320.f, 520.f});
    m_objetos.emplace_back("Balanza",
        "assets/EdificioDescartes/obj_balanza.png",   true,  sf::Vector2f{600.f, 540.f});
    m_objetos.emplace_back("Libro cerrado",
        "assets/EdificioDescartes/obj_libro.png",     true,  sf::Vector2f{820.f, 560.f});

    m_objetos.emplace_back("Holograma de flor",
        "assets/EdificioDescartes/obj_holograma.png", false, sf::Vector2f{1040.f, 520.f});
    m_objetos.emplace_back("Espejismo de agua",
        "assets/EdificioDescartes/obj_espejismo.png", false, sf::Vector2f{1240.f, 540.f});
    m_objetos.emplace_back("Sombra duplicada",
        "assets/EdificioDescartes/obj_sombra.png",    false, sf::Vector2f{1440.f, 560.f});
}

void EdificioDescartes::mostrarDilemaEvidencia() {
    startDialog({
        "Mesa de la EVIDENCIA:",
        "Dilema: Solo acepta lo claro y distinto.",
        "Selecciona 3 objetos fiables (no ilusorios).",
        "Acercate y presiona SPACE para marcar/desmarcar."
    });
}

void EdificioDescartes::generarInferenciasDesdeEvidencia() {
    m_inferencias.clear();
    // Crea inferencias simples a partir de los nombres elegidos
    for (auto& n : m_selEvidencia) {
        if (n == "Vela encendida") m_inferencias.push_back("La luz permite distinguir formas.");
        else if (n == "Balanza")   m_inferencias.push_back("Lo medible ofrece criterios objetivos.");
        else if (n == "Libro cerrado") m_inferencias.push_back("La razon abre lo que los sentidos no muestran.");
    }
    // Si faltó alguno, añade genéricas
    while (m_inferencias.size() < 3)
        m_inferencias.push_back("La mente ordena lo confuso.");
}

void EdificioDescartes::prepararSintesis() {
    m_relacionesOK.clear();
    // emparejar primer objeto con primera inferencia, etc. (hay múltiples válidas; aquí simple)
    size_t n = std::min(m_selEvidencia.size(), m_inferencias.size());
    for (size_t i=0;i<n;i++) m_relacionesOK.push_back({m_selEvidencia[i], m_inferencias[i]});
}

void EdificioDescartes::prepararEnumeracion() {
    // Orden correcto de conceptos cartesianos hacia una conclusion tipo “cogito”
    m_ordenCorrecto = {
        "Duda metódica", "Ideas claras y distintas", "Razonamiento ordenado", "Conclusion: pienso, luego existo"
    };
}

void EdificioDescartes::cargarPreguntasAnalisis() {
    while(!m_cola.empty()) m_cola.pop();
    // Usa inferencias capturadas
    std::string i1 = (m_inferencias.size()>0)? m_inferencias[0] : "La mente ordena lo confuso.";
    std::string i2 = (m_inferencias.size()>1)? m_inferencias[1] : "Lo medible ofrece criterios objetivos.";
    m_cola.push({
        "ANALISIS: divide el problema. Si los sentidos pueden fallar, ¿como avanzas?",
        {"Acepto todo por costumbre","Dudo y busco partes simples","Sigo la mayoria","Me guio por la autoridad"}, 1
    });
    m_cola.push({
        "De tu evidencia has derivado: \""+i1+"\". Eso implica que…",
        {"Solo la experiencia manda","La claridad guia la certeza","Nada puede conocerse","Todo es relativo"}, 1
    });
    m_cola.push({
        "Tambien inferiste: \""+i2+"\". Entonces para analizar conviene…",
        {"Medir y definir terminos","Soñar una respuesta","Ignorar contradicciones","Elegir lo mas rapido"}, 0
    });
}

void EdificioDescartes::cargarPreguntasSintesis() {
    while(!m_cola.empty()) m_cola.pop();
    // Relacionar objeto con inferencia
    if (m_relacionesOK.empty()) m_relacionesOK.push_back({"Vela encendida","La luz permite distinguir formas."});
    auto par = m_relacionesOK[0];
    m_cola.push({
        "SINTESIS: relaciona. ¿Que se vincula mejor con \""+par.second+"\"?",
        {par.first,"Espejismo de agua","Holograma de flor","Sombra duplicada"}, 0
    });
    m_cola.push({
        "Con las relaciones adecuadas, ¿cual frase refleja el paso simple->complejo?",
        {"Claridad -> definiciones -> deducciones","Deducciones al azar","Autoridad -> dogma","Costumbre -> certeza"}, 0
    });
}

void EdificioDescartes::cargarPreguntasEnumeracion() {
    while(!m_cola.empty()) m_cola.pop();
    // Ordenar 4 conceptos. Pedimos la primera clave del orden.
    m_cola.push({
        "ENUMERACION: ¿cual va primero?",
        {"Duda metodica","Ideas claras y distintas","Razonamiento ordenado","Conclusion: pienso, luego existo"}, 0
    });
    m_cola.push({
        "¿cual va segundo?",
        {"Conclusion: pienso, luego existo","Ideas claras y distintas","Razonamiento ordenado","Duda metodica"}, 1
    });
    m_cola.push({
        "¿cual va tercero?",
        {"Razonamiento ordenado","Duda metodica","Conclusion: pienso, luego existo","Ideas claras y distintas"}, 0
    });
    m_cola.push({
        "¿cual va ultimo?",
        {"Razonamiento ordenado","Conclusion: pienso, luego existo","Duda metodica","Ideas claras y distintas"}, 1
    });
}

void EdificioDescartes::siguientePregunta() {
    if (m_cola.empty()) {
        m_mostrarQuiz=false; m_feedback=true;
        if (m_fase==FaseDesc::Analisis){ m_anaOK=true; m_fb.setString("Analisis completado. Ve a la Sintesis. ENTER"); }
        else if (m_fase==FaseDesc::Sintesis){ m_sinOK=true; m_fb.setString("Sintesis lograda. Ve a la Enumeracion. ENTER"); }
        else if (m_fase==FaseDesc::Enumeracion){ m_enumOK=true; m_fb.setString("Conclusion alcanzada. Puedes salir cuando quieras. ENTER"); m_fase=FaseDesc::Fin; }
        return;
    }
    m_actual = m_cola.front(); m_cola.pop();
    m_txt.setString(m_actual.enunciado);
    for (int i=0;i<4;i++) m_opts[i].setString(std::to_string(i+1)+") "+m_actual.opciones[i]);
    m_sel=-1; m_feedback=false; m_mostrarQuiz=true;
}

void EdificioDescartes::startDialog(std::initializer_list<std::string> lines){
    m_dialog.assign(lines.begin(), lines.end());
    m_dIdx=0; m_dialogOn=true; m_feedback=true; m_mostrarQuiz=false;
    m_txt.setString("");
    if (!m_dialog.empty()) m_fb.setString(m_dialog[0]); else { m_dialogOn=false; m_feedback=false; }
}
bool EdificioDescartes::nextDialog(){
    if (!m_dialogOn) return true;
    if (++m_dIdx < m_dialog.size()){ m_fb.setString(m_dialog[m_dIdx]); return false; }
    m_dialogOn=false; m_feedback=false;
    if (!m_npcHablado){ m_npcHablado=true; }
    return true;
}

HitDesc EdificioDescartes::detectarCercania() const{
    auto p = m_personaje.getPosition();
    auto near = [&](const sf::Sprite& s){ return dist(p, center(s)) <= 120.f; };
    if (!m_npcHablado) {
        if (near(m_sprNPC)) return HitDesc::NPC;
        return HitDesc::Ninguno;
    }

    if (near(m_sprNPC))  return HitDesc::NPC;

    // gating por fase/progreso
    if (!m_eviOK && near(m_sprEvi)) return HitDesc::M_Evidencia;
    if ( m_eviOK && !m_anaOK && near(m_sprAna)) return HitDesc::M_Analisis;
    if ( m_anaOK && !m_sinOK && near(m_sprSin)) return HitDesc::M_Sintesis;
    if ( m_sinOK && !m_enumOK && near(m_sprEnum)) return HitDesc::M_Enumeracion;
    return HitDesc::Ninguno;
}

void EdificioDescartes::toggleObjetoCercano(){
    // marcar/desmarcar objeto más cercano al personaje
    float best=1e9f; int idx=-1;
    auto p = m_personaje.getPosition();
    for (int i=0;i<(int)m_objetos.size();++i){
        float d = dist(p, center(m_objetos[i].spr));
        if (d<best){ best=d; idx=i; }
    }
    if (idx<0) return;
    auto& o = m_objetos[idx];
    // regla: solo 3 elegidos, y deben ser reales
    if (!o.elegido){
        if (!o.real){ m_fb.setString("Ilusion. No es claro ni distinto."); m_feedback=true; return; }
        if ((int)m_selEvidencia.size()>=3){ m_fb.setString("Ya elegiste 3 evidencias."); m_feedback=true; return; }
        o.elegido=true; m_selEvidencia.push_back(o.nombre);
        m_fb.setString("Marcado: "+o.nombre); m_feedback=true;
    }else{
        o.elegido=false;
        for (auto it=m_selEvidencia.begin(); it!=m_selEvidencia.end(); ++it)
            if (*it==o.nombre){ m_selEvidencia.erase(it); break; }
        m_fb.setString("Desmarcado: "+o.nombre); m_feedback=true;
    }
}

bool EdificioDescartes::evidenciaResuelta() const{
    if (m_selEvidencia.size()!=3) return false;
    // verificar que todos sean reales
    int ok=0;
    for (auto& n : m_selEvidencia){
        for (auto& o : m_objetos) if (o.nombre==n && o.real) ok++;
    }
    return ok==3;
}

void EdificioDescartes::manejarEventos(sf::RenderWindow& win){
    while (auto ev = win.pollEvent()){
        if (ev->is<sf::Event::Closed>()) { win.close(); continue; }
        if (auto* k=ev->getIf<sf::Event::KeyPressed>()){
            if (k->code==sf::Keyboard::Key::Escape){ gestor->sacarEstado(); return; }

            // avanzar diálogo o feedback
            if (k->code==sf::Keyboard::Key::Enter){
                if (m_dialogOn){ (void)nextDialog(); continue; }
                if (m_feedback && !m_mostrarQuiz){ m_feedback=false; continue; }
            }

            // interacción contextual
            if (k->code==sf::Keyboard::Key::Space){
                if (m_mostrarQuiz) continue;
                if (!m_npcHablado && m_cerca != HitDesc::NPC) {
                    m_feedback = true;
                    m_fb.setString("Habla primero con Descartes.");
                    continue;
                }
                if (m_cerca==HitDesc::NPC){
                    startDialog({
                        "Bienvenido al Laboratorio de la Duda.",
                        "Regla 1: Evidencia. 2: Analisis. 3: Sintesis. 4: Enumeracion.",
                        "Empieza eligiendo 3 objetos fiables en la Evidencia."
                    });
                    continue;
                }

                if (m_cerca==HitDesc::M_Evidencia){
                    if (!m_eviOK){
                        if (!m_evidenciaActiva){
                            m_evidenciaActiva = true;
                            m_needLayoutEvi = true; // <--- NUEVO: layout pendiente
                            startDialog({
                                "Mesa de la EVIDENCIA activada.",
                                "Dilema: Acepta solo lo claro y distinto.",
                                "Selecciona 3 objetos reales con CLICK."
                            });
                        } else {
                            // Ya no alternamos con SPACE; ahora la seleccion es por CLICK
                            // (nada aquí)
                        }
                    } else {
                        m_feedback = true;
                        m_fb.setString("La Evidencia ya fue resuelta.");
                    }
                    continue;
                }

                if (m_cerca==HitDesc::M_Analisis && m_eviOK && !m_anaOK){
                    m_fase=FaseDesc::Analisis;
                    cargarPreguntasAnalisis();
                    siguientePregunta();
                    continue;
                }
                if (m_cerca==HitDesc::M_Sintesis && m_anaOK && !m_sinOK){
                    m_fase=FaseDesc::Sintesis;
                    prepararSintesis();
                    cargarPreguntasSintesis();
                    siguientePregunta();
                    continue;
                }
                if (m_cerca==HitDesc::M_Enumeracion && m_sinOK && !m_enumOK){
                    m_fase=FaseDesc::Enumeracion;
                    prepararEnumeracion();
                    cargarPreguntasEnumeracion();
                    siguientePregunta();
                    continue;
                }
            }

            // selección 1..4 en quiz y ENTER para siguiente
            if (m_mostrarQuiz){
                if (!m_feedback){
                    if (k->code==sf::Keyboard::Key::Num1) m_sel=0;
                    if (k->code==sf::Keyboard::Key::Num2) m_sel=1;
                    if (k->code==sf::Keyboard::Key::Num3) m_sel=2;
                    if (k->code==sf::Keyboard::Key::Num4) m_sel=3;
                    if (m_sel!=-1){
                        m_feedback=true;
                        m_fb.setString(m_sel==m_actual.correcta? "Correcto." : "Incorrecto.");
                    }
                } else if (k->code==sf::Keyboard::Key::Enter) {
                    siguientePregunta();
                }
            }
        }

        if (auto* m = ev->getIf<sf::Event::MouseButtonPressed>()){
            if (m->button == sf::Mouse::Button::Left && m_evidenciaActiva && !m_mostrarQuiz){
                sf::Vector2f mp = sf::Vector2f((float)m->position.x, (float)m->position.y);

                // Solo procesar clicks dentro del panel
                if (m_eviPanelRect.contains(mp)){
                    // Buscar si clickeo algun objeto
                    for (auto& o : m_objetos){
                        if (o.spr.getGlobalBounds().contains(mp)){
                            // Reglas: solo reales y max 3
                            if (!o.elegido){
                                if (!o.real){
                                    m_fb.setString("Ilusion. No es claro ni distinto.");
                                    m_feedback = true;
                                    break;
                                }
                                if ((int)m_selEvidencia.size()>=3){
                                    m_fb.setString("Ya elegiste 3 evidencias.");
                                    m_feedback = true;
                                    break;
                                }
                                o.elegido = true;
                                m_selEvidencia.push_back(o.nombre);
                                m_fb.setString("Marcado: " + o.nombre);
                                m_feedback = true;
                            } else {
                                // Desmarcar
                                o.elegido = false;
                                for (auto it=m_selEvidencia.begin(); it!=m_selEvidencia.end(); ++it){
                                    if (*it == o.nombre){ m_selEvidencia.erase(it); break; }
                                }
                                m_fb.setString("Desmarcado: " + o.nombre);
                                m_feedback = true;
                            }

                            // ¿Se completó la evidencia?
                            if (evidenciaResuelta()){
                                m_eviOK = true;
                                m_evidenciaActiva = false;   // cerrar panel
                                generarInferenciasDesdeEvidencia();
                                m_fase = FaseDesc::Analisis;
                                startDialog({
                                    "Evidencia completada.",
                                    "Ahora divide: Analisis de tus hallazgos."
                                });
                            }
                            break; // tras procesar un objeto, salir
                        }
                    }
                }
            }
        }
    }
}

void EdificioDescartes::actualizar(){
    // Posición previa ANTES de mover
    m_prevPosJugador = m_personaje.getPosition();

    if (!m_mostrarQuiz && !m_dialogOn)
        m_personaje.actualizarSinTiles(m_window.getSize().x, m_window.getSize().y);

    // Clamp al área visible del laboratorio
    sf::FloatRect gb = m_sprFondo.getGlobalBounds();
    {
        sf::FloatRect hb = m_personaje.obtenerHitbox();
        sf::Vector2f p = m_personaje.getPosition();
        const float minX = gb.position.x;
        const float minY = gb.position.y;
        const float maxX = gb.position.x + gb.size.x - hb.size.x;
        const float maxY = gb.position.y + gb.size.y - hb.size.y;
        p.x = std::clamp(p.x, minX, maxX);
        p.y = std::clamp(p.y, minY, maxY);
        m_personaje.setPosition(
            (int)std::lround(p.x),
            (int)std::lround(p.y)
        );
    }

    aplicarColisiones();             // ← NUEVO
    m_cerca = detectarCercania();    // (lo tuyo)
    m_hintBob += 3.f*(1.f/60.f);
    interaccionSalida();             // (ya la tienes)
}

void EdificioDescartes::dibujar(sf::RenderWindow& w){
    w.draw(m_sprFondo);

    // Mesas y resto
    w.draw(m_sprEvi); w.draw(m_sprAna); w.draw(m_sprSin); w.draw(m_sprEnum);
    w.draw(m_sprNPC);
    m_personaje.dibujar(w);

    if (m_debugColisiones){
        for (auto& r : m_dbgColisiones) w.draw(r);
        for (auto& n : m_dbgNumeros) w.draw(n);
    }

    if (m_evidenciaActiva){
        // 1) Dibuja panel estilo dialogo y obtiene rect
        auto r = dibujarPanelEvidencia(w);

        // 2) Layout grid (3 columnas x 2 filas)
        const float pad = 28.f;
        const float cellW = (r.size.x - pad*2) / 3.f;
        const float cellH = (r.size.y - (pad*2 + 36.f)) / 2.f; // 36 para el titulo
        const float baseX = r.position.x + pad;
        const float baseY = r.position.y + pad + 36.f;

        if (m_needLayoutEvi){
            // Calcula y guarda posiciones de sprites (centro de cada celda)
            m_eviGridPos.clear();
            m_eviGridPos.reserve(m_objetos.size());
            for (int i=0;i<(int)m_objetos.size();++i){
                int col = i % 3;
                int row = i / 3;
                float cx = baseX + col*cellW + cellW*0.5f;
                float cy = baseY + row*cellH + cellH*0.5f;
                m_eviGridPos.push_back({cx, cy});
            }
            // Escalar sprites para caber en celda (si tus PNG son grandes, ajusta 0.4~0.8)
            for (auto& o : m_objetos){
                o.spr.setScale({0.1f, 0.1f});
            }
            m_needLayoutEvi = false;
        }

        // 3) Posiciona y dibuja
        for (int i=0;i<(int)m_objetos.size();++i){
            auto& o = m_objetos[i];
            // centrar sprite en la celda
            auto gb = o.spr.getGlobalBounds();
            sf::Vector2f pos = m_eviGridPos[i] - sf::Vector2f(gb.size.x*0.5f, gb.size.y*0.5f);
            o.spr.setPosition(pos);
            // color de seleccionado
            o.spr.setColor(o.elegido ? sf::Color(200,255,200,255) : sf::Color::White);
            w.draw(o.spr);

            // etiqueta (nombre)
            sf::Text etiqueta(m_font, o.nombre, 16);
            etiqueta.setFillColor(sf::Color(220,220,220));
            auto tb = etiqueta.getGlobalBounds();
            etiqueta.setPosition({m_eviGridPos[i].x - tb.size.x*0.5f, pos.y + gb.size.y + 6.f});
            w.draw(etiqueta);
        }
    }

    // hint “SPACE”
    if (!m_mostrarQuiz && !m_dialogOn){
        sf::Sprite const* target=nullptr;
        if (m_cerca==HitDesc::NPC) target=&m_sprNPC;
        else if (m_cerca==HitDesc::M_Evidencia && !m_eviOK) target=&m_sprEvi;
        else if (m_cerca==HitDesc::M_Analisis  && m_eviOK && !m_anaOK) target=&m_sprAna;
        else if (m_cerca==HitDesc::M_Sintesis  && m_anaOK && !m_sinOK) target=&m_sprSin;
        else if (m_cerca==HitDesc::M_Enumeracion && m_sinOK && !m_enumOK) target=&m_sprEnum;
        if (target){
            auto b = target->getGlobalBounds();
            float cx=b.position.x+b.size.x*0.5f; float cy=b.position.y-18.f+std::sin(m_hintBob)*4.f;
            m_hint.setPosition({cx-m_hint.getGlobalBounds().size.x*0.5f, cy});
            w.draw(m_hint);
        }
    }

    if (m_mostrarQuiz){
        auto r = dibujarCajaDialogo(w);
        float x=r.position.x+16.f, y=r.position.y+16.f;
        m_txt.setPosition({x,y}); m_txt.setFillColor(sf::Color::White); w.draw(m_txt);
        y+=44.f;
        for (int i=0;i<4;i++){
            m_opts[i].setPosition({x,y});
            if (m_feedback){
                if (i==m_actual.correcta) m_opts[i].setFillColor(sf::Color(60,200,60));
                else if (i==m_sel)       m_opts[i].setFillColor(sf::Color(220,60,60));
                else                     m_opts[i].setFillColor(sf::Color::White);
            } else m_opts[i].setFillColor(sf::Color::White);
            w.draw(m_opts[i]); y+=28.f;
        }
        if (m_feedback){
            m_fb.setFillColor(sf::Color(230,230,230));
            m_fb.setPosition({x, r.position.y + r.size.y - 34.f});
            w.draw(m_fb);
        }
    } else if (m_dialogOn || m_feedback){
        auto r = dibujarCajaDialogo(w);
        float x=r.position.x+16.f, y=r.position.y+16.f;
        m_fb.setFillColor(sf::Color::White);
        m_fb.setPosition({x,y}); w.draw(m_fb);
        // Mostrar selección actual de evidencia como recordatorio
        if (!m_eviOK){
            std::stringstream ss; ss<<"Seleccionados: ";
            for (size_t i=0;i<m_selEvidencia.size();++i){ if (i) ss<<", "; ss<<m_selEvidencia[i]; }
            sf::Text info(m_font, ss.str(), 18);
            info.setFillColor(sf::Color(200,200,200));
            info.setPosition({x, y+120.f}); w.draw(info);
        }
    }

    w.draw(m_areaSalida);
}

sf::FloatRect EdificioDescartes::dibujarCajaDialogo(sf::RenderWindow& w){
    const float W = static_cast<float>(w.getSize().x);
    const float H = static_cast<float>(w.getSize().y);
    const float h = 200.f;
    const float x = 20.f;
    const float y = H - h - 20.f;

    sf::RectangleShape box({W - 40.f, h});
    box.setPosition({x, y});
    box.setFillColor(sf::Color(0,0,0,190));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(255,255,255,40));
    w.draw(box);

    return sf::FloatRect({x, y}, {W - 40.f, h});
}

sf::FloatRect EdificioDescartes::dibujarPanelEvidencia(sf::RenderWindow& w){
    const float W = (float)w.getSize().x;
    const float H = (float)w.getSize().y;
    const float pad = 20.f;
    const float h  = 280.f;              // panel mas alto que el dialogo normal
    const float y  = H - h - 40.f;       // un poco mas arriba
    const float x  = 40.f;
    const float wbox = W - 80.f;

    sf::RectangleShape box({wbox, h});
    box.setPosition({x, y});
    box.setFillColor(sf::Color(0,0,0,190));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(255,255,255,40));
    w.draw(box);

    // Titulo del panel
    sf::Text titulo(m_font, "Selecciona 3 evidencias (clic)", 18);
    titulo.setFillColor(sf::Color::White);
    titulo.setPosition({x + pad, y + pad});
    w.draw(titulo);

    m_eviPanelRect = sf::FloatRect({x, y}, {wbox, h});
    return m_eviPanelRect;
}

bool EdificioDescartes::intersecta(const sf::FloatRect& A, const sf::FloatRect& B) {
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

void EdificioDescartes::interaccionSalida() {
    const sf::FloatRect personaje = m_personaje.obtenerHitbox();
    const sf::FloatRect salida = m_areaSalida.getGlobalBounds();

    if (intersecta(personaje, salida)) {
        std::cout << "🚪 Saliendo del laboratorio de Descartes.\n";
        ProgresoJuego::get().marcarCleared(ProgresoJuego::Nivel::Descartes);
        gestor->sacarEstado();
        m_personaje.setPosition(950.f, 300.f); // posición al regresar al mapa
    }
}

void EdificioDescartes::cargarColisionesMapa() {
    m_colisiones.clear();
    m_dbgColisiones.clear();

    // Fondo real en pantalla
    const sf::FloatRect gb = m_sprFondo.getGlobalBounds();

    // Tamaño lógico del arte
    const float LW = 1536.f, LH = 864.f;
    const float fx = gb.size.x / LW;
    const float fy = gb.size.y / LH;

    auto T = [&](float x, float y, float w, float h) -> sf::FloatRect {
        return sf::FloatRect(
            { gb.position.x + x * fx, gb.position.y + y * fy },
            { w * fx, h * fy }
        );
    };
    auto push = [&](const sf::FloatRect& r){
        m_colisiones.push_back(r);
        sf::RectangleShape s; s.setPosition(r.position); s.setSize(r.size);
        s.setFillColor(sf::Color(0,255,255,60));
        s.setOutlineColor(sf::Color(0,120,255,200));
        s.setOutlineThickness(1.5f);
        m_dbgColisiones.push_back(s);
    };

    // ===== PAREDES (finitas, sin cubrir el aula) =====
    push(T(0,   0,    LW, 150));                // superior
    push(T(0,  32,    112, LH));             // lateral izq
    push(T(LW-126, 32, 112, LH));             // lateral der

    // Zócalo inferior con hueco de puerta (centro)
    const float baseH = 22, doorW = 210;
    const float cx = LW * 0.5f;
    push(T(0, LH-baseH, cx - doorW*0.5f+40, baseH));
    push(T(cx + doorW*0.5f +40, LH-baseH, LW - (cx + doorW*0.5f), baseH));

    // ===== BLOQUES GRANDES (arriba/laterales) =====
    // Librero alto izq
    push(T(104, 72, 320, 92));
    // Escritorio PC arriba-centro
    push(T(560, 76, 228, 92));
    // Mesas carpetas arriba-dcha
    push(T(904, 76, 328, 92));
    // Libreros medio-izq (dos módulos)
    push(T(0, 340, 460, 194));
    // Escritorio con taza medio-dcha
    push(T(1065, 220, 220, 110));
    // Archiveros + mesita abajo-dcha
    push(T(1200, 604, 238, 186));
    // Librero/mesita abajo-izq
    push(T(112, 600, 238, 205));

    // ===== MÁQUINA CIRCULAR (solo su “base” real) =====
    // Más pequeña para que puedas rodearla
    push(T(1180, 400, 225, 130));

    // ===== MESAS AMARILLAS (3 columnas) =====
    // Columnas más angostas y con “respiro” arriba/abajo
    const float colW = 92;
    // Izquierda
    push(T(640, 330, colW + 25, 200));              // tramo superior
    push(T(640, 540, colW + 25, 200));              // tramo inferior
    // Centro
    //push(T(785, 330, colW, 170));
    //push(T(785, 560, colW, 170));
    // Derecha
    push(T(915, 330, colW + 25, 200));
    push(T(915, 540, colW + 25, 200));

    // ===== SILLAS VERDES (solo las que bloquean) =====
    // Deja pasillos; no colisionamos todas.
    push(T(520, 400, 100, 70));  // izq-sup
    push(T(520, 610, 100, 70));  // izq-inf
    //push(T(740, 420, 54, 54));  // centro-sup izq
    //push(T(830, 660, 54, 54));  // centro-inf der
    push(T(1035,400, 100, 70));  // der-sup
    push(T(1035,610, 100, 70));  // der-inf

    // >>> IMPORTANTE: NO hay ningún rectángulo grande “de piso”.
    // Solo piezas; así los pasillos (entre columnas y hacia la puerta) quedan libres.
}

void EdificioDescartes::aplicarColisiones() {
    const sf::FloatRect player = m_personaje.obtenerHitbox();
    for (const auto& box : m_colisiones){
        if (EdificioDescartes::intersecta(player, box)){  // usa tu helper
            m_personaje.setPosition(
                (int)std::lround(m_prevPosJugador.x),
                (int)std::lround(m_prevPosJugador.y)
            );
            return;
        }
    }
}
