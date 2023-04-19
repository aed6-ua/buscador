#include "indexadorInformacion.h"

using namespace std;

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

// Constructor con parámetros
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

// Operador de asignación
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

// Operador de asignación
InfColeccionDocs &InfColeccionDocs::operator=(const InfColeccionDocs &infColeccionDocs)
{
    this->numDocs = infColeccionDocs.numDocs;
    this->numTotalPal = infColeccionDocs.numTotalPal;
    this->numTotalPalSinParada = infColeccionDocs.numTotalPalSinParada;
    this->numTotalPalDiferentes = infColeccionDocs.numTotalPalDiferentes;
    this->tamBytes = infColeccionDocs.tamBytes;
    return *this;
}