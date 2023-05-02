#include <iostream> 
#include <string>
#include <list> 
#include "indexadorHash.h"

using namespace std;

int
main(void)
{
IndexadorHash a("./StopWordsEspanyol.txt", ". ,:", false, false, "./indicePrueba", 0, false, true);

if(a.Indexar("./listaFicheros_corto.txt"))
	cout << "Indexacion terminada" << endl;
else
	cout << "Indexacion NO terminada" << endl;

a.VaciarIndiceDocs();

cout << a.NumPalIndexadas() << endl;

if(a.ListarDocs("corpus_corto/fichero1.txt"))
	cout << "Existe el archivo corpus_corto/fichero1.txt" << endl;
else
	cout << "NO Existe el archivo corpus_corto/fichero1.txt" << endl;


}
