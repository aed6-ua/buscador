#include <iostream> 
#include <string>
#include <list>
#include "buscador.h"
using namespace std;
main() {
IndexadorHash b("./StopWordsIngles.txt", ". ,:", false, false, "./indiceEval", 0, false, false);
b.Indexar("ficherosTimes.txt");
b.GuardarIndexacion();
Buscador a("./indiceEval", 0);
a.Buscar("/home/edu/ei/tokenizador/CorpusTime/Preguntas/", 423, 1, 83);
a.ImprimirResultadoBusqueda(423);
a.ImprimirResultadoBusqueda(423, "fich_salida_buscador_alumno.txt");
}
