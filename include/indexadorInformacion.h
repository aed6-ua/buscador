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

    // Operador de asignación
    InfTermDoc &operator=(const InfTermDoc &infTermDoc) { this->ft = infTermDoc.ft; this->posTerm = infTermDoc.posTerm; return *this;}
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
    void addFt() { this->ft++; }
    void addPosTerm(const int &pos) { this->posTerm.push_back(pos); }
    int getFt() const { return this->ft; }
    void cargar(string serializacion);
private:
    int ft; // Frecuencia del término en el documento
    list<int> posTerm;
    // Solo se almacenará esta información si el campo privado del indexador almacenarPosTerm == true
    /* Lista de números de palabra en los que aparece el término en el
    documento. Los números de palabra comenzarán desde cero (la primera
    palabra del documento). Se numerarán las palabras de parada. Estará
    ordenada de menor a mayor posición. */
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

    // Pone ftc = 0 y vacía l_docs
    ~InformacionTermino() { this->ftc = 0; this->l_docs.clear();}

    // Operador de asignación
    InformacionTermino &operator=(const InformacionTermino &infTerm) { this->ftc = infTerm.ftc; this->l_docs = infTerm.l_docs; return *this;}
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
    void addFtc() { this->ftc++; }
    // Busca el idDoc en la tabla hash l_docs, si lo encuentra devuelve true y en el parámetro infTermDoc devuelve la información del término en el documento
    bool getInfTermDoc(const int &idDoc, InfTermDoc &infTermDoc) const;
    // Añade el idDoc a la tabla hash l_docs, si no existe, y devuelve true, si ya existe devuelve false
    bool addInfTermDoc(const int &idDoc, const InfTermDoc &infTermDoc) { return this->l_docs.insert({idDoc, infTermDoc}).second; }
    // Obtiene el InfTermDoc del idDoc
    InfTermDoc* getInfTermDoc(const int &idDoc) { return &this->l_docs[idDoc]; }
    // Comprobamos si el idDoc existe en la tabla hash l_docs
    bool existIdDoc(const int &idDoc) const { return this->l_docs.count(idDoc); }
    // Borra el idDoc de la tabla hash l_docs y restamos la frecuencia del termino en el documento a la frecuencia total del termino en la coleccion
    void eraseInfTermDoc(const int &idDoc) { this->ftc -= this->l_docs[idDoc].getFt(); this->l_docs.erase(idDoc); }
    // Devuelve el número de documentos en los que aparece el término
    int getNumDocs() const { return this->l_docs.size(); }
    // Cargar la información de un término a partir de su serialización
    void cargar(string serializacion);
private:
    int ftc; // Frecuencia total del término en la colección
    unordered_map<int, InfTermDoc> l_docs;
    // Tabla Hash que se accederá por el id del documento, devolviendo un objeto de la clase InfTermDoc que contiene toda la información de aparición del término en el documento
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
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
    time_t getFechaModificacion() const { return fechaModificacion; }
    // Devuelve el identificador del documento
    int getIdDoc() const { return idDoc; }
    // Devuelve el número total de palabras del documento
    int getNumPal() const { return numPal; }
    // Devuelve el número total de palabras sin stop-words del documento
    int getNumPalSinParada() const { return numPalSinParada; }
    // Devuelve el número total de palabras diferentes que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
    int getNumPalDiferentes() const { return numPalDiferentes; }
    // Devuelve el tamaño en bytes del documento
    int getTamBytes() const { return tamBytes; }
    // Cargar la información de un documento a partir de su serialización
    void cargar(string serializacion);
private:
    int idDoc;
    // Identificador del documento. El primer documento indexado en la colección será el identificador 1
    int numPal;          // Nº total de palabras del documento
    int numPalSinParada; // Nº total de palabras sin stop-words del documento
    int numPalDiferentes;
    // Nº total de palabras diferentes que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
    int tamBytes; // Tamaño en bytes del documento
    time_t fechaModificacion;
    // Atributo correspondiente a la fecha y hora (completa) de modificación del documento. El tipo "Fecha/hora? lo elegirá/implementará el alumno
};

class InfColeccionDocs
{
    friend ostream &operator<<(ostream &s, const InfColeccionDocs &p);

public:
    InfColeccionDocs(const InfColeccionDocs &);
    InfColeccionDocs();
    ~InfColeccionDocs();
    InfColeccionDocs &operator=(const InfColeccionDocs &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
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
    int numDocs; // Nº total de documentos en la colección
    int numTotalPal;
    // Nº total de palabras en la colección
    int numTotalPalSinParada;
    // Nº total de palabras sin stop-words en la colección
    int numTotalPalDiferentes;
    // Nº total de palabras diferentes en la colección que no sean stopwords (sin acumular la frecuencia de cada una de ellas)
    int tamBytes; // Tamaño total en bytes de la colección
};

class InformacionTerminoPregunta
{
    friend ostream &operator<<(ostream &s, const InformacionTerminoPregunta &p);

public:
    // Copiar todos los atributos de la clase InformacionTerminoPregunta
    InformacionTerminoPregunta(const InformacionTerminoPregunta &orig) { this->ft = orig.ft; this->posTerm = orig.posTerm; } 
    
    // Inicializa ft = 0
    InformacionTerminoPregunta() { this->ft = 0; }

    // Pone ft = 0 y vacía posTerm
    ~InformacionTerminoPregunta() { this->ft = 0; this->posTerm.clear(); }

    // Operador de asignación
    InformacionTerminoPregunta &operator=(const InformacionTerminoPregunta &orig) { this->ft = orig.ft; this->posTerm = orig.posTerm; return *this; }
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
    void cargar(string serializacion);
private:
    int ft; // Frecuencia total del término en la pregunta
    list<int> posTerm;
    // Solo se almacenará esta información si el campo privado del indexador almacenarPosTerm == true
    /* Lista de números de palabra en los que aparece el término en la
    pregunta. Los números de palabra comenzarán desde cero (la primera
    palabra de la pregunta). Se numerarán las palabras de parada. Estará
    ordenada de menor a mayor posición.*/
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

    // Operador de asignación
    InformacionPregunta &operator=(const InformacionPregunta &orig) { this->numTotalPal = orig.numTotalPal; this->numTotalPalSinParada = orig.numTotalPalSinParada; this->numTotalPalDiferentes = orig.numTotalPalDiferentes; return *this; }
    // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
    void cargar(string serializacion);
private:
    int numTotalPal;
    // Nº total de palabras en la pregunta
    int numTotalPalSinParada;
    // Nº total de palabras sin stop-words en la pregunta
    int numTotalPalDiferentes;
    // Nº total de palabras diferentes en la pregunta que no sean stop-words(sin acumular la frecuencia de cada una de ellas)
};

#endif