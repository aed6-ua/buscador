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
string s = "catedraTelefonicaUA@iuii.ua.es p1 p2";
a.DelimitadoresPalabra("@.&");
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s = "catedraTelefonicaUA@@iuii.ua.es p1 p2";
a.Tokenizar(s, lt1);
// La lista de tokens a devolver debería contener: "0.34 0,56"
imprimirListaSTL(lt1);

s = "pal1 @iuii.ua.es p1 p2";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="pal1 cat@iuii.ua.es@cd p1 p2";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="pal1 cat@iuii@cd.ua.es p1 p2";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

a.DelimitadoresPalabra("&.");

s="catedraTelefonicaUA@iuii.ua.es p1 p2";
a.Tokenizar(s, lt1);
// La lista de tokens a devolver debería contener: "0.34 0,56"
imprimirListaSTL(lt1);

s="pal1 @iuii.ua.es p1 p2";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="pal1&@iuii.ua.es p1 p2";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="pal1&catedra@iuii.ua.es p1 p2";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="pal1 cat@iuii.ua.es@cd p1 p2";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

a.DelimitadoresPalabra("@.-_");

s="-catedraTelefonicaUA@iuii.ua.es @p1 p2 ";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="@p2@@dot.com@p1-p2";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="a@a- b@- c@c--c d@d-- e@-e f@--";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);
}
