#include <iostream> 
#include <string>
#include "buscador.h"
#include "indexadorHash.h"
using namespace std;
main() {
IndexadorHash b("./StopWordsEspanyol.txt", ". ,:", false, false, 
"./indicePrueba", 0, false, false);
b.Indexar("./listaFicheros_corto.txt");
b.GuardarIndexacion();
Buscador a("./indicePrueba", 0);
cout << "Buscador: " << a;
cout << a.DevolverTipoStemming () << endl;
// Estaría accediendo al método de la clase ?IndexadorHash? de la 
//que ?Buscador? está heredando
a.IndexarPregunta("documentos sobre la Guerra Civil española");
if(a.Buscar(20))
a.ImprimirResultadoBusqueda(10);
if(a.CambiarFormulaSimilitud(1))
{
if(a.Buscar(20))
a.ImprimirResultadoBusqueda(10);
}
}