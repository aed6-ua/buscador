#include <iostream> 
#include <string>
#include <list> 
#include "indexadorHash.h"
#include "indexadorInformacion.h"

using namespace std;

int
main(void)
{
IndexadorHash a("./StopWordsEspanyol.txt", ". ,:", false, false, "./indicePrueba", 0, false, true);

string preg1;
InformacionPregunta infPreg1;

a.IndexarPregunta("pal1 yo pal2 pal1. pal3 el  ");

if(a.DevuelvePregunta(preg1))
	cout << "ESTA INDEXADA LA PREGUNTA: " << preg1 << endl;
else
	cout << "NO HAY INDEXADA NINGUNA PREGUNTA" << endl;

a.VaciarIndicePreg();
 
if(a.DevuelvePregunta(preg1))
	cout << "ESTA INDEXADA LA PREGUNTA: " << preg1 << endl;
else
	cout << "NO HAY INDEXADA NINGUNA PREGUNTA" << endl;


if(a.DevuelvePregunta(infPreg1))
        cout << "ESTA INDEXADA LA PREGUNTA: " << infPreg1 << endl;
else
        cout << "NO HAY INDEXADA NINGUNA PREGUNTA" << endl;

}
