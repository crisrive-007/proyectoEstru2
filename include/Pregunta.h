#ifndef PREGUNTA_H
#define PREGUNTA_H

struct Pregunta {
    std::string enunciado;
    std::array<std::string,4> opciones;
    int correcta = 0;
};

#endif // PREGUNTA_H
