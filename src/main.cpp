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
                cout << (*itCadena) << " ";
        }
        cout << endl;
}

int
main(void)
{
Tokenizador a(",", true, false);
list<string> lt1;
string s = "p0 http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es p1 p2";
a.DelimitadoresPalabra("@.,&");
s = "pal1 10.000,34 10,000.34 10.000.123.456.789.009,34 10,000,123,456,789,009.34 20.03 40,03 2005 10. 20, 10.0 20,0 La 20,12.456,7.8.9,";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s = ".34 ,56";
a.Tokenizar(s, lt1);
// La lista de tokens a devolver debería contener: "0.34 0,56"
imprimirListaSTL(lt1);

s = "pal1 10.35% 10,35% 23.000,3% 23,5$ 23.05$ 23,05$ 11.1$ 11.05$ 3.4% 4,3% 9$ 7% 9,56%@ 9,56% @";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="pal1 10.00a 10.000.a.000 10/12/85 1,23E+10";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="pal1&10.00@10.000&abc@10/12/85";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s=".34@@&,56";
a.Tokenizar(s, lt1);
// La lista de tokens a devolver debería contener: "0.34 0,56"
imprimirListaSTL(lt1);

s="...10.000.a.000 ,,23.05 10/12/85 1,23E+10";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="...10.000.000 ,,23.05 10/12/85 1,23E+10";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="3..2 4,,,,5 ..35";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

a.DelimitadoresPalabra("");

s="...10.000.a.000 ,,23.05 10/12/85 1,23E+10";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);

s="3..2 4,,,,5 ..35";
a.Tokenizar(s, lt1);
imprimirListaSTL(lt1);
}
