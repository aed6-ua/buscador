#include <iostream>
#include <unordered_map>
#include <list>

using namespace std;

class InformacionTermino
{
    friend ostream &operator<<(ostream &s, const InformacionTermino &p);

public:
    InformacionTermino(const InformacionTermino &);
    InformacionTermino();  // Inicializa ftc = 0
    ~InformacionTermino(); // Pone ftc = 0 y vacía l_docs
    InformacionTermino &operator=(const InformacionTermino &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
    void addFtc() { this->ftc++; }
private:
    int ftc; // Frecuencia total del término en la colección
    unordered_map<int, InfTermDoc> l_docs;
    // Tabla Hash que se accederá por el id del documento, devolviendo un objeto de la clase InfTermDoc que contiene toda la información de aparición del término en el documento
};
ostream &operator<<(ostream &s, const InformacionTermino &p)
{
    s << "Frecuencia total : " << p.ftc << "\tfd : " << p.l_docs.size();
    // A continuación se mostrarían todos los elementos de p.l_docs:
    //s << "\tId.Doc : " << idDoc << "\t" << InfTermDoc;
    return s;
}

class InfTermDoc
{
    friend ostream &operator<<(ostream &s, const InfTermDoc &p);

public:
    InfTermDoc(const InfTermDoc &);
    InfTermDoc();  // Inicializa ft = 0
    ~InfTermDoc(); // Pone ft = 0
    InfTermDoc &operator=(const InfTermDoc &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
private:
    int ft; // Frecuencia del término en el documento
    list<int> posTerm;
    // Solo se almacenará esta información si el campo privado del indexador almacenarPosTerm == true
    /* Lista de números de palabra en los que aparece el término en el
    documento. Los números de palabra comenzarán desde cero (la primera
    palabra del documento). Se numerarán las palabras de parada. Estará
    ordenada de menor a mayor posición. */
};
ostream &operator<<(ostream &s, const InfTermDoc &p)
{
    s << "ft: " << p.ft;
    // A continuación se mostrarían todos los elementos de p.posTerm ("posicion TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB”):
    // s << "\t" << posicion;
    return s;
}

class InfDoc
{
    friend ostream &operator<<(ostream &s, const InfDoc &p);

public:
    InfDoc(const InfDoc &);
    InfDoc();
    ~InfDoc();
    InfDoc &operator=(const InfDoc &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
    time_t getFechaModificacion() const { return fechaModificacion; }
private:
    int idDoc;
    // Identificador del documento. El primer documento indexado en la colección será el identificador 1
    int numPal;          // Nº total de palabras del documento
    int numPalSinParada; // Nº total de palabras sin stop-words del documento
    int numPalDiferentes;
    // Nº total de palabras diferentes que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
    int tamBytes; // Tamaño en bytes del documento
    time_t fechaModificacion;
    // Atributo correspondiente a la fecha y hora (completa) de modificación del documento. El tipo "Fecha/hora” lo elegirá/implementará el alumno
};
ostream &operator<<(ostream &s, const InfDoc &p)
{
    s << "idDoc: " << p.idDoc << "\tnumPal: " << p.numPal << "\tnumPalSinParada: " << p.numPalSinParada << "\tnumPalDiferentes: " << p.numPalDiferentes << "\ttamBytes: " << p.tamBytes;
    return s;
}

class InfColeccionDocs
{
    friend ostream &operator<<(ostream &s, const InfColeccionDocs &p);

public:
    InfColeccionDocs(const InfColeccionDocs &);
    InfColeccionDocs();
    ~InfColeccionDocs();
    InfColeccionDocs &operator=(const InfColeccionDocs &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
private:
    int numDocs; // Nº total de documentos en la colección
    int numTotalPal;
    // Nº total de palabras en la colección
    int numTotalPalSinParada;
    // Nº total de palabras sin stop-words en la colección
    int numTotalPalDiferentes;
    // Nº total de palabras diferentes en la colección que no sean stopwords (sin acumular la frecuencia de cada una de ellas)
    int tamBytes; // Tamaño total en bytes de la colección
};
ostream &operator<<(ostream &s, const InfColeccionDocs &p)
{
    s << "numDocs: " << p.numDocs << "\tnumTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: " << p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes << "\ttamBytes: " << p.tamBytes;
    return s;
}

class InformacionTerminoPregunta
{
    friend ostream &operator<<(ostream &s, const InformacionTerminoPregunta &
                                               p);

public:
    InformacionTerminoPregunta(const InformacionTerminoPregunta &);
    InformacionTerminoPregunta();
    ~InformacionTerminoPregunta();
    InformacionTerminoPregunta &operator=(const InformacionTerminoPregunta
                                              &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
private:
    int ft; // Frecuencia total del término en la pregunta
    list<int> posTerm;
    // Solo se almacenará esta información si el campo privado del indexador almacenarPosTerm == true
    /* Lista de números de palabra en los que aparece el término en la
    pregunta. Los números de palabra comenzarán desde cero (la primera
    palabra de la pregunta). Se numerarán las palabras de parada. Estará
    ordenada de menor a mayor posición.*/
};
ostream &operator<<(ostream &s, const InformacionTerminoPregunta &p)
{
    s << "ft: " << p.ft;
    // A continuación se mostrarían todos los elementos de p.posTerm ("posicion TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB”):
    // s << "\t" << posicion;
    return s;
}

class InformacionPregunta
{
    friend ostream &operator<<(ostream &s, const InformacionPregunta &p);

public:
    InformacionPregunta(const InformacionPregunta &);
    InformacionPregunta();
    ~InformacionPregunta();
    InformacionPregunta &operator=(const InformacionPregunta &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
private:
    int numTotalPal;
    // Nº total de palabras en la pregunta
    int numTotalPalSinParada;
    // Nº total de palabras sin stop-words en la pregunta
    int numTotalPalDiferentes;
    // Nº total de palabras diferentes en la pregunta que no sean stop-words(sin acumular la frecuencia de cada una de ellas)
};
ostream &operator<<(ostream &s, const InformacionPregunta &p)
{
    s << "numTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: " << p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes;
    return s;
}
