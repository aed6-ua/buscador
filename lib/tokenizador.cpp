#include "tokenizador.h"
#include <sys/stat.h>
#include <cmath>

using namespace std;
Tokenizador::Tokenizador(const string &delimitadoresPalabra, const bool &kcasosEspeciales, const bool &minuscSinAcentos)
{
    string URLdelimiters_string = ":/.?&-=#@"; // delimiters for URLs
    for (unsigned char c : URLdelimiters_string) // loop through each character in s
        URLdelimiters[c] = 1; // set the bit for c to 1
    string decimaldelimiters_string = ".,"; // delimiters for decimals
    for (unsigned char c : decimaldelimiters_string) // loop through each character in s
        decimaldelimiters[c] = 1; // set the bit for c to 1
    string emaildelimiters_string = ".-_@"; // delimiters for emails
    for (unsigned char c : emaildelimiters_string) // loop through each character in s
        emaildelimiters[c] = 1; // set the bit for c to 1
    delimiters = delimitadoresPalabra;
    //delimitersSet = unordered_set<char>(delimiters.begin(), delimiters.end());
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
    delimitersBitset.reset();
    for (unsigned char c : delimiters) // loop through each character in s
        delimitersBitset[c] = 1; // set the bit for c to 1
}

// Constructor de copia
Tokenizador::Tokenizador(const Tokenizador &t)
{
    delimiters = t.delimiters;
    //delimitersSet = t.delimitersSet;
    delimitersBitset = t.delimitersBitset;
    casosEspeciales = t.casosEspeciales;
    pasarAminuscSinAcentos = t.pasarAminuscSinAcentos;
}

Tokenizador::Tokenizador()
{
    string URLdelimiters_string = ":/.?&-=#@"; // delimiters for URLs
    for (unsigned char c : URLdelimiters_string) // loop through each character in s
        URLdelimiters[c] = 1; // set the bit for c to 1
    string decimaldelimiters_string = ".,"; // delimiters for decimals
    for (unsigned char c : decimaldelimiters_string) // loop through each character in s
        decimaldelimiters[c] = 1; // set the bit for c to 1
    string emaildelimiters_string = ".-_@"; // delimiters for emails
    for (unsigned char c : emaildelimiters_string) // loop through each character in s
        emaildelimiters[c] = 1; // set the bit for c to 1
    delimiters = ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@";
    //delimitersSet = unordered_set<char>(delimiters.begin(), delimiters.end());
    for (unsigned char c : delimiters) // loop through each character in s
        delimitersBitset[c] = 1; // set the bit for c to 1
    casosEspeciales = true;
    pasarAminuscSinAcentos = false;
}

// Pone delimiters=""
Tokenizador::~Tokenizador()
{
    delimiters = "";
    //delimitersSet.clear();
    delimitersBitset.reset();
}

// Copia
Tokenizador &Tokenizador::operator=(const Tokenizador &t)
{
    if (this == &t)
        return *this;
    delimiters = t.delimiters;
    //delimitersSet = t.delimitersSet;
    delimitersBitset = t.delimitersBitset;
    casosEspeciales = t.casosEspeciales;
    pasarAminuscSinAcentos = t.pasarAminuscSinAcentos;
    return *this;
}

void Tokenizador::Tokenizar(const string &str, list<string> &tokens)
{
    // Limpiar la lista de tokens
    tokens.clear();
    if (casosEspeciales)
        if (pasarAminuscSinAcentos)
            TokenizarEspeciales(pasar_a_minusculas_sin_acentos(str), tokens);
        else
            TokenizarEspeciales(str, tokens);
    else
        if (pasarAminuscSinAcentos)
            TokenizarSimple(pasar_a_minusculas_sin_acentos(str), tokens);
        else
            TokenizarSimple(str, tokens);
}

void Tokenizador::TokenizarSimple(const string &str, list<string> &tokens)
{
    // Reseteamos el bitset de delimiters porque en TokenizarEspeciales se puede haber modificado
    delimitersBitset.reset();
    for (unsigned char c : delimiters) // loop through each character in s
        delimitersBitset[c] = 1; // set the bit for c to 1
    std::string token; // temporary string to store token
    for (unsigned char c : str) { // loop through each character in str
        if (delimitersBitset[c]) { // if c is a delimiter
            if (!token.empty()) { // if token is not empty
                tokens.push_back(token); // add token to vector
                token.clear(); // clear token
            }
        } else { // if c is not a delimiter
            token += c; // append c to token
        }
    }
    if (!token.empty()) { // if token is not empty after loop
        tokens.push_back(token); // add last token to vector
    }
}

/*
// M?todo auxiliar para tokenizar teniendo en cuenta los casos especiales
void Tokenizador::TokenizarEspeciales(const string &str, list<string> &tokens) const
{
    // Si los delimitadores no contiene espacios, se a?aden
    string delimitadores = delimiters;
    if (delimiters.find(" ") == string::npos)
    {
        delimitadores += " ";
    }
    // Si los delimitadores no contiene el salto de l?nea, se a?ade
    if (delimiters.find("\n") == string::npos)
    {
        delimitadores += "\n";
    }

    // Se detectar?n casos especiales en el siguiente orden:
    // 1. URL: delimitadores especiales: ?:/.?&-=#@?
    string URLdelimiters = ":/.?&-=#@";
    // 2. N?meros decimales: delimitadores especiales: ?.,?
    string decimaldelimiters = ".,";
    // 3. E-mail: delimitadores especiales: ?.-_@?
    string emaildelimiters = ".-_@";
    // 4. Acronimos: delimitadores especiales: ?.?
    string acronimodelimiters = ".";
    // 5. Palabras con guiones: delimitadores especiales: ?-?
    string guiondelimiters = "-";

    // Se activara el caso especial si se detecta un delimitador especial perteneciente al caso especial
    string::size_type lastPos = efficient_find_first_not_of(str, 0);
    string::size_type pos = efficient_find_first_of(str, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        
        // URL
        if (URLdelimiters.find(str[pos]) != string::npos)
        {
            // Comprobar si es una URL (que comience por ?http:? o ?https:? o ?ftp:? seguido de alg?n car?cter que no sea delimitador)
            if (str.substr(lastPos, 5) == "http:" || str.substr(lastPos, 6) == "https:" || str.substr(lastPos, 4) == "ftp:")
            {
                // Comprobar que va seguido de un car?cter que no sea delimitador
                pos = str.find(":", lastPos)+1;
                if (delimitadores.find(str[pos]) == string::npos || URLdelimiters.find(str[pos]) != string::npos)
                {
                    bool salir = false;
                    // Buscar el siguiente delimitador que no sea de URL pero que s? sea de los delimitadores
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

        // N?meros decimales
        if (delimitersSet.count('.') && delimitersSet.count(','))
        {
            size_t lastPosAux = lastPos;
            size_t posAux = pos;
            // Comprobar si el "." o "," aparecen al principio del token
            if (decimaldelimiters.find(str[lastPos-1]) != string::npos) {
                // Si pos es un "." o ",", se sigue buscando el siguiente caracter que no sea un "." o "," o un digito num?rico
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
            // Comprobar si todos los caracteres entre lastPos y pos son d?gitos num?ricos o "." o ","
            bool esNumeroDecimal = true;
            for (size_t i = lastPosAux; i < posAux; i++) {
                if (!isdigit(str[i]) && decimaldelimiters.find(str[i]) == string::npos) {
                    esNumeroDecimal = false;
                    break;
                }
            }
            // Si es un n?mero decimal, se comprueba si hay un "." o "," antes del primer d?gito num?rico
            // Si lo hay se inserta un "0"
            if (esNumeroDecimal) {
                if (decimaldelimiters.find(str[lastPosAux-1]) != string::npos) {
                    //str.insert(lastPosAux, "0");
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
    
    // A?adir el espacio al set de delimitadores
    delimitersSet.insert(' ');

    ESTADO estado = INICIO;
    char c;
    size_t start;
    bool todo_digitos = true;
    
    // Si no es un delimitador especial, se a?ade el token a la lista de tokens

    // Se detectar?n casos especiales en el siguiente orden:
    // 1. URL: delimitadores especiales: ?:/.?&-=#@?
    string URLdelimiters_string = ":/.?&-=#@";
    unordered_set<char> URLdelimiters = unordered_set<char>(URLdelimiters_string.begin(), URLdelimiters_string.end());
    // 2. N?meros decimales: delimitadores especiales: ?.,?
    string decimaldelimiters_string = ".,";
    unordered_set<char> decimaldelimiters = unordered_set<char>(decimaldelimiters_string.begin(), decimaldelimiters_string.end());
    // 3. E-mail: delimitadores especiales: ?.-_@?
    string emaildelimiters_string = ".-_@";
    unordered_set<char> emaildelimiters = unordered_set<char>(emaildelimiters_string.begin(), emaildelimiters_string.end());
    // 4. Acronimos: delimitadores especiales: ?.?
    string acronimodelimiters_string = ".";
    unordered_set<char> acronimodelimiters = unordered_set<char>(acronimodelimiters_string.begin(), acronimodelimiters_string.end());
    // 5. Palabras con guiones: delimitadores especiales: ?-?
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
                // Comprobar que va seguido de un car?cter que no sea delimitador
                size_t aux = str.find(":", start)+1;
                if (delimitersSet.count(str[aux]) == 0 || URLdelimiters.count(str[aux]) > 0)
                {
                    bool salir = false;
                    // Buscar el siguiente delimitador que no sea de URL pero que s? sea de los delimitadores
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
                // Como no es una url, se a?ade el token a la lista de tokens
                tokens.push_back(str.substr(start, i-start));
                estado = INICIO;
                break;
            }
        case DECIMAL:
            // Comprobar que de momento en el token solo hay digitos numericos
            if (todo_digitos) {
                bool punto_inicial = false;
                if (decimaldelimiters.count(str[start - 1]) > 0) {
                    // Si hay un "." o "," antes del primer d?gito num?rico, se inserta un "0"
                    punto_inicial = true;
                    start--;
                }
                while (i <= str.size()) {
                    
                    // Detectar el final del numero por un espacio; "." o "," seguido de espacio; 
                    // o los s?mbolos "%" y "$" seguidos de espacio (los cuales se almacenar?an en un t?rmino posterior, 
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
*/

void Tokenizador::TokenizarEspeciales(const string &str, list<string> &tokens) {
    
    // A?adir el espacio y el salto de l?nea al set de delimitadores
    delimitersBitset[' '] = 1;
    delimitersBitset['\n'] = 1;

    ESTADO estado = INICIO;
    unsigned char c;
    size_t start;
    bool todo_digitos = true;
    
    // Si no es un delimitador especial, se a?ade el token a la lista de tokens

    // Se detectar?n casos especiales en el siguiente orden:
    // 1. URL: delimitadores especiales: ?:/.?&-=#@?
    // 2. N?meros decimales: delimitadores especiales: ?.,?
    // 3. E-mail: delimitadores especiales: ?.-_@?
    // 4. Acronimos: delimitadores especiales: ?.?
    // 5. Palabras con guiones: delimitadores especiales: ?-?

    // Booleano para saber si se ha confirmado un caso especial
    bool confirmado = false;

    // Booleano que controla si se encuentra "." o "," al principio de un token
    bool delim_at_start = false;

    // Recorrer la cadena a tokenizar de izquierda a derecha hasta encontrar un delimitador o un espacio
    for (size_t i = 0; i < str.size(); i++) {
        c = str[i];
        // Si estamos dentro de un token comprobamos si es un delimitador especial
        if (estado == TOKEN && delimitersBitset[c]) {
            if (URLdelimiters[c])
                confirmado = TokenizarURL(URLdelimiters, str, tokens, start, i);
            if (!confirmado && decimaldelimiters[c])
                confirmado = TokenizarDecimal(decimaldelimiters, str, tokens, start, i, delim_at_start);
            if (!confirmado && c == '@')
                confirmado = TokenizarEmail(emaildelimiters, str, tokens, start, i);
            if (!confirmado && c == '.')
                confirmado = TokenizarAcronimo(str, tokens, start, i);
            if (!confirmado && c == '-')
                confirmado = TokenizarGuion(str, tokens, start, i);
            if (!confirmado){
                tokens.push_back(str.substr(start, i-start));
                estado = INICIO;
                confirmado = false;
            }
            if (confirmado) {
                estado = INICIO;
                confirmado = false;
            }
        }
        // Si estamos al inicio nos saltamos todos los delimitadores hasta encontrar un car?cter
        // Al encontrar el primer car?cter es donde empieza el token
        else if (estado == INICIO) {
            if (!delimitersBitset[c]) {
                start = i;
                estado = TOKEN;
            }
            // Para los casos en que el "." o "," aparece al principio del token
            else if (decimaldelimiters[c]) {
                delim_at_start = true;
                confirmado = TokenizarDecimal(decimaldelimiters, str, tokens, start, i, delim_at_start);
                if (confirmado) {
                    estado = INICIO;
                    confirmado = false;
                }
                delim_at_start = false;
            }
        }
    }
    // Si se acaba el string mientras se estaba leyendo un token, se a?ade el token a la lista de tokens
    if (estado == TOKEN)
        tokens.push_back(str.substr(start, str.size()-start));
}

bool Tokenizador::TokenizarURL(const bitset<256> &URLdelimiters, const string &str, list<string> &tokens, size_t &start, size_t &i) const {
    // Comprobar si empieza por "http:" o "https:" o "ftp:" seguido de algun caracter que no sea delimitador
    if (str.substr(start, 5) == "http:" || str.substr(start, 6) == "https:" || str.substr(start, 4) == "ftp:") {
        // Comprobar que va seguido de al menos un car?cter que no sea delimitador
        size_t aux = str.find(":", start)+1;
        if (aux < str.size() && (!delimitersBitset[(unsigned char)(str[aux])] || URLdelimiters[(unsigned char)(str[aux])]))
        {
            //bool salir = false;
            // Buscar el siguiente delimitador que no sea de URL pero que s? sea de los delimitadores
            i = aux;
            while (i < str.size() && (URLdelimiters[(unsigned char)(str[i])] || !delimitersBitset[(unsigned char)(str[i])]))
                i++;
            /*
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
            }*/
            tokens.push_back(str.substr(start, i-start));
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

bool Tokenizador::TokenizarDecimal(const bitset<256> &decimaldelimiters, const string &str, list<string> &tokens, size_t &start, size_t &i, const bool &delim_at_start) const {
    bool cero_inicial = false;
    // Si el delimitador aparece al principio
    if (delim_at_start) {
        // Si el siguiente car?cter es un d?gito
        if (isdigit(str[i+1])) {
            // Se activa el booleano para 0 inicial
            cero_inicial = true;
            start = i;
        }
        else {
            return false;
        }
    }
    else {
        // Si el delimitador est? en medio del token comprobar que lo de detras sean todo digitos numericos
        for (size_t j = start; j < i; j++) {
            if (!isdigit(str[j])) {
                return false;
            }
        }
        // Comprobar que lo que sigue sea un d?gito numerico
        if (!isdigit(str[i+1])) {
            return false;
        }
    }
    // Comprobar que lo que sigue sean todo digitos numericos o "." o ","
    for (size_t j = i+1; j <= str.size(); j++) {
        // Si aparece un "." o "," seguido de un blanco, o los s?mbolos "$" o "%" seguidos de un blanco, o un delimitador es el final del decimal
        if ((decimaldelimiters[(unsigned char)(str[j])] && str[j+1] == ' ') || 
                    (str[j] == '$' && (str[j+1] == ' ' || (j+1) == str.size())) || 
                    (str[j] == '%' && (str[j+1] == ' ' || (j+1) == str.size())) || 
                    (!decimaldelimiters[(unsigned char)(str[j])] && delimitersBitset[(unsigned char)(str[j])]) ||
                    (decimaldelimiters[(unsigned char)str[j]] && (j+1) == str.size()) || 
                    (j == str.size())) {
            i = j;
            if (cero_inicial)
                tokens.push_back("0"+str.substr(start, i-start));
            else
                tokens.push_back(str.substr(start, i-start));
            // Si al final aparec?an los s?mbolos "$" o "%" se a?aden como tokens separados
            if (str[j] == '$' || str[j] == '%') {
                tokens.push_back(str.substr(j, 1));
            }
            return true;
        }
        // Si aparece un car?cter que no es un d?gito, ni "." o "," el token no es un decimal
        if (!isdigit(str[j]) && !delimitersBitset[(unsigned char)(str[j])]) {
            return false;
        }
        // Si aparece un "." o "," seguido de otro "." o "," el token acaba en el primer "." o ","
        if ((decimaldelimiters[(unsigned char)(str[j])] && decimaldelimiters[(unsigned char)(str[j+1])])) {
            i = j;
            if (cero_inicial)
                tokens.push_back("0"+str.substr(start, i-start));
            else
                tokens.push_back(str.substr(start, i-start));
            return true;
        }
    }
    return true;
}

bool Tokenizador::TokenizarEmail(const bitset<256> &emaildelimiters, const string &str, list<string> &tokens, size_t &start, size_t &i) const {
    // Comprobar que antes del "@" hay al menos un car?cter no delimitador
    if (delimitersBitset[(unsigned char)(str[i-1])]) {
        return false;
    }
    // Comprobar que despu?s del "@" hay al menos un car?cter no delimitador
    if (delimitersBitset[(unsigned char)(str[i+1])] && (i+1) < str.size()) {
        return false;
    }
    // Comprobar que lo que sigue sean car?cteres alfanum?ricos o delimitadores de email
    for (size_t j = i+1; j <= str.size(); j++) {
        // Si aparece una segunda "@" no es un email
        if (str[j] == '@') {
            return false;
        }
        // Si aparece un delimitador de email que no sea "@" este debe ir rodeados de caracteres no delimitadores sin nig?n blanco de por medio, si no acaba el email
        if (emaildelimiters[(unsigned char)(str[j])] && str[j] != '@') {
            if (delimitersBitset[(unsigned char)(str[j-1])] || delimitersBitset[(unsigned char)(str[j+1])] || (j+1) >= str.size()) {
                i = j;
                tokens.push_back(str.substr(start, i-start));
                return true;
            }
            else{
                j++;
                continue;
            }
        }
        // Si aparece un delimitador es el final del email
        if (delimitersBitset[(unsigned char)(str[j])] || j == str.size()) {
            i = j;
            tokens.push_back(str.substr(start, i-start));
            return true;
        }
    }
    i = str.size();
    tokens.push_back(str.substr(start, i-start));
    return true;
}

bool Tokenizador::TokenizarAcronimo(const string &str, list<string> &tokens, size_t &start, size_t &i) const {
    // Comprobar si lo que hay antes y despu?s del "." no son delimitadores
    if (delimitersBitset[(unsigned char)(str[i-1])] || delimitersBitset[(unsigned char)(str[i+1])] || (i+1) >= str.size()) {
        return false;
    }
    // Comprobar que lo que sigue sean car?cteres alfanum?ricos o delimitadores de acr?nimo
    for (size_t j = i+1; j <= str.size(); j++) {
        // Si aparece un "." este debe ir rodeados de caracteres no delimitadores sin nig?n blanco de por medio, si no acaba el acr?nimo
        if (str[j] == '.') {
            if (delimitersBitset[(unsigned char)(str[j-1])] || delimitersBitset[(unsigned char)(str[j+1])] || (j+1) >= str.size()) {
                i = j;
                tokens.push_back(str.substr(start, i-start));
                return true;
            }
            else{
                j++;
                continue;
            }
        }
        // Si aparece un delimitador es el final del acr?nimo
        if (delimitersBitset[(unsigned char)(str[j])] || j == str.size()) {
            i = j;
            tokens.push_back(str.substr(start, i-start));
            return true;
        }
    }
    i = str.size();
    tokens.push_back(str.substr(start, i-start));
    return true;
}

bool Tokenizador::TokenizarGuion(const string &str, list<string> &tokens, size_t &start, size_t &i) const {
    // Comprobar si lo que hay antes y despu?s del "-" no son delimitadores
    if (delimitersBitset[(unsigned char)(str[i-1])] || delimitersBitset[(unsigned char)(str[i+1])] || (i+1) >= str.size()) {
        return false;
    }
    // Comprobar que lo que sigue sean car?cteres alfanum?ricos o guiones pero no dos guiones seguidos
    for (size_t j = i+1; j < str.size(); j++) {
        // Si aparece un "-" este debe ir rodeado de caracteres no delimitadores sin nig?n blanco de por medio, si no acaba la palabra
        if (str[j] == '-') {
            if (delimitersBitset[(unsigned char)(str[j-1])] || delimitersBitset[(unsigned char)(str[j+1])] || (j+1) >= str.size()) {
                i = j;
                tokens.push_back(str.substr(start, i-start));
                return true;
            }
            else{
                j++;
                continue;
            }
        }
        // Si aparece un delimitador es el final del gui?n
        if (delimitersBitset[(unsigned char)(str[j])] || j == str.size()) {
            i = j;
            tokens.push_back(str.substr(start, i-start));
            return true;
        }
    }
    i = str.size();
    tokens.push_back(str.substr(start, i-start));
    return true;
}

// Tokeniza el fichero i guardando la salida en el fichero f (una
// palabra en cada l?nea del fichero). Devolver? true si se realiza la
// tokenizaci?n de forma correcta;
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
// a?adi?ndole extensi?n.tk(sin eliminar previamente la extensi?n de i
// por ejemplo,
// del archivo pp.txt se generar?a el resultado en pp.txt.tk),
// y que contendr? una palabra en cada l?nea del fichero.Devolver? true si
// se realiza la tokenizaci?n de forma correcta;
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
    //delimitersSet.clear();
    //delimitersSet = unordered_set<char>(delimiters.begin(), delimiters.end());
    delimitersBitset.reset();
    for (unsigned char c : delimiters) // loop through each character in s
        delimitersBitset[c] = 1; // set the bit for c to 1
}

void Tokenizador::AnyadirDelimitadoresPalabra(const string &nuevoDelimiters) {
    for (int i = 0; i < nuevoDelimiters.length(); i++) {
        if (delimiters.find(nuevoDelimiters[i]) == string::npos) {
            delimiters += nuevoDelimiters[i];
            //delimitersSet.insert(nuevoDelimiters[i]);
        }
    }
    delimitersBitset.reset();
    for (unsigned char c : delimiters) // loop through each character in s
        delimitersBitset[c] = 1; // set the bit for c to 1
}

string Tokenizador::DelimitadoresPalabra() const {
    return delimiters;
}

void Tokenizador::CasosEspeciales(const bool &nuevoCasosEspeciales) {
    casosEspeciales = nuevoCasosEspeciales;
}

bool Tokenizador::CasosEspeciales() {
    return casosEspeciales;
}

void Tokenizador::PasarAminuscSinAcentos(const bool &nuevoPasarAminuscSinAcentos) {
    pasarAminuscSinAcentos = nuevoPasarAminuscSinAcentos;
}

bool Tokenizador::PasarAminuscSinAcentos() {
    return pasarAminuscSinAcentos;
}

/*
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
*/

string Tokenizador::pasar_a_minusculas_sin_acentos(const string &str) const {
    string result = "";
    for (unsigned char c : str) {
        int code = int(c);
        /*
        if (code >= 192 && code <= 198) {
            result += 'a';
        } else if (code == 199) {
            result += 'c';
        } else if (code >= 200 && code <= 203) {
            result += 'e';
        } else if (code >= 204 && code <= 207) {
            result += 'i';
        } else if (code == 208) {
            result += 'd';
        } else if (code == 209) {
            result += '?';
        } else if (code >= 210 && code <= 214) {
            result += 'o';
        } else if (code == 215) {
            result += 'x';
        } else if (code == 216) {
            result += 'o';
        } else if (code >= 217 && code <= 220) {
            result += 'u';
        } else if (code == 221) {
            result += 'y';
        } else if (code == 222) {
            result += 't';
        } else if (code == 223) {
            result += 'b';
        } else if (code >= 224 && code <= 229) {
            result += 'a';
        } else if (code == 231) {
            result += 'c';
        } else if (code >= 232 && code <= 235) {
            result += 'e';
        } else if (code >= 236 && code <= 239) {
            result += 'i';
        } else if (code == 240) {
            result += 'd';
        } else if (code >= 242 && code <= 246) {
            result += 'o';
        } else if (code == 248) {
            result += 'o';
        } else if (code >= 250 && code <= 252) {
            result += 'u';
        } else if (code == 253) {
            result += 'y';
        } else if (code == 254) {
            result += 't';
        } else if (code == 255) {
            result += 'y';
        }
        else if (code > 256) {
            result += '?';
        }
        else {
            result += tolower(c);
        }*/
        if (code < 192)
            result += tolower(c);
        else if (code == 209)
            result += 'ñ';
        else if (code < 224) {
            double y = floor((code-192)/6);
            if (y<=2)
                result += string(1, ((y + 1) * 4) + 93);
            else
                result += string(1, (y * 6) + 93);
        }
        else if (code == 241)
            result += 'ñ';
        else {
            double y = floor((code - 224) / 6);
            if (y <= 2)
                result += tolower(char(((y + 1) * 4) + 93));
            else
                result += tolower(char((y * 6) + 93));
        }
        
    }
    return result;
}

// Operador de salida de un tokenizador
ostream &operator<<(ostream &os, const Tokenizador &t) {
    os << "DELIMITADORES: " << t.delimiters << " TRATA CASOS ESPECIALES: " << t.casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " 
    << t.pasarAminuscSinAcentos;
    return os;
}