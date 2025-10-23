#include "Ruleta.h"

Ruleta::Ruleta() : anguloActual(0), velocidad(0), girando(false), resultado(0) {
    opciones = {"OPCION 1", "OPCION 2", "OPCION 3", "OPCION 4"};
    colores = {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow};
    // Ángulos precisos para cada sección (en el sentido contrario a las agujas del reloj)
    // Asumiendo que la ruleta tiene 4 secciones de 90 grados cada una
    angulosSecciones = {0.0f, 90.0f, 180.0f, 270.0f};
}

bool Ruleta::cargarRecursos() {
    // Cargar textura de la ruleta
    if (!textureRuleta.loadFromFile("assets/spinwheel.png")) {
        std::cout << "Error cargando ruleta.png" << std::endl;
        std::cout << "Asegurate de que el archivo este en la carpeta correcta" << std::endl;
        return false;
    }

    // Crear textura para la flecha
    if (!textureFlecha.loadFromFile("assets/arrow.png")) {
        return false;
    }

    // Cargar fuente
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cout << "Error cargando arial.ttf" << std::endl;
        return false;
    }

    // Inicializar sprites y texto usando emplace
    spriteRuleta.emplace(textureRuleta);
    spriteRuleta->setOrigin(sf::Vector2f(textureRuleta.getSize().x / 2.0f, textureRuleta.getSize().y / 2.0f));
    spriteRuleta->setPosition(sf::Vector2f(400, 300));
    spriteRuleta->setScale(sf::Vector2f(0.5f, 0.5f));

    flecha.emplace(textureFlecha);
    // Origen en la punta de la flecha para mejor precisión
    flecha->setOrigin(sf::Vector2f(textureFlecha.getSize().x / 2.0f, textureFlecha.getSize().y * 0.9f));
    flecha->setPosition(sf::Vector2f(400, 150));
    flecha->setScale(sf::Vector2f(0.1f, 0.1f));

    textoResultado.emplace(font, "Presiona ESPACIO para girar", 24);
    textoResultado->setFillColor(sf::Color::White);
    textoResultado->setPosition(sf::Vector2f(250, 500));

    return true;
}

void Ruleta::girar() {
    if (!girando) {
        girando = true;
        // Velocidad inicial más controlada
        velocidad = 15.0f + (std::rand() % 10);

        // Calcular rotaciones completas aleatorias (entre 3 y 6 vueltas completas)
        int vueltasCompletas = 3 + (std::rand() % 4);

        // Seleccionar una sección aleatoria como destino
        int seccionObjetivo = std::rand() % 4;

        // Calcular el ángulo objetivo exacto
        anguloObjetivo = vueltasCompletas * 360.0f + angulosSecciones[seccionObjetivo];

        if (textoResultado) {
            textoResultado->setString("¡Girando!");
        }
    }
}

void Ruleta::actualizar() {
    if (girando) {
        // Calcular la distancia restante al objetivo
        float distanciaRestante = anguloObjetivo - anguloActual;

        // Desaceleración suave usando interpolación
        if (distanciaRestante > 180.0f) {
            // Fase inicial: velocidad constante
            anguloActual += velocidad;
        } else if (distanciaRestante > 90.0f) {
            // Fase de desaceleración gradual
            velocidad *= 0.98f;
            anguloActual += velocidad;
        } else if (distanciaRestante > 5.0f) {
            // Fase de desaceleración fuerte
            velocidad *= 0.95f;
            if (velocidad < 0.5f) velocidad = 0.5f;
            anguloActual += velocidad;
        } else {
            // Fase final: ajuste preciso
            anguloActual += distanciaRestante * 0.1f;

            // Detener cuando esté muy cerca del objetivo
            if (std::abs(distanciaRestante) < 0.1f) {
                anguloActual = anguloObjetivo;
                velocidad = 0;
                girando = false;
                calcularResultado();
            }
        }

        if (spriteRuleta) {
            spriteRuleta->setRotation(sf::degrees(anguloActual));
        }
    }
}

void Ruleta::calcularResultado() {
    // Normalizar ángulo entre 0 y 360
    float anguloNormalizado = std::fmod(anguloActual, 360.0f);
    if (anguloNormalizado < 0) anguloNormalizado += 360.0f;

    // Determinar la sección basada en el ángulo normalizado
    // Ajustar según la orientación de tu imagen de ruleta
    // La flecha apunta hacia arriba, por lo que calculamos desde el ángulo 0 (derecha)

    // Cada opción ocupa 90 grados (360/4)
    // Agregamos un offset de 45 grados para centrar en las secciones
    float anguloAjustado = std::fmod(360.0f - anguloNormalizado + 45.0f, 360.0f);
    resultado = static_cast<int>(anguloAjustado / 90.0f) % 4;

    if (textoResultado) {
        textoResultado->setString("Resultado: " + opciones[resultado]);
    }
}

void Ruleta::dibujar(sf::RenderWindow& ventana) {
    if (spriteRuleta) {
        ventana.draw(*spriteRuleta);
    }
    if (flecha) {
        ventana.draw(*flecha);
    }
    if (textoResultado) {
        ventana.draw(*textoResultado);
    }

    // Dibujar información de las opciones
    sf::Text textoOpcion(font, "", 18);

    for (int i = 0; i < 4; i++) {
        textoOpcion.setString(opciones[i]);
        textoOpcion.setFillColor(colores[i]);
        textoOpcion.setPosition(sf::Vector2f(650.0f, 200.0f + i * 30.0f));
        ventana.draw(textoOpcion);
    }
}

bool Ruleta::estaGirando() const {
    return girando;
}

int Ruleta::getResultado() const {
    return resultado;
}

Ruleta::~Ruleta() {
    //dtor
}
