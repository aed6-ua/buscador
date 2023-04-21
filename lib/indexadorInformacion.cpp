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
    ft = atoi(serializacion.substr(0,serializacion.find("\t")).c_str());
    // Obtenemos la lista de posiciones del término en el documento
    pos = serializacion.find("\t") + 1;
    posTerm.clear();
    while (pos < (serializacion.size() - 1))
    {
        posTerm.push_back(atoi(serializacion.substr(pos, serializacion.find("\t", pos) - pos).c_str()));
        pos = serializacion.find("\t", pos) + 1;
    }
    posTerm.push_back(atoi(serializacion.substr(pos, serializacion.find(("\n" || "\tId.Doc: "), pos) - pos).c_str()));
}

// InformacionTermino

bool InformacionTermino::getInfTermDoc(const int &idDoc, InfTermDoc &infTermDoc) const
{
    unordered_map<int, InfTermDoc>::const_iterator it = l_docs.find(idDoc);
    if (it != l_docs.end())
    {
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
    for (int i = 0; i < (tamListaDocs - 1); i++)
    {
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
    // Obtenemos el id del documento
    int idDoc = atoi(serializacion.substr(pos, serializacion.find("\tft: ", pos) - pos).c_str());
    pos = serializacion.find("\tft: ", pos) + 5;
    // Deserializamos la información del término en el documento
    InfTermDoc infTermDoc;
    infTermDoc.cargar(serializacion.substr(pos, serializacion.find("\n", pos) - pos));
    // Insertamos el objeto en la lista de documentos
    l_docs.insert(make_pair(idDoc, infTermDoc));
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

ostream &operator<<(ostream &s, const InfDoc &p)
{
    s << "idDoc: " << p.idDoc << "\tnumPal: " << p.numPal << "\tnumPalSinParada: " << p.numPalSinParada << "\tnumPalDiferentes: " << p.numPalDiferentes << "\ttamBytes: " << p.tamBytes;
    return s;
}

void InfDoc::cargar(string serializacion)
{
    int pos;
    // Cargamos el id del documento (se encuentra después de "idDoc: ")
    idDoc = atoi(serializacion.substr(6, serializacion.find("\tnumPal: ") - 6).c_str());
    // Cargamos el número de palabras (se encuentra después de "\tnumPal: ")
    pos = serializacion.find("\tnumPal: ") + 9;
    numPal = atoi(serializacion.substr(pos, serializacion.find("\tnumPalSinParada: ", pos) - pos).c_str());
    // Cargamos el número de palabras sin parada (se encuentra después de "\tnumPalSinParada: ")
    pos = serializacion.find("\tnumPalSinParada: ", pos) + 18;
    numPalSinParada = atoi(serializacion.substr(pos, serializacion.find("\tnumPalDiferentes: ", pos) - pos).c_str());
    // Cargamos el número de palabras diferentes (se encuentra después de "\tnumPalDiferentes: ")
    pos = serializacion.find("\tnumPalDiferentes: ", pos) + 19;
    numPalDiferentes = atoi(serializacion.substr(pos, serializacion.find("\ttamBytes: ", pos) - pos).c_str());
    // Cargamos el tamaño en bytes (se encuentra después de "\ttamBytes: ")
    pos = serializacion.find("\ttamBytes: ", pos) + 10;
    tamBytes = atoi(serializacion.substr(pos, serializacion.find("\tfechaModificacion: ", pos) - pos).c_str());
    // Cargamos la fecha de modificación (se encuentra después de "\tfechaModificacion: ")
    pos = serializacion.find("\tfechaModificacion: ", pos) + 20;
    fechaModificacion = atoi(serializacion.substr(pos, serializacion.find("\n", pos) - pos).c_str());
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

ostream &operator<<(ostream &s, const InfColeccionDocs &p)
{
    s << "numDocs: " << p.numDocs << "\tnumTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: " << p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes << "\ttamBytes: " << p.tamBytes;
    return s;
}

void InfColeccionDocs::cargar(string serializacion)
{
    int pos;
    // Cargamos el número de documentos (se encuentra después de "numDocs: ")
    numDocs = atoi(serializacion.substr(8, serializacion.find("\tnumTotalPal: ") - 8).c_str());
    // Cargamos el número total de palabras (se encuentra después de "\tnumTotalPal: ")
    pos = serializacion.find("\tnumTotalPal: ") + 14;
    numTotalPal = atoi(serializacion.substr(pos, serializacion.find("\tnumTotalPalSinParada: ", pos) - pos).c_str());
    // Cargamos el número total de palabras sin parada (se encuentra después de "\tnumTotalPalSinParada: ")
    pos = serializacion.find("\tnumTotalPalSinParada: ", pos) + 23;
    numTotalPalSinParada = atoi(serializacion.substr(pos, serializacion.find("\tnumTotalPalDiferentes: ", pos) - pos).c_str());
    // Cargamos el número total de palabras diferentes (se encuentra después de "\tnumTotalPalDiferentes: ")
    pos = serializacion.find("\tnumTotalPalDiferentes: ", pos) + 24;
    numTotalPalDiferentes = atoi(serializacion.substr(pos, serializacion.find("\ttamBytes: ", pos) - pos).c_str());
    // Cargamos el tamaño en bytes (se encuentra después de "\ttamBytes: ")
    pos = serializacion.find("\ttamBytes: ", pos) + 10;
    tamBytes = atoi(serializacion.substr(pos, serializacion.find("\n", pos) - pos).c_str());
}

// InformacionTerminoPregunta

ostream &operator<<(ostream &s, const InformacionTerminoPregunta &p)
{
    s << "ft: " << p.ft;
    // A continuación se mostrarían todos los elementos de p.posTerm ("posicion TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB?):
    for (auto i = p.posTerm.begin(); i != p.posTerm.end(); i++)
        s << "\t" << *i;
    // s << "\t" << posicion;
    return s;
}

void InformacionTerminoPregunta::cargar(string serializacion)
{
    int pos;
    // Cargamos el ft (se encuentra después de "ft: ")
    ft = atoi(serializacion.substr(3, serializacion.find("\t") - 3).c_str());
    // Cargamos las posiciones (se encuentran después de "\tposTerm: ")
    pos = serializacion.find("\t", 3) + 1;
    while (pos < serializacion.length())
    {
        posTerm.push_back(atoi(serializacion.substr(pos, serializacion.find("\t", pos) - pos).c_str()));
        pos = serializacion.find("\t", pos) + 1;
    }
}

// InformacionPregunta

ostream &operator<<(ostream &s, const InformacionPregunta &p)
{
    s << "numTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: " << p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes;
    return s;
}

void InformacionPregunta::cargar(string serializacion)
{
    int pos;
    // Cargamos el número total de palabras (se encuentra después de "numTotalPal: ")
    numTotalPal = atoi(serializacion.substr(13, serializacion.find("\tnumTotalPalSinParada: ") - 13).c_str());
    // Cargamos el número total de palabras sin parada (se encuentra después de "\tnumTotalPalSinParada: ")
    pos = serializacion.find("\tnumTotalPalSinParada: ") + 23;
    numTotalPalSinParada = atoi(serializacion.substr(pos, serializacion.find("\tnumTotalPalDiferentes: ", pos) - pos).c_str());
    // Cargamos el número total de palabras diferentes (se encuentra después de "\tnumTotalPalDiferentes: ")
    pos = serializacion.find("\tnumTotalPalDiferentes: ", pos) + 24;
    numTotalPalDiferentes = atoi(serializacion.substr(pos, serializacion.find("\n", pos) - pos).c_str());
}