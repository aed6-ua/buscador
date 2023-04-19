#ifndef _INDEXADORINFORMACION_H_
#define _INDEXADORINFORMACION_H_

#include <iostream>
#include <unordered_map>
#include <list>

using namespace std;

class InfTermDoc
{
    friend ostream &operator<<(ostream &s, const InfTermDoc &p)
    {
        s << "ft: " << p.ft;
        // A continuaci�n se mostrar�an todos los elementos de p.posTerm ("posicion TAB posicion TAB ... posicion, es decir nunca finalizar� en un TAB?):
        // s << "\t" << posicion;
        return s;
    }

public:
    // Constructor de copia
    InfTermDoc(const InfTermDoc &infTermDoc) { this->ft = infTermDoc.ft; this->posTerm = infTermDoc.posTerm;}

    // Inicializa ft = 0
    InfTermDoc() { this->ft = 0; }

    // Pone ft = 0
    ~InfTermDoc() { this->ft = 0;}

    // Operador de asignaci�n
    InfTermDoc &operator=(const InfTermDoc &infTermDoc) { this->ft = infTermDoc.ft; this->posTerm = infTermDoc.posTerm; return *this;}
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte privada de la clase
private:
    int ft; // Frecuencia del t�rmino en el documento
    list<int> posTerm;
    // Solo se almacenar� esta informaci�n si el campo privado del indexador almacenarPosTerm == true
    /* Lista de n�meros de palabra en los que aparece el t�rmino en el
    documento. Los n�meros de palabra comenzar�n desde cero (la primera
    palabra del documento). Se numerar�n las palabras de parada. Estar�
    ordenada de menor a mayor posici�n. */
};

class InformacionTermino
{
    friend ostream &operator<<(ostream &s, const InformacionTermino &p)
    {
        s << "Frecuencia total : " << p.ftc << "\tfd : " << p.l_docs.size();
        // A continuaci�n se mostrar�an todos los elementos de p.l_docs:
        // s << "\tId.Doc : " << idDoc << "\t" << InfTermDoc;
        return s;
    }

public:
    // Constructor de copia
    InformacionTermino(const InformacionTermino &infTerm) { this->ftc = infTerm.ftc; this->l_docs = infTerm.l_docs;} 

    // Inicializa ftc = 0
    InformacionTermino() { this->ftc = 0;}  

    // Pone ftc = 0 y vac�a l_docs
    ~InformacionTermino() { this->ftc = 0; this->l_docs.clear();}

    // Operador de asignaci�n
    InformacionTermino &operator=(const InformacionTermino &infTerm) { this->ftc = infTerm.ftc; this->l_docs = infTerm.l_docs; return *this;}
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte privada de la clase
    void addFtc() { this->ftc++; }

private:
    int ftc; // Frecuencia total del t�rmino en la colecci�n
    unordered_map<int, InfTermDoc> l_docs;
    // Tabla Hash que se acceder� por el id del documento, devolviendo un objeto de la clase InfTermDoc que contiene toda la informaci�n de aparici�n del t�rmino en el documento
};

class InfDoc
{
    friend ostream &operator<<(ostream &s, const InfDoc &p)
    {
        s << "idDoc: " << p.idDoc << "\tnumPal: " << p.numPal << "\tnumPalSinParada: " << p.numPalSinParada << "\tnumPalDiferentes: " << p.numPalDiferentes << "\ttamBytes: " << p.tamBytes;
        return s;
    }

public:
    // Copiar todos los atributos de la clase InfDoc
    InfDoc(const InfDoc &);

    // Inicializa todos los atributos de la clase InfDoc
    InfDoc();

    // Pone todos los atributos de la clase InfDoc a 0
    ~InfDoc();

    InfDoc &operator=(const InfDoc &);
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte privada de la clase
    time_t getFechaModificacion() const { return fechaModificacion; }

private:
    int idDoc;
    // Identificador del documento. El primer documento indexado en la colecci�n ser� el identificador 1
    int numPal;          // N� total de palabras del documento
    int numPalSinParada; // N� total de palabras sin stop-words del documento
    int numPalDiferentes;
    // N� total de palabras diferentes que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
    int tamBytes; // Tama�o en bytes del documento
    time_t fechaModificacion;
    // Atributo correspondiente a la fecha y hora (completa) de modificaci�n del documento. El tipo "Fecha/hora? lo elegir�/implementar� el alumno
};

class InfColeccionDocs
{
    friend ostream &operator<<(ostream &s, const InfColeccionDocs &p)
    {
        s << "numDocs: " << p.numDocs << "\tnumTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: " << p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes << "\ttamBytes: " << p.tamBytes;
        return s;
    }

public:
    InfColeccionDocs(const InfColeccionDocs &);
    InfColeccionDocs();
    ~InfColeccionDocs();
    InfColeccionDocs &operator=(const InfColeccionDocs &);
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte privada de la clase
private:
    int numDocs; // N� total de documentos en la colecci�n
    int numTotalPal;
    // N� total de palabras en la colecci�n
    int numTotalPalSinParada;
    // N� total de palabras sin stop-words en la colecci�n
    int numTotalPalDiferentes;
    // N� total de palabras diferentes en la colecci�n que no sean stopwords (sin acumular la frecuencia de cada una de ellas)
    int tamBytes; // Tama�o total en bytes de la colecci�n
};

class InformacionTerminoPregunta
{
    friend ostream &operator<<(ostream &s, const InformacionTerminoPregunta &p)
    {
        s << "ft: " << p.ft;
        // A continuaci�n se mostrar�an todos los elementos de p.posTerm ("posicion TAB posicion TAB ... posicion, es decir nunca finalizar� en un TAB?):
        // s << "\t" << posicion;
        return s;
    }

public:
    // Copiar todos los atributos de la clase InformacionTerminoPregunta
    InformacionTerminoPregunta(const InformacionTerminoPregunta &orig) { this->ft = orig.ft; this->posTerm = orig.posTerm; } 
    
    // Inicializa ft = 0
    InformacionTerminoPregunta() { this->ft = 0; }

    // Pone ft = 0 y vac�a posTerm
    ~InformacionTerminoPregunta() { this->ft = 0; this->posTerm.clear(); }

    // Operador de asignaci�n
    InformacionTerminoPregunta &operator=(const InformacionTerminoPregunta &orig) { this->ft = orig.ft; this->posTerm = orig.posTerm; return *this; }
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte privada de la clase
private:
    int ft; // Frecuencia total del t�rmino en la pregunta
    list<int> posTerm;
    // Solo se almacenar� esta informaci�n si el campo privado del indexador almacenarPosTerm == true
    /* Lista de n�meros de palabra en los que aparece el t�rmino en la
    pregunta. Los n�meros de palabra comenzar�n desde cero (la primera
    palabra de la pregunta). Se numerar�n las palabras de parada. Estar�
    ordenada de menor a mayor posici�n.*/
};

class InformacionPregunta
{
    friend ostream &operator<<(ostream &s, const InformacionPregunta &p)
    {
        s << "numTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: " << p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes;
        return s;
    }

public:
    // Copiar todos los atributos de la clase InformacionPregunta
    InformacionPregunta(const InformacionPregunta &orig) { this->numTotalPal = orig.numTotalPal; this->numTotalPalSinParada = orig.numTotalPalSinParada; this->numTotalPalDiferentes = orig.numTotalPalDiferentes; }

    // Inicializa todos los atributos de la clase InformacionPregunta
    InformacionPregunta() { this->numTotalPal = 0; this->numTotalPalSinParada = 0; this->numTotalPalDiferentes = 0; }

    // Pone todos los atributos de la clase InformacionPregunta a 0
    ~InformacionPregunta() { this->numTotalPal = 0; this->numTotalPalSinParada = 0; this->numTotalPalDiferentes = 0; }

    // Operador de asignaci�n
    InformacionPregunta &operator=(const InformacionPregunta &orig) { this->numTotalPal = orig.numTotalPal; this->numTotalPalSinParada = orig.numTotalPalSinParada; this->numTotalPalDiferentes = orig.numTotalPalDiferentes; return *this; }
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte privada de la clase
private:
    int numTotalPal;
    // N� total de palabras en la pregunta
    int numTotalPalSinParada;
    // N� total de palabras sin stop-words en la pregunta
    int numTotalPalDiferentes;
    // N� total de palabras diferentes en la pregunta que no sean stop-words(sin acumular la frecuencia de cada una de ellas)
};

#endif