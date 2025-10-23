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
            arte.push_back({"Uno de los siguientes personajes fue el encargado de pintar la capilla Sixtina:", {"Miguel �ngel", "Donatello", "Leonardo Da Vinci", "Francis Bacon"}, "Miguel �ngel", Categoria::Arte});
            arte.push_back({"Genio del renacimiento que esculpi� el Mois�s, el David y la Piet�:", {"Miguel �ngel Buonarroti", "Leonardo Da Vinci", "Rafael Sanzio", "Galileo Galilei"}, "Miguel �ngel Buonarroti", Categoria::Arte});
            arte.push_back({"Durante el renacimiento el estilo art�stico que impregn� el arte, la filosof�a, la pintura escritura fue el:", {"El G�tico", "El Barroco", "El Clasicismo", "El Romanticismo"}, "El Barroco", Categoria::Arte});
            arte.push_back({"Durante el renacimiento surge una nueva visi�n del hombre, que se vio reflejada en el arte, en la pol�tica y en las ciencias sociales y humanas, a lo que se denomina:", {"Antropocentrismo", "Humanismo", "Paradigma antropol�gico", "Teocentrismo"}, "Humanismo", Categorias::Arte});
            arte.push_back({"Cuatro genios del renacimiento (Leonardo, Donatello, Rafael y Michelangelo) han sido llevados a la pantalla en los comics de:", {"Las Tortugas Ninjas", "Los Caballeros del Zodiaco", "Los Cuatro Fant�sticos", "Los antagonistas de Attack on Titan"}, "Las Tortugas Ninjas", Categorias::Arte});

            politica.push_back({"Durante el renacimiento, el modelo de gobierno es uno de los siguientes:", {"Monarqu�a absoluta", "Tiran�a republicana", "Democracia participativa", "Liberalismo pol�tico"}, "Monarqu�a absoluta", Categorias::Politica});
            politica.push_back({"De los siguientes acontecimientos, selecciones el que inicia el per�odo moderno:", {"Toma de Constantinopla", "Tratado de paz de Westfalia", "Toma de la Bastilla", "La ruta de la seda"}, "Tratado de paz de Westfalia", Categorias::Politica});
            politica.push_back({"Durante el siglo XV, la sociedad se estratifica en tres estamentos definidos:", {"Clase media, baja y alta", "Nobleza, clero y estado llano", "Artesanos, guardianes y gobernantes", "Burgues�a, campesinado y aristocracia"}, "Nobleza, clero y estado llano", Categorias::Politica});
            politica.push_back({"Aparece el realismo pol�tico, que se basaba en un orden establecido, explicaci�n de un sistema y recomendaciones de como gobernar:", {"Tom�s Moro", "Jean Bod�n", "Nicolas Maquiavelo", "Erasmo de Rotterdam"}, "Nicolas Maquiavelo", Categorias::Politica});
            politica.push_back({"Terminada la edad media, en el contexto de la pol�tica resulta que:", {"La Iglesia resalta su poder", "La Iglesia pierde el papel rector en la pol�tica", "La Iglesia evang�lica se posiciona en la pol�tica", "La pol�tica desaparece"}, "La Iglesia pierde el papel rector en la pol�tica", Categorias::Politica});

            ciencia.push_back({"Entre los siguientes renacentistas seleccione, uno de los precursores fil�sofo-cient�fico del heliocentrismo (teor�a que afirma que el sol es el centro del universo):", {"Tomas Moro", "Galileo", "Plat�n", "Arquimedes"}, "Galileo", Categorias::Ciencia});
            ciencia.push_back({"El m�todo cient�fico se introduce por el inter�s de tres fil�sofos. Entre los siguientes uno de los mencionados no es precursor del m�todo cient�fico:", {"Francis Bacon", "Galileo Galilei", "Nicolas Maquiavelo", "Ren� Descartes"}, "Nicolas Maquiavelo", Categorias::Ciencia});
            ciencia.push_back({"Es uno de los precursores del pensamiento Moderno:", {"Isaac Newton", "Ren� Descartes", "Erasmo de Roterdam", "Francis Bacon"}, "Ren� Descartes", Categorias::Ciencia});
            ciencia.push_back({"De los siguientes fil�sofos niega el geocentrismo (teor�a que afirma que el centro de nuestro sistema solar es la tierra):", {"Arist�teles", "Nicol�s Cop�rnico", "Tom�s de Aquino", "Isaac Newton"}, "Nicol�s Cop�rnico", Categorias::Ciencia});
            ciencia.push_back({"Uno de los inventos que suscit� un conocimiento ilimitado, fue el de Gutenberg:", {"El astrolabio", "La imprenta", "La nao y la carabela", "El telescopio"}, "La imprenta", Categorias::Ciencia});

            historia.push_back({"Despu�s del feudalismo medieval acudimos al surgimiento de una nueva clase social conocida como la:", {"La monarqu�a", "El mercantilismo", "La burgues�a", "El proletariado"}, "La burgues�a", Categorias::Historia});
            historia.push_back({"El renacimiento supone una �poca de absolutismo y nacionalismos, como el nacimiento de fuertes monarqu�as europeas centralizadas como:", {"Grecia", "Inglaterra", "Yugoslavia", "Egipto"}, "Inglaterra", Categorias::Historia});
            historia.push_back({"Antes de la consolidaci�n del estado moderno, Italia estuvo divida en peque�as ciudades-estado normalmente enfrentadas entre si, como es el caso de:", {"Florencia-Napoli", "�msterdam-Cracovia", "Reims-Colonia", "Mil�n-Lourdes"}, "Florencia-Napoli", Categorias::Historia});
            historia.push_back({"La toma de Constantinopla supone un bloqueo comercial entre Europa y Asia (la ruta de la seda) y ocurri� en lo que hoy es actualmente:", {"Eslovaquia", "Estambul en Turqu�a", "Mesopotamia", "Jerusal�n"}, "Estambul en Turqu�a", Categorias::Historia});
            historia.push_back({"Resurge el inter�s por Grecia y Roma, junto al declive del sistema feudal, el crecimiento del comercio e innovaciones entre las que mencionamos:", {"La imprenta y la br�jula", "La rueda y la escritura", "Las m�quinas de vapor y la producci�n en masa", "La p�lvora y la rueda"}, "La imprenta y la br�jula", Categorias::Historia});
        }
        virtual ~BancoPreguntas() {}

    protected:

    private:
};

#endif // BANCOPREGUNTAS_H
