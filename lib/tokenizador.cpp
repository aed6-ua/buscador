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
    delimiters = ",;:.-/+*\\ '\"{}[]()<>?!??&#=\t\n\r@";
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

void Tokenizador::TokenizarSimple2(const string &str, vector<string> &tokens)
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
        if (delimitersBitset[c]) {
            if (estado == TOKEN) {
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
        else {
            // Si estamos al inicio nos saltamos todos los delimitadores hasta encontrar un car?cter
            // Al encontrar el primer car?cter es donde empieza el token
            if (estado == INICIO) {
                start = i;
                estado = TOKEN;
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
    string salida = "";
    list<string> tokens;
    list<string>::iterator itS;
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
                for (itS = tokens.begin(); itS != tokens.end(); itS++)
                {
                    salida += (*itS) + "\n";
                }
            }
        }
    }
    i.close();
    f.open(NomFichSal.c_str());
    f << salida;
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

bool Tokenizador::Tokenizar(const string &NomFichEntr, vector<string> &salida)
{
    ifstream i;
    ofstream f;
    string cadena;
    list<string> tokens;
    list<string>::iterator itS;
    //string ssalida = "";
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
                // Limpiar la lista de tokens
                tokens.clear();
                if (casosEspeciales) {
                    if (pasarAminuscSinAcentos)
                        TokenizarEspeciales(pasar_a_minusculas_sin_acentos(cadena), tokens);
                    else
                        TokenizarEspeciales(cadena, tokens);
                    salida.insert(salida.end(), tokens.begin(), tokens.end());
                }
                else
                    if (pasarAminuscSinAcentos)
                        TokenizarSimple2(pasar_a_minusculas_sin_acentos(cadena), salida);
                    else
                        TokenizarSimple2(cadena, salida);
                /*
                for (itS = tokens.begin(); itS != tokens.end(); itS++)
                {
                    ssalida += (*itS) + "\n";
                }
                */
            }
        }
    }
    i.close();
    /*
    f.open((NomFichEntr + ".tk").c_str());
    f << ssalida;
    f.close();*/
    return true;
}


bool Tokenizador::TokenizarListaFicheros(const string &NomFichEntr)
{
    ifstream i;
    ofstream f;
    string cadena;
    vector<string> tokens;
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
                Tokenizar(cadena, tokens);
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
    delimitersBitset.reset();
    for (unsigned char c : delimiters) // loop through each character in s
        delimitersBitset[c] = 1; // set the bit for c to 1
}

void Tokenizador::AnyadirDelimitadoresPalabra(const string &nuevoDelimiters) {
    for (int i = 0; i < nuevoDelimiters.length(); i++) {
        if (delimiters.find(nuevoDelimiters[i]) == string::npos) {
            delimiters += nuevoDelimiters[i];
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

bool Tokenizador::CasosEspeciales() const {
    return casosEspeciales;
}

void Tokenizador::PasarAminuscSinAcentos(const bool &nuevoPasarAminuscSinAcentos) {
    pasarAminuscSinAcentos = nuevoPasarAminuscSinAcentos;
}

bool Tokenizador::PasarAminuscSinAcentos() const {
    return pasarAminuscSinAcentos;
}

string Tokenizador::pasar_a_minusculas_sin_acentos(const string &str) const {
    static const unsigned char acentos[256] = {
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ','!','"','#','$','%','&',' ',
        '(',')','*','+',',','-','.','/','0','1',
        '2','3','4','5','6','7','8','9',':',';',
        '<','=','>','?','@','a','b','c','d','r',
        'f','g','h','i','j','k','l','m','n','o',
        'p','q','r','s','t','u','v','w','x','y',
        'z',' ',' ',' ',' ',' ',' ','a','b','c',
        'd','e','f','g','h','i','j','k','l','m',
        'n','o','p','q','r','s','t','u','v','w',
        'x','y','z',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ','a','a','a','a','a','a','a',
        'c','e','e','e','e','i','i','i','i','d','n',
        'o','o','o','o','o','o','/','o','u','u',
        'u','u','y','b','y','n','a','a','a','a',
        'a','a','c','e','e','e','i','i','i','i',
        'd','n','o','o','o','o','o','o','o','u',
        'u','u','u','y','b','y'
    };
    string resultado = "";
    for (char c : str) {
        if (c >= 0x41 && c <= 0x5a) {
            resultado += acentos[c];
        } else if (c >= 0xc0) {
            resultado += acentos[c];
        } else {
            // No es una letra acentuada, se agrega tal cual
            resultado += c;
        }
    }
    return resultado;
    /*string result = "";
    for (unsigned char c : str) {
        int code = int(c);
        if (code < 192)
            result += tolower(c);
        else if (code == 209)
            result += '?';
        else if (code < 224) {
            double y = floor((code-192)/6);
            if (y<=2)
                result += string(1, ((y + 1) * 4) + 93);
            else
                result += string(1, (y * 6) + 93);
        }
        else if (code == 241)
            result += '?';
        else {
            double y = floor((code - 224) / 6);
            if (y <= 2)
                result += tolower(char(((y + 1) * 4) + 93));
            else
                result += tolower(char((y * 6) + 93));
        }
        
    }
    return result;*/
}

// Operador de salida de un tokenizador
ostream &operator<<(ostream &os, const Tokenizador &t) {
    os << "DELIMITADORES: " << t.delimiters << " TRATA CASOS ESPECIALES: " << t.casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " 
    << t.pasarAminuscSinAcentos;
    return os;
}