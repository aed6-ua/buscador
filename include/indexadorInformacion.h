#ifndef _INDEXADORINFORMACION_H_
#define _INDEXADORINFORMACION_H_

#include <iostream>
#include <unordered_map>
#include <list>

using namespace std;

class InfTermDoc
{
    friend ostream &operator<<(ostream &s, const InfTermDoc &p);

public:
    // Constructor de copia
    InfTermDoc(const InfTermDoc &infTermDoc) { this->ft = infTermDoc.ft; this->posTerm = infTermDoc.posTerm;}

    // Inicializa ft = 0
    InfTermDoc() { this->ft = 0; this->posTerm.clear(); }

    // Pone ft = 0
    ~InfTermDoc() { this->ft = 0;}

    // Operador de asignaci�n
    InfTermDoc &operator=(const InfTermDoc &infTermDoc) { this->ft = infTermDoc.ft; this->posTerm = infTermDoc.posTerm; return *this;}
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte privada de la clase
    void addFt() { this->ft++; }
    void addPosTerm(const int &pos) { this->posTerm.push_back(pos); }
    int getFt() const { return this->ft; }
    void cargar(string serializacion);
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
    friend ostream &operator<<(ostream &s, const InformacionTermino &p);

public:
    // Constructor de copia
    InformacionTermino(const InformacionTermino &infTerm) { this->ftc = infTerm.ftc; this->l_docs = infTerm.l_docs;} 

    // Inicializa ftc = 0
    InformacionTermino() { this->ftc = 0;}

    // Constructor por defecto
    InformacionTermino(const int &ftc) { this->ftc = ftc; this->l_docs.clear();}

    // Pone ftc = 0 y vac�a l_docs
    ~InformacionTermino() { this->ftc = 0; this->l_docs.clear();}

    // Operador de asignaci�n
    InformacionTermino &operator=(const InformacionTermino &infTerm) { this->ftc = infTerm.ftc; this->l_docs = infTerm.l_docs; return *this;}
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte privada de la clase
    void addFtc() { this->ftc++; }
    // Busca el idDoc en la tabla hash l_docs, si lo encuentra devuelve true y en el par�metro infTermDoc devuelve la informaci�n del t�rmino en el documento
    bool getInfTermDoc(const int &idDoc, InfTermDoc &infTermDoc) const;
    // A�ade el idDoc a la tabla hash l_docs, si no existe, y devuelve true, si ya existe devuelve false
    bool addInfTermDoc(const int &idDoc, const InfTermDoc &infTermDoc) { return this->l_docs.insert({idDoc, infTermDoc}).second; }
    // Obtiene el InfTermDoc del idDoc
    InfTermDoc* getInfTermDoc(const int &idDoc) { return &this->l_docs[idDoc]; }
    // Comprobamos si el idDoc existe en la tabla hash l_docs
    bool existIdDoc(const int &idDoc) const { return this->l_docs.count(idDoc); }
    // Borra el idDoc de la tabla hash l_docs y restamos la frecuencia del termino en el documento a la frecuencia total del termino en la coleccion
    void eraseInfTermDoc(const int &idDoc) { this->ftc -= this->l_docs[idDoc].getFt(); this->l_docs.erase(idDoc); }
    // Devuelve el n�mero de documentos en los que aparece el t�rmino
    int getNumDocs() const { return this->l_docs.size(); }
    // Cargar la informaci�n de un t�rmino a partir de su serializaci�n
    void cargar(string serializacion);
private:
    int ftc; // Frecuencia total del t�rmino en la colecci�n
    unordered_map<int, InfTermDoc> l_docs;
    // Tabla Hash que se acceder� por el id del documento, devolviendo un objeto de la clase InfTermDoc que contiene toda la informaci�n de aparici�n del t�rmino en el documento
};

class InfDoc
{
    friend ostream &operator<<(ostream &s, const InfDoc &p);

public:
    // Copiar todos los atributos de la clase InfDoc
    InfDoc(const InfDoc &);

    // Inicializa todos los atributos de la clase InfDoc
    InfDoc();

    InfDoc(int idDoc, int numPal, int numPalSinParada, int numPalDiferentes, int tamBytes, time_t fechaModificacion);

    // Pone todos los atributos de la clase InfDoc a 0
    ~InfDoc();

    InfDoc &operator=(const InfDoc &);
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte privada de la clase
    time_t getFechaModificacion() const { return fechaModificacion; }
    // Devuelve el identificador del documento
    int getIdDoc() const { return idDoc; }
    // Devuelve el n�mero total de palabras del documento
    int getNumPal() const { return numPal; }
    // Devuelve el n�mero total de palabras sin stop-words del documento
    int getNumPalSinParada() const { return numPalSinParada; }
    // Devuelve el n�mero total de palabras diferentes que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
    int getNumPalDiferentes() const { return numPalDiferentes; }
    // Devuelve el tama�o en bytes del documento
    int getTamBytes() const { return tamBytes; }
    // Cargar la informaci�n de un documento a partir de su serializaci�n
    void cargar(string serializacion);
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
    friend ostream &operator<<(ostream &s, const InfColeccionDocs &p);

public:
    InfColeccionDocs(const InfColeccionDocs &);
    InfColeccionDocs();
    ~InfColeccionDocs();
    InfColeccionDocs &operator=(const InfColeccionDocs &);
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte privada de la clase
    int getNumDocs() const { return numDocs; }
    void addDoc() { this->numDocs++; }
    void addNumTotalPal(int numPal) { this->numTotalPal += numPal; }
    void addNumTotalPalSinParada(int numPalSinParada) { this->numTotalPalSinParada += numPalSinParada; }
    void addNumTotalPalDiferentes(int numPalDiferentes) { this->numTotalPalDiferentes += numPalDiferentes; }
    void addTamBytes(int tamBytes) { this->tamBytes += tamBytes; }
    void subDoc() { this->numDocs--; }
    void subNumTotalPal(int numPal) { this->numTotalPal -= numPal; }
    void subNumTotalPalSinParada(int numPalSinParada) { this->numTotalPalSinParada -= numPalSinParada; }
    void subNumTotalPalDiferentes() { this->numTotalPalDiferentes--; }
    void subTamBytes(int tamBytes) { this->tamBytes -= tamBytes; }
    void cargar(string serializacion);
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
    friend ostream &operator<<(ostream &s, const InformacionTerminoPregunta &p);

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
    void cargar(string serializacion);
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
    friend ostream &operator<<(ostream &s, const InformacionPregunta &p);
    

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
    void cargar(string serializacion);
private:
    int numTotalPal;
    // N� total de palabras en la pregunta
    int numTotalPalSinParada;
    // N� total de palabras sin stop-words en la pregunta
    int numTotalPalDiferentes;
    // N� total de palabras diferentes en la pregunta que no sean stop-words(sin acumular la frecuencia de cada una de ellas)
};

#endif