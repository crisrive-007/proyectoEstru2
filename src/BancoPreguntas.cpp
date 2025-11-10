#include "BancoPreguntas.h"

std::queue<Pregunta> BancoPreguntas::kant() {
    std::queue<Pregunta> cola;

    cola.push({"Seleccione el mandato cuya obligacion viene del miedo al castigo o la busqueda de un premio:",
              {"Imperativo hipotetico","Imperativo categorico","Ambos","Ninguno"}, 0});

    cola.push({"Para Emanuel Kant, es posible conocer lo que las cosas nos permiten (como lo superficial) a traves de nuestros sentidos:",
              {"Conocimiento noumenico","Conocimiento fenomenico","Conocimiento empirico","Conocimiento racional"}, 1});

    cola.push({"Kant decia que el lema de la ilustracion era “Sapere aude”, que significa:",
              {"Sopesa tus acciones","Atrevete a saber por ti mismo","Saber a la fuerza","Someterse al conocimiento"}, 1});

    cola.push({"Kant (igual que Copernico cambio el centro del universo de la tierra al sol), cambia el centro del conocimiento del objeto al sujeto, a esto se le llama:",
              {"Subjetivismo","Prejuicio","Giro copernicano","Suerte"}, 2});

    cola.push({"La postura conciliadora de Kant respecto a los empiristas y racionalistas define que los datos experimentales son la fuente del conocimiento racional del sujeto:",
              {"Racionalismo","Empirismo","Criticismo","Escepticismo"}, 2});

    cola.push({"De las siguientes obras de Emanuel Kant, seleccione aquella que define su epistemologia:",
              {"Critica de la razon practica","Critica de la razon pura","Critica del juicio","Critica fenomenologica"}, 1});

    return cola;
}

std::queue<Pregunta> BancoPreguntas::descartes() {
    std::queue<Pregunta> cola;

    cola.push({"Son los pasos o razones de la duda metodica:",
              {"Los sentidos nos engañan","La existencia del genio maligno","Imposibilidad para diferenciar la vigilia del sueño","Todas son correctas"}, 3});

    cola.push({"Una de las siguientes es considerada de las cuatro reglas del método en Descartes:",
              {"Hipotesis","Deduccion","Evidencia","Induccion"}, 2});

    cola.push({"En relacion con los datos experimentales que otorgan los sentidos en el conocimiento, Descartes define como:",
              {"Los sentidos son una fuente confiable para conocer","Desconfianza de lo que los sentidos nos proporcionan","Los sentidos son complementarios","Los sentidos son determinantes"}, 1});

    cola.push({"Solo debemos aceptar como verdadero aquel conocimiento que sea EVIDENTE, CLARO Y DISTINTO:",
              {"Rene Descartes","David Hume","George Berkeley","Aristoteles"}, 0});

    cola.push({"En cuanto a la certeza del conocimiento, Rene Descartes afirma lo siguiente:",
              {"Es preciso dudar","Debemos confiar ciegamente","Nada es importante","Todo es posible"}, 0});

    return cola;
}







