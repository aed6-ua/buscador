#include "indexadorInformacion.h"

using namespace std;

// InfTermDoc

ostream &operator<<(ostream &s, const InfTermDoc &p)
{
    s << "ft: " << p.ft;
        // A continuación se mostrarían todos los elementos de p.posTerm ("posicion TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB?):
    for (list<int>::const_iterator it = p.posTerm.begin(); it != p.posTerm.end(); ++it)
        s << "\t" << *it;
    return s;
}

// InformacionTermino

bool InformacionTermino::getInfTermDoc(const int &idDoc, InfTermDoc &infTermDoc) const
{
    unordered_map<int, InfTermDoc>::const_iterator it = l_docs.find(idDoc);
    if (it != l_docs.end()) {
        infTermDoc = it->second;
        return true;
    }
    return false;
}

ostream &operator<<(ostream &s, const InformacionTermino &p)
{
    s << "Frecuencia total: " << p.ftc << "\tfd: " << p.l_docs.size();
    // Mostrar todos los elementos de p.l_docs
    for (unordered_map<int, InfTermDoc>::const_iterator it = p.l_docs.begin(); it != p.l_docs.end(); ++it)
        s << "\tId.Doc: " << it->first << "\t" << it->second;
    return s;
}



// InfDoc

// Constructor por defecto
InfDoc::InfDoc()
{
    idDoc = 0;
    numPal = 0;
    numPalSinParada = 0;
    numPalDiferentes = 0;
    tamBytes = 0;
    fechaModificacion = 0;
}

// Constructor de copia
InfDoc::InfDoc(const InfDoc &infDoc)
{
    idDoc = infDoc.idDoc;
    numPal = infDoc.numPal;
    numPalSinParada = infDoc.numPalSinParada;
    numPalDiferentes = infDoc.numPalDiferentes;
    tamBytes = infDoc.tamBytes;
    fechaModificacion = infDoc.fechaModificacion;
}

// Constructor con parÃ¡metros
InfDoc::InfDoc(int idDoc, int numPal, int numPalSinParada, int numPalDiferentes, int tamBytes, time_t fechaModificacion)
{
    this->idDoc = idDoc;
    this->numPal = numPal;
    this->numPalSinParada = numPalSinParada;
    this->numPalDiferentes = numPalDiferentes;
    this->tamBytes = tamBytes;
    this->fechaModificacion = fechaModificacion;
}

// Destructor
InfDoc::~InfDoc()
{
    idDoc = 0;
    numPal = 0;
    numPalSinParada = 0;
    numPalDiferentes = 0;
    tamBytes = 0;
    fechaModificacion = 0;
}

// Operador de asignaciÃ³n
InfDoc &InfDoc::operator=(const InfDoc &infDoc)
{
    idDoc = infDoc.idDoc;
    numPal = infDoc.numPal;
    numPalSinParada = infDoc.numPalSinParada;
    numPalDiferentes = infDoc.numPalDiferentes;
    tamBytes = infDoc.tamBytes;
    fechaModificacion = infDoc.fechaModificacion;
    return *this;
}

// InfColeccionDocs

// Constructor por defecto
InfColeccionDocs::InfColeccionDocs()
{
    this->numDocs = 0;
    this->numTotalPal = 0;
    this->numTotalPalSinParada = 0;
    this->numTotalPalDiferentes = 0;
    this->tamBytes = 0;
}

// Constructor de copia
InfColeccionDocs::InfColeccionDocs(const InfColeccionDocs &infColeccionDocs)
{
    this->numDocs = infColeccionDocs.numDocs;
    this->numTotalPal = infColeccionDocs.numTotalPal;
    this->numTotalPalSinParada = infColeccionDocs.numTotalPalSinParada;
    this->numTotalPalDiferentes = infColeccionDocs.numTotalPalDiferentes;
    this->tamBytes = infColeccionDocs.tamBytes;
}

// Destructor
InfColeccionDocs::~InfColeccionDocs()
{
    this->numDocs = 0;
    this->numTotalPal = 0;
    this->numTotalPalSinParada = 0;
    this->numTotalPalDiferentes = 0;
    this->tamBytes = 0;
}

// Operador de asignaciÃ³n
InfColeccionDocs &InfColeccionDocs::operator=(const InfColeccionDocs &infColeccionDocs)
{
    this->numDocs = infColeccionDocs.numDocs;
    this->numTotalPal = infColeccionDocs.numTotalPal;
    this->numTotalPalSinParada = infColeccionDocs.numTotalPalSinParada;
    this->numTotalPalDiferentes = infColeccionDocs.numTotalPalDiferentes;
    this->tamBytes = infColeccionDocs.tamBytes;
    return *this;
}