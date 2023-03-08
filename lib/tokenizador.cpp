#include "../include/tokenizador.h"
#include <sys/stat.h>

using namespace std;
// Inicializa delimiters a delimitadoresPalabra filtrando que no se
// introduzcan delimitadores repetidos(de izquierda a derecha, en cuyo
// caso se eliminarían los que hayan sido repetidos por la derecha);
// casosEspeciales a kcasosEspeciales;
// pasarAminuscSinAcentos a minuscSinAcentos
Tokenizador::Tokenizador(const string &delimitadoresPalabra, const bool &kcasosEspeciales, const bool &minuscSinAcentos)
{
    delimiters = delimitadoresPalabra;
    delimitersSet = unordered_set<char>(delimiters.begin(), delimiters.end());
    casosEspeciales = kcasosEspeciales;
    pasarAminuscSinAcentos = minuscSinAcentos;
    string::size_type pos = 0;
    while (pos < delimiters.size())
    {
        string::size_type pos2 = delimiters.find(delimiters[pos], pos + 1);
        if (pos2 != string::npos)
        {
            delimiters.erase(pos2, 1);
        }
        else
        {
            pos++;
        }
    }
}

// Constructor de copia
Tokenizador::Tokenizador(const Tokenizador &t)
{
    delimiters = t.delimiters;
    delimitersSet = t.delimitersSet;
    casosEspeciales = t.casosEspeciales;
    pasarAminuscSinAcentos = t.pasarAminuscSinAcentos;
}

// Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@";
// casosEspeciales a true;
// pasarAminuscSinAcentos a false
Tokenizador::Tokenizador()
{
    delimiters = ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@";
    delimitersSet = unordered_set<char>(delimiters.begin(), delimiters.end());
    casosEspeciales = true;
    pasarAminuscSinAcentos = false;
}

// Pone delimiters=""
Tokenizador::~Tokenizador()
{
    delimiters = "";
}

// Copia
Tokenizador &Tokenizador::operator=(const Tokenizador &t)
{
    if (this == &t)
        return *this;
    delimiters = t.delimiters;
    delimitersSet = t.delimitersSet;
    casosEspeciales = t.casosEspeciales;
    pasarAminuscSinAcentos = t.pasarAminuscSinAcentos;
    return *this;
}

// Tokeniza str devolviendo el resultado en tokens. La lista tokens se
// vaciará antes de almacenar el resultado de la tokenización.
void Tokenizador::Tokenizar(string &str, list<string> &tokens)
{
    // Limpiar la lista de tokens
    tokens.clear();
    if (casosEspeciales)
        TokenizarEspecialesEstados(str, tokens);
    else
        {
        string::size_type lastPos = efficient_find_first_not_of(str, 0);
        string::size_type pos = efficient_find_first_of(str, lastPos);
        while (string::npos != pos || string::npos != lastPos)
        {
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            lastPos = efficient_find_first_not_of(str, pos);
            pos = efficient_find_first_of(str, lastPos);
        }
    }
}

// Método auxiliar para tokenizar teniendo en cuenta los casos especiales
void Tokenizador::TokenizarEspeciales(string &str, list<string> &tokens) const
{
    // Si los delimitadores no contiene espacios, se añaden
    string delimitadores = delimiters;
    if (delimiters.find(" ") == string::npos)
    {
        delimitadores += " ";
    }
    // Si los delimitadores no contiene el salto de línea, se añade
    if (delimiters.find("\n") == string::npos)
    {
        delimitadores += "\n";
    }

    // Se detectarán casos especiales en el siguiente orden:
    // 1. URL: delimitadores especiales: “:/.?&-=#@“
    string URLdelimiters = ":/.?&-=#@";
    // 2. Números decimales: delimitadores especiales: “.,“
    string decimaldelimiters = ".,";
    // 3. E-mail: delimitadores especiales: “.-_@“
    string emaildelimiters = ".-_@";
    // 4. Acronimos: delimitadores especiales: “.“
    string acronimodelimiters = ".";
    // 5. Palabras con guiones: delimitadores especiales: “-“
    string guiondelimiters = "-";

    // Se activara el caso especial si se detecta un delimitador especial perteneciente al caso especial
    string::size_type lastPos = efficient_find_first_not_of(str, 0);
    string::size_type pos = efficient_find_first_of(str, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        
        // URL
        if (URLdelimiters.find(str[pos]) != string::npos)
        {
            // Comprobar si es una URL (que comience por “http:” o “https:” o “ftp:” seguido de algún carácter que no sea delimitador)
            if (str.substr(lastPos, 5) == "http:" || str.substr(lastPos, 6) == "https:" || str.substr(lastPos, 4) == "ftp:")
            {
                // Comprobar que va seguido de un carácter que no sea delimitador
                pos = str.find(":", lastPos)+1;
                if (delimitadores.find(str[pos]) == string::npos || URLdelimiters.find(str[pos]) != string::npos)
                {
                    bool salir = false;
                    // Buscar el siguiente delimitador que no sea de URL pero que sí sea de los delimitadores
                    while (!salir)
                    {
                        pos = efficient_find_first_of(str, pos);
                        if (URLdelimiters.find(str[pos]) == string::npos)
                        {
                            salir = true;
                        }
                        else
                        {
                            pos++;
                        }
                    }
                }
            }

        }

        // Números decimales
        if (delimitersSet.count('.') && delimitersSet.count(','))
        {
            size_t lastPosAux = lastPos;
            size_t posAux = pos;
            // Comprobar si el "." o "," aparecen al principio del token
            if (decimaldelimiters.find(str[lastPos-1]) != string::npos) {
                // Si pos es un "." o ",", se sigue buscando el siguiente caracter que no sea un "." o "," o un digito numérico
                string perc_dollar = "%$";
                while (!(str[posAux] == ' ' || 
                (decimaldelimiters.find(str[posAux]) != string::npos && (str[posAux+1] == ' ')) ||
                ((perc_dollar.find(str[posAux]) != string::npos ) && (str[posAux+1] == ' ')) ||
                delimitersSet.count(str[posAux]))) {
                    
                    for (size_t i = pos; i < str.size(); i++) {
                        if (this->delimitersSet.count(str[i])>0 || perc_dollar.find(str[i]) != string::npos) {
                            posAux = i;
                            break;
                        }
                    }
                }
            }
            // Comprobar si todos los caracteres entre lastPos y pos son dígitos numéricos o "." o ","
            bool esNumeroDecimal = true;
            for (size_t i = lastPosAux; i < posAux; i++) {
                if (!isdigit(str[i]) && decimaldelimiters.find(str[i]) == string::npos) {
                    esNumeroDecimal = false;
                    break;
                }
            }
            // Si es un número decimal, se comprueba si hay un "." o "," antes del primer dígito numérico
            // Si lo hay se inserta un "0"
            if (esNumeroDecimal) {
                if (decimaldelimiters.find(str[lastPosAux-1]) != string::npos) {
                    str.insert(lastPosAux, "0");
                    posAux++;
                    lastPosAux-=2;
                }
            }
                
        }

        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = efficient_find_first_not_of(str, pos);
        pos = efficient_find_first_of(str, lastPos);
    }
}

void Tokenizador::TokenizarEspecialesEstados(const string &str, list<string> &tokens) {
    
    // Añadir el espacio al set de delimitadores
    delimitersSet.insert(' ');

    ESTADO estado = INICIO;
    char c;
    size_t start;
    bool todo_digitos = true;
    
    // Si no es un delimitador especial, se añade el token a la lista de tokens

    // Se detectarán casos especiales en el siguiente orden:
    // 1. URL: delimitadores especiales: “:/.?&-=#@“
    string URLdelimiters_string = ":/.?&-=#@";
    unordered_set<char> URLdelimiters = unordered_set<char>(URLdelimiters_string.begin(), URLdelimiters_string.end());
    // 2. Números decimales: delimitadores especiales: “.,“
    string decimaldelimiters_string = ".,";
    unordered_set<char> decimaldelimiters = unordered_set<char>(decimaldelimiters_string.begin(), decimaldelimiters_string.end());
    // 3. E-mail: delimitadores especiales: “.-_@“
    string emaildelimiters_string = ".-_@";
    unordered_set<char> emaildelimiters = unordered_set<char>(emaildelimiters_string.begin(), emaildelimiters_string.end());
    // 4. Acronimos: delimitadores especiales: “.“
    string acronimodelimiters_string = ".";
    unordered_set<char> acronimodelimiters = unordered_set<char>(acronimodelimiters_string.begin(), acronimodelimiters_string.end());
    // 5. Palabras con guiones: delimitadores especiales: “-“
    string guiondelimiters_string = "-";
    unordered_set<char> guiondelimiters = unordered_set<char>(guiondelimiters_string.begin(), guiondelimiters_string.end());

    // Recorrer la cadena a tokenizar de izquierda a derecha hasta encontrar un delimitador o un espacio
    for (size_t i = 0; i < str.size(); i++) {
        c = str[i];
        // Comprobar si c es un digito numerico
        if (delimitersSet.count(c) == 0 && !isdigit(c))
            todo_digitos = false;
        if (estado == TOKEN && delimitersSet.count(c) > 0) {
            // Comprobar si es un delimitador especial
            if (URLdelimiters.count(c) > 0 && (str.substr(start, 5) == "http:" || str.substr(start, 6) == "https:" || str.substr(start, 4) == "ftp:"))
                estado = URL;
            else if (decimaldelimiters.count(c) > 0 && todo_digitos)
                estado = DECIMAL;
            else if (emaildelimiters.count(c) > 0)
                estado = EMAIL;
            else if (acronimodelimiters.count(c) > 0)
                estado = ACRONIMO;
            else if (guiondelimiters.count(c) > 0)
                estado = GUION;
            else {
                tokens.push_back(str.substr(start, i-start));
                estado = INICIO;
            }
        }
        switch (estado)
        {
        case INICIO:
            if (delimitersSet.count(c) == 0) {
                start = i;
                if (!isdigit(c))
                    todo_digitos = false;
                else
                    todo_digitos = true;
                if (decimaldelimiters.count(str[start-1]) > 0 && isdigit(c))
                    estado = DECIMAL;
                else
                    estado = TOKEN;
            }
            break;
        
        case URL:
            // Comprobar si empieza por "http:" o "https:" o "ftp:" seguido de algun caracter que no sea delimitador
            if (str.substr(start, 5) == "http:" || str.substr(start, 6) == "https:" || str.substr(start, 4) == "ftp:") {
                // Comprobar que va seguido de un carácter que no sea delimitador
                size_t aux = str.find(":", start)+1;
                if (delimitersSet.count(str[aux]) == 0 || URLdelimiters.count(str[aux]) > 0)
                {
                    bool salir = false;
                    // Buscar el siguiente delimitador que no sea de URL pero que sí sea de los delimitadores
                    i = aux;
                    while (!salir)
                    {
                        i = efficient_find_first_of(str, i);
                        if (i==-1) {
                            salir = true;
                            i = str.size();
                        }
                        else if (URLdelimiters.count(str[i]) == 0)
                        {
                            salir = true;
                        }
                        else
                        {
                            i++;
                        }
                    }
                    tokens.push_back(str.substr(start, i-start));
                    estado = INICIO;
                    break;
                }
            }
            else {
                // Como no es una url, se añade el token a la lista de tokens
                tokens.push_back(str.substr(start, i-start));
                estado = INICIO;
                break;
            }
        case DECIMAL:
            // Comprobar que de momento en el token solo hay digitos numericos
            if (todo_digitos) {
                bool punto_inicial = false;
                if (decimaldelimiters.count(str[start - 1]) > 0) {
                    // Si hay un "." o "," antes del primer dígito numérico, se inserta un "0"
                    punto_inicial = true;
                    start--;
                }
                while (i <= str.size()) {
                    
                    // Detectar el final del numero por un espacio; "." o "," seguido de espacio; 
                    // o los símbolos "%" y "$" seguidos de espacio (los cuales se almacenarían en un término posterior, 
                    // aunque hubiesen sido definidos como delimitadores); o delimitador
                    if ((isdigit(str[i-1]) && (str[i] == ' ' || str[i] == '\0')) || 
                        (decimaldelimiters.count(str[i]) > 0 && (str[i+1] == ' ' || str[i+1] == '\0')) || 
                        ((str[i] == '%' || str[i] == '$') && (str[i+1] == ' ' || str[i+1] == '\0')) || 
                        (delimitersSet.count(str[i]) > 0 && decimaldelimiters.count(str[i]) == 0)) {
                        if (punto_inicial)
                            tokens.push_back("0" + str.substr(start, i-start));
                        else
                            tokens.push_back(str.substr(start, i-start));
                        if (str[i] == '%' || str[i] == '$'){
                            tokens.push_back(str.substr(i, 1));
                        }
                        estado = INICIO;
                        break;
                    }
                    if ((str[i] == '%' || str[i] == '$') && (str[i+1] != ' ' && str[i+1] != '\0')) {
                        // Si lo que sigue al "%$" no es un espacio o el final, no es un decimal
                        dividir_por_puntos_y_comas(str, tokens, punto_inicial, start, i);
                        estado = INICIO;
                        break;
                    }
                    // Si no es un digito numerico no era un numero decimal
                    if (!isdigit(str[i]) && decimaldelimiters.count(str[i]) == 0) {
                        i--;
                        estado = TOKEN;
                        break;
                    }
                    if (decimaldelimiters.count(str[i]) > 0 && decimaldelimiters.count(str[i+1]) > 0) {
                        // Si hay dos "." o "," seguidos no es un decimal
                        tokens.push_back(str.substr(start, i-start));
                        estado = INICIO;
                        break;
                    }
                    i++;
                }
            }
        }
    }
    if (estado == TOKEN)
    {
        tokens.push_back(str.substr(start, str.size()-start));
    }
}

void Tokenizador::dividir_por_puntos_y_comas(const string &str, list<string> &tokens, const bool &punto_inicial, size_t &start, size_t &i) const {
    if (punto_inicial)
        start++;
    size_t pos = efficient_find_first_of(str, start);
    while (pos < i && pos != string::npos) {
        tokens.push_back(str.substr(start, pos-start));
        start = pos+1;
        pos = efficient_find_first_of(str, start);
    }
    if (delimitersSet.count(str[i]) != 0)
        tokens.push_back(str.substr(start, i-start));
}

// Tokeniza el fichero i guardando la salida en el fichero f (una
// palabra en cada línea del fichero). Devolverá true si se realiza la
// tokenización de forma correcta;
// false en caso contrario enviando a cerr
// el mensaje
// correspondiente(p.ej.que no exista el archivo i)
bool Tokenizador::Tokenizar(const string &NomFichEntr, const string &NomFichSal)
{
    ifstream i;
    ofstream f;
    string cadena;
    list<string> tokens;
    i.open(NomFichEntr.c_str());
    if (!i)
    {
        cerr << "ERROR: No existe el archivo: " << NomFichEntr << endl;
        return false;
    }
    else
    {
        while (!i.eof())
        {
            cadena = "";
            getline(i, cadena);
            if (cadena.length() != 0)
            {
                Tokenizar(cadena, tokens);
            }
        }
    }
    i.close();
    f.open(NomFichSal.c_str());
    list<string>::iterator itS;
    for (itS = tokens.begin(); itS != tokens.end(); itS++)
    {
        f << (*itS) << endl;
    }
    f.close();
    return true;
}

// Tokeniza el fichero i guardando la salida en un fichero de nombre i
// añadiéndole extensión.tk(sin eliminar previamente la extensión de i
// por ejemplo,
// del archivo pp.txt se generaría el resultado en pp.txt.tk),
// y que contendrá una palabra en cada línea del fichero.Devolverá true si
// se realiza la tokenización de forma correcta;
// false en caso contrario enviando a cerr el mensaje correspondiente(p.ej.que no exista el
// archivo i)

bool Tokenizador::Tokenizar(const string &NomFichEntr)
{
    ifstream i;
    ofstream f;
    string cadena;
    list<string> tokens;
    i.open(NomFichEntr.c_str());
    if (!i)
    {
        cerr << "ERROR: No existe el archivo: " << NomFichEntr << endl;
        return false;
    }
    else
    {
        while (!i.eof())
        {
            cadena = "";
            getline(i, cadena);
            if (cadena.length() != 0)
            {
                Tokenizar(cadena, tokens);
            }
        }
    }
    i.close();
    f.open((NomFichEntr + ".tk").c_str());
    list<string>::iterator itS;
    for (itS = tokens.begin(); itS != tokens.end(); itS++)
    {
        f << (*itS) << endl;
    }
    f.close();
    return true;
}

// Tokeniza el fichero i que contiene un nombre de fichero por línea
// guardando la salida en ficheros(uno por cada línea de i) cuyo nombre
// será el leído en i añadiéndole extensión.tk,
// y que contendrá una
// palabra en cada línea del fichero leído en i.Devolverá true si se
// realiza la tokenización de forma correcta de todos los archivos que
// contiene i;
// devolverá false en caso contrario enviando a cerr el mensaje
// correspondiente(p.ej.que no exista el archivo i, o que se trate de un directorio, 
// enviando a “cerr” los archivos de i que no existan o que sean directorios; luego no se ha de interrumpir la ejecución si hay
// algún archivo en i que no exista)

bool Tokenizador::TokenizarListaFicheros(const string &NomFichEntr)
{
    ifstream i;
    ofstream f;
    string cadena;
    list<string> tokens;
    i.open(NomFichEntr.c_str());
    if (!i)
    {
        cerr << "ERROR: No existe el archivo: " << NomFichEntr << endl;
        return false;
    }
    else
    {
        cadena = "";
        while (!i.eof())
        {
            getline(i, cadena);
            if (cadena.length() != 0)
            {
                Tokenizar(cadena);
            }
        }
    }
    i.close();
    return true;
}

// Tokeniza todos los archivos que contenga el directorio i, incluyendo
// los de los subdirectorios, guardando la salida en ficheros cuyo nombre será el de entrada añadiéndole extensión.tk, 
// y que contendrá una palabra en cada línea del fichero.Devolverá true si se realiza la tokenización de forma correcta 
// de todos los archivos;
// devolverá false en
// caso contrario enviando a cerr el mensaje
// correspondiente(p.ej.que no
// exista el directorio i,
// o los ficheros que no se hayan podido tokenizar)

bool Tokenizador::TokenizarDirectorio(const string &dirAIndexar)
{
    struct stat dir;
    // Compruebo la existencia del directorio
    int err = stat(dirAIndexar.c_str(), &dir);
    if (err == -1 || !S_ISDIR(dir.st_mode))
        return false;
    else
    {
        // Hago una lista en un fichero con find>fich
        string cmd = "find " + dirAIndexar + " -follow |sort > .lista_fich";
        system(cmd.c_str());
        return TokenizarListaFicheros(".lista_fich");
    }
}

// Inicializa delimiters a nuevoDelimiters, filtrando que no se
// introduzcan delimitadores repetidos(de izquierda a derecha, en cuyo
// caso se eliminarían los que hayan sido repetidos por la derecha)
void Tokenizador::DelimitadoresPalabra(const string &nuevoDelimiters) {
    delimiters = nuevoDelimiters;
    string::size_type pos = 0;
    while (pos < delimiters.size())
    {
        string::size_type pos2 = delimiters.find(delimiters[pos], pos + 1);
        if (pos2 != string::npos)
        {
            delimiters.erase(pos2, 1);
        }
        else
        {
            pos++;
        }
    }
    // Borrar delimitersSet
    delimitersSet.clear();
    delimitersSet = unordered_set<char>(delimiters.begin(), delimiters.end());
}

// Añade al final de “delimiters” los nuevos delimitadores que aparezcan
// en “nuevoDelimiters”(no se almacenarán caracteres repetidos)
void Tokenizador::AnyadirDelimitadoresPalabra(const string &nuevoDelimiters) {    
    string delimiters = nuevoDelimiters;
    string delimiters2 = "";
    for (int i = 0; i < delimiters.length(); i++) {
        if (delimiters2.find(delimiters[i]) == string::npos) {
            delimiters2 += delimiters[i];
        }
    }
    delimitersSet.clear();
    delimitersSet = unordered_set<char>(delimiters.begin(), delimiters.end());
}

// Devuelve “delimiters”
string Tokenizador::DelimitadoresPalabra() const {
    return delimiters;
}

// Cambia la variable privada “casosEspeciales”
void Tokenizador::CasosEspeciales(const bool &nuevoCasosEspeciales) {
    casosEspeciales = nuevoCasosEspeciales;
}

// Devuelve el contenido de la variable privada “casosEspeciales”
bool Tokenizador::CasosEspeciales() {
    return casosEspeciales;
}

// Cambia la variable privada “pasarAminuscSinAcentos”. Atención al
// formato de codificación del corpus(comando “file” de Linux).Para la corrección de la práctica 
// se utilizará el formato actual(ISO - 8859).
void Tokenizador::PasarAminuscSinAcentos(const bool &nuevoPasarAminuscSinAcentos) {
    pasarAminuscSinAcentos = nuevoPasarAminuscSinAcentos;
}

// Devuelve el contenido de la variable privada “pasarAminuscSinAcentos”
bool Tokenizador::PasarAminuscSinAcentos() {
    return pasarAminuscSinAcentos;
}

size_t Tokenizador::efficient_find_first_of(const string &str, const size_t &pos) const {
    for (size_t i = pos; i < str.size(); i++) {
        if (this->delimitersSet.count(str[i])>0) {
            return i;
        }
    }
    return string::npos;
}

size_t Tokenizador::efficient_find_first_not_of(const string &str, const size_t &pos) const {
    for (size_t i = pos; i < str.size(); i++) {
        if (this->delimitersSet.count(str[i])==0) {
            return i;
        }
    }
    return string::npos;
}