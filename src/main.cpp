#include <iostream> 
#include <string>
#include <list> 
#include "tokenizador.h"

using namespace std;

void imprimirListaSTL(const list<string>& cadena)
{
        cout << "---------------------------------------------------------------\n";      
        list<string>::const_iterator itCadena;
        for(itCadena=cadena.begin();itCadena!=cadena.end();itCadena++)
        {
                cout << (*itCadena) << ", ";
        }
        cout << endl;
}

int
main(void)
{
Tokenizador a(",", true, false);
list<string> lt1;
string s = "U.S.A p1 e.g. p2. La";
a.DelimitadoresPalabra("@.&");
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s = "U..S.A p1 e..g. p2. La";
a.Tokenizar(s, lt1);
// La lista de tokens a devolver debería contener: "0.34 0,56"
imprimirListaSTL(lt1);

s = "U.S....A.BC.D ";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="...U.S.A p1 e..g. p2. La";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="...U.S.A... p1 e..g. p2. La";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="...U.S.A@p1 e..g-p2. La";
a.Tokenizar(s, lt1);
// La lista de tokens a devolver debería contener: "0.34 0,56"
imprimirListaSTL(lt1);

s="Hack.4.Good p1 ";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

a.DelimitadoresPalabra("");

s="U.S.A .U.S.A .p1 p1 e.g. p2. La";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="U..S.A p1 e..g. p2. La";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="...U.S.A p1 e..g. p2. La";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="a&U.S.A p1 e.g. p2. La";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

a.DelimitadoresPalabra("&");

s="a&U.S.A p1 e.g. p2. La";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);
}
