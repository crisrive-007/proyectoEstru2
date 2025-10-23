#ifndef PREGUNTA_H
#define PREGUNTA_H

#include "Categorias.h"
#include <string>

using namespace std;
class Pregunta
{
    public:
        Pregunta(/*int id, string pregunta, string[] opciones, string respuesta, Categorias categoria*/);
        int id;
        string pregunta;
        string opciones[4];
        string respuesta;
        Categorias categoria;

        virtual ~Pregunta();

    protected:

    private:
};

#endif // PREGUNTA_H
