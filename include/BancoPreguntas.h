#ifndef BANCOPREGUNTAS_H
#define BANCOPREGUNTAS_H

enum Categorias {
    Arte,
    Politica,
    Historia,
    Ciencia
};

struct Pregunta {
    string pregunta;
    string opciones[4];
    string respuesta;
    Categorias categoria;
};

class BancoPreguntas
{
    public:
        std::vector<Pregunta> arte;
        std::vector<Pregunta> politica;
        std::vector<Pregunta> ciencia;
        std::vector<Pregunta> historia;

        BancoPreguntas() {
            arte.push_back({"Uno de los siguientes personajes fue el encargado de pintar la capilla Sixtina:", {"Miguel Ángel", "Donatello", "Leonardo Da Vinci", "Francis Bacon"}, "Miguel Ángel", Categoria::Arte});
            arte.push_back({"Genio del renacimiento que esculpió el Moisés, el David y la Pietá:", {"Miguel Ángel Buonarroti", "Leonardo Da Vinci", "Rafael Sanzio", "Galileo Galilei"}, "Miguel Ángel Buonarroti", Categoria::Arte});
            arte.push_back({"Durante el renacimiento el estilo artístico que impregnó el arte, la filosofía, la pintura escritura fue el:", {"El Gótico", "El Barroco", "El Clasicismo", "El Romanticismo"}, "El Barroco", Categoria::Arte});
            arte.push_back({"Durante el renacimiento surge una nueva visión del hombre, que se vio reflejada en el arte, en la política y en las ciencias sociales y humanas, a lo que se denomina:", {"Antropocentrismo", "Humanismo", "Paradigma antropológico", "Teocentrismo"}, "Humanismo", Categorias::Arte});
            arte.push_back({"Cuatro genios del renacimiento (Leonardo, Donatello, Rafael y Michelangelo) han sido llevados a la pantalla en los comics de:", {"Las Tortugas Ninjas", "Los Caballeros del Zodiaco", "Los Cuatro Fantásticos", "Los antagonistas de Attack on Titan"}, "Las Tortugas Ninjas", Categorias::Arte});

            politica.push_back({"Durante el renacimiento, el modelo de gobierno es uno de los siguientes:", {"Monarquía absoluta", "Tiranía republicana", "Democracia participativa", "Liberalismo político"}, "Monarquía absoluta", Categorias::Politica});
            politica.push_back({"De los siguientes acontecimientos, selecciones el que inicia el período moderno:", {"Toma de Constantinopla", "Tratado de paz de Westfalia", "Toma de la Bastilla", "La ruta de la seda"}, "Tratado de paz de Westfalia", Categorias::Politica});
            politica.push_back({"Durante el siglo XV, la sociedad se estratifica en tres estamentos definidos:", {"Clase media, baja y alta", "Nobleza, clero y estado llano", "Artesanos, guardianes y gobernantes", "Burguesía, campesinado y aristocracia"}, "Nobleza, clero y estado llano", Categorias::Politica});
            politica.push_back({"Aparece el realismo político, que se basaba en un orden establecido, explicación de un sistema y recomendaciones de como gobernar:", {"Tomás Moro", "Jean Bodín", "Nicolas Maquiavelo", "Erasmo de Rotterdam"}, "Nicolas Maquiavelo", Categorias::Politica});
            politica.push_back({"Terminada la edad media, en el contexto de la política resulta que:", {"La Iglesia resalta su poder", "La Iglesia pierde el papel rector en la política", "La Iglesia evangélica se posiciona en la política", "La política desaparece"}, "La Iglesia pierde el papel rector en la política", Categorias::Politica});

            ciencia.push_back({"Entre los siguientes renacentistas seleccione, uno de los precursores filósofo-científico del heliocentrismo (teoría que afirma que el sol es el centro del universo):", {"Tomas Moro", "Galileo", "Platón", "Arquimedes"}, "Galileo", Categorias::Ciencia});
            ciencia.push_back({"El método científico se introduce por el interés de tres filósofos. Entre los siguientes uno de los mencionados no es precursor del método científico:", {"Francis Bacon", "Galileo Galilei", "Nicolas Maquiavelo", "René Descartes"}, "Nicolas Maquiavelo", Categorias::Ciencia});
            ciencia.push_back({"Es uno de los precursores del pensamiento Moderno:", {"Isaac Newton", "René Descartes", "Erasmo de Roterdam", "Francis Bacon"}, "René Descartes", Categorias::Ciencia});
            ciencia.push_back({"De los siguientes filósofos niega el geocentrismo (teoría que afirma que el centro de nuestro sistema solar es la tierra):", {"Aristóteles", "Nicolás Copérnico", "Tomás de Aquino", "Isaac Newton"}, "Nicolás Copérnico", Categorias::Ciencia});
            ciencia.push_back({"Uno de los inventos que suscitó un conocimiento ilimitado, fue el de Gutenberg:", {"El astrolabio", "La imprenta", "La nao y la carabela", "El telescopio"}, "La imprenta", Categorias::Ciencia});

            historia.push_back({"Después del feudalismo medieval acudimos al surgimiento de una nueva clase social conocida como la:", {"La monarquía", "El mercantilismo", "La burguesía", "El proletariado"}, "La burguesía", Categorias::Historia});
            historia.push_back({"El renacimiento supone una época de absolutismo y nacionalismos, como el nacimiento de fuertes monarquías europeas centralizadas como:", {"Grecia", "Inglaterra", "Yugoslavia", "Egipto"}, "Inglaterra", Categorias::Historia});
            historia.push_back({"Antes de la consolidación del estado moderno, Italia estuvo divida en pequeñas ciudades-estado normalmente enfrentadas entre si, como es el caso de:", {"Florencia-Napoli", "Ámsterdam-Cracovia", "Reims-Colonia", "Milán-Lourdes"}, "Florencia-Napoli", Categorias::Historia});
            historia.push_back({"La toma de Constantinopla supone un bloqueo comercial entre Europa y Asia (la ruta de la seda) y ocurrió en lo que hoy es actualmente:", {"Eslovaquia", "Estambul en Turquía", "Mesopotamia", "Jerusalén"}, "Estambul en Turquía", Categorias::Historia});
            historia.push_back({"Resurge el interés por Grecia y Roma, junto al declive del sistema feudal, el crecimiento del comercio e innovaciones entre las que mencionamos:", {"La imprenta y la brújula", "La rueda y la escritura", "Las máquinas de vapor y la producción en masa", "La pólvora y la rueda"}, "La imprenta y la brújula", Categorias::Historia});
        }
        virtual ~BancoPreguntas() {}

    protected:

    private:
};

#endif // BANCOPREGUNTAS_H
