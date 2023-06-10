#include <iostream>
#include <string>
#include <list>
#include <sys/resource.h>
#include "buscador.h"
using namespace std;
double getcputime(void)
{
        struct timeval tim;
        struct rusage ru;
        getrusage(RUSAGE_SELF, &ru);
        tim = ru.ru_utime;
        double t = (double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
        tim = ru.ru_stime;
        t += (double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
        return t;
}
main()
{
        Buscador a("./CorpusTime/Vectores_documentos", 3, true);
        std::cout << "Buscador creado" << endl;
        // Imprimir cuantos embeddings hay indexados
        std::cout << "Hay " << a.embeddings.size() << " embeddings indexados" << endl;

        a.Buscar_vector("/home/edu/ei/tokenizador/CorpusTime/Vectores_preguntas/", 423, 1, 83);
        a.ImprimirResultadoBusqueda(423);
        a.ImprimirResultadoBusqueda(423, "fich_salida_buscador_alumno.txt");
        
        /*
        Tokenizador a(",;:.-/+*\\ '\"{}[]()<>?!??&#=\t\n\r@", false, true);
        std::cout << "Tokenizador creado" << endl;
        a.TokenizarDirectorio("/home/edu/ei/tokenizador/CorpusTime/Preguntas/");
        std::cout << "Directorio tokenizado" << endl;
        */
}