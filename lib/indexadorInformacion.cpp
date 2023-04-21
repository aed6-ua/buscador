#include "indexadorInformacion.h"

using namespace std;

// InfTermDoc

ostream &operator<<(ostream &s, const InfTermDoc &p)
{
    s << "ft: " << p.ft;
        // A continuaci?n se mostrar?an todos los elementos de p.posTerm ("posicion TAB posicion TAB ... posicion, es decir nunca finalizar? en un TAB?):
    for (list<int>::const_iterator it = p.posTerm.begin(); it != p.posTerm.end(); ++it)
        s << "\t" << *it;
    return s;
}

void InfTermDoc::cargar(string serializacion)
{
    int pos;
    // Cargamos la frecuencia del término en el documento (se encuentra después de "ft: ")
    ft = atoi(serializacion.substr(3, serializacion.find("\t", 3) - 3).c_str());
    // Obtenemos la lista de posiciones del término en el documento
    pos = serializacion.find("\t", 3) + 1;
    posTerm.clear();
    while (pos < serializacion.size()) {
        posTerm.push_back(atoi(serializacion.substr(pos, serializacion.find("\t", pos) - pos).c_str()));
        pos = serializacion.find("\t", pos) + 1;
    }
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

void InformacionTermino::cargar(string serializacion)
{
    int pos;
    // Cargamos la frecuencia total (se encuentra después de "Frecuencia total: ")
    ftc = atoi(serializacion.substr(18, serializacion.find("\tfd: ") - 18).c_str());
    // Obtenemos el tamaño de la lista de documentos (se encuentra después de "\tfd: ")
    pos = serializacion.find("\tfd: ") + 5;
    int tamListaDocs = atoi(serializacion.substr(pos, serializacion.find("\tId.Doc: ", pos) - pos).c_str());
    // Obtenemos la lista de documentos
    l_docs.clear();
    pos = serializacion.find("\tId.Doc: ") + 9;
    for (int i = 0; i < tamListaDocs; i++) {
        // Obtenemos el id del documento
        int idDoc = atoi(serializacion.substr(pos, serializacion.find("\tft: ", pos) - pos).c_str());
        pos = serializacion.find("\tft: ", pos) + 5;
        // Deserializamos la información del término en el documento
        InfTermDoc infTermDoc;
        infTermDoc.cargar(serializacion.substr(pos, serializacion.find("\tId.Doc: ", pos) - pos));
        // Insertamos el objeto en la lista de documentos
        l_docs.insert(make_pair(idDoc, infTermDoc));
        pos = serializacion.find("\tId.Doc: ", pos) + 9;
    }
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