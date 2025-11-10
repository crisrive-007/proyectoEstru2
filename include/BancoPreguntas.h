#ifndef BANCOPREGUNTAS_H
#define BANCOPREGUNTAS_H

#pragma once
#include <queue>
#include "Pregunta.h"

namespace BancoPreguntas {
    std::queue<Pregunta> kant();
    std::queue<Pregunta> descartes();
}

#endif // BANCOPREGUNTAS_H
