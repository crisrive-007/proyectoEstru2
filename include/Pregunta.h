#ifndef PREGUNTA_H
#define PREGUNTA_H

#include <iostream>
#include <string>
#include <array>

struct Pregunta {
    std::string enunciado;
    std::array<std::string,4> opciones;
    int correcta = 0;
};

#endif // PREGUNTA_H
