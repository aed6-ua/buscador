#include <sys/stat.h>
#include <unordered_set>
#include <dirent.h>
#include <cstring>
#include <filesystem>
#include <math.h>
#include <algorithm>
#include "indexadorHash.h"
#include "tokenizador.h"
#include "stemmer.h"
int LIMITE_INDEXACION_MEMORIA = 100;
using namespace std;

// IndexadorHash
/* ?fichStopWords? serï¿½ el nombre del archivo que contendrï¿½ todas las
palabras de parada (una palabra por cada lï¿½nea del fichero) y se
almacenarï¿½ en el campo privado ?ficheroStopWords?. Asimismo, almacenarï¿½
todas las palabras de parada que contenga el archivo en el campo privado
?stopWords?, el ï¿½ndice de palabras de parada.
?delimitadores? serï¿½ el string que contiene todos los delimitadores
utilizados por el tokenizador (campo privado ?tok?)
detectComp y minuscSinAcentos serï¿½n los parï¿½metros que se pasarï¿½n al
tokenizador
?dirIndice? serï¿½ el directorio del disco duro donde se almacenarï¿½ el
ï¿½ndice (campo privado ?directorioIndice?). Si dirIndice=?? entonces se
almacenarï¿½ en el directorio donde se ejecute el programa
?tStemmer? inicializarï¿½ la variable privada ?tipoStemmer?:
0 = no se aplica stemmer: se indexa el tï¿½rmino tal y como
aparece tokenizado
1 = stemmer de Porter para espaï¿½ol
2 = stemmer de Porter para inglï¿½s
?almEnDisco? inicializarï¿½ la variable privada ?almacenarEnDisco?
?almPosTerm? inicializarï¿½ la variable privada ?almacenarPosTerm?
Los ï¿½ndices (p.ej. ï¿½ndice, indiceDocs e informacionColeccionDocs)
quedarï¿½n vacï¿½os*/
IndexadorHash::IndexadorHash()
{
    indice = unordered_map<string, InformacionTermino>();
    indiceDocs = unordered_map<string, InfDoc>();
    informacionColeccionDocs = InfColeccionDocs();
    pregunta = "";
    indicePregunta = unordered_map<string, InformacionTerminoPregunta>();
    infPregunta = InformacionPregunta();
    stopWords = unordered_set<string>();
    ficheroStopWords = "";
    tok = Tokenizador();
    directorioIndice = "";
    tipoStemmer = 0;
    almacenarEnDisco = false;
    almacenarPosTerm = false;
}

IndexadorHash::IndexadorHash(const string &fichStopWords, const string &delimitadores,
                             const bool &detectComp, const bool &minuscSinAcentos, const string &dirIndice, const int &tStemmer, const bool &almEnDisco, const bool &almPosTerm)
{
    indice = unordered_map<string, InformacionTermino>();
    // ï¿½ndice de tï¿½rminos indexados accesible por el tï¿½rmino
    indiceDocs = unordered_map<string, InfDoc>();
    // ï¿½ndice de documentos indexados accesible por el nombre del documento
    informacionColeccionDocs = InfColeccionDocs();
    // Informaciï¿½n recogida de la colecciï¿½n de documentos indexada
    pregunta = "";
    // Pregunta indexada actualmente. Si no hay ninguna indexada, contendrï¿½a el valor ??
    indicePregunta = unordered_map<string, InformacionTerminoPregunta>();
    // ï¿½ndice de tï¿½rminos indexados en una pregunta. Se almacenarï¿½ en memoria principal
    infPregunta = InformacionPregunta();
    // Informaciï¿½n recogida de la pregunta indexada. Se almacenarï¿½ en memoria principal

    ficheroStopWords = fichStopWords;
    tipoStemmer = tStemmer;
    stopWords = unordered_set<string>();
    // Almacenar las stopWords en un unordered_set a partir del fichero de stopWords
    ifstream f(ficheroStopWords.c_str());
    if (!f)
    {
        cerr << "Error al abrir el fichero " << ficheroStopWords << endl;
    }
    else
    {
        stemmerPorter stemmer = stemmerPorter();
        string palabra;
        while (std::getline(f, palabra))
        {
            stopWords_originales.insert(palabra);
            stemmer.stemmer(palabra, tipoStemmer);
            stopWords.insert(palabra);
        }
        f.close();
    }
    tok = Tokenizador(delimitadores, detectComp, minuscSinAcentos);
    directorioIndice = dirIndice;
    almacenarEnDisco = almEnDisco;
    almacenarPosTerm = almPosTerm;
}

IndexadorHash::IndexadorHash(const IndexadorHash &orig)
{
    indice = orig.indice;
    indiceDocs = orig.indiceDocs;
    informacionColeccionDocs = orig.informacionColeccionDocs;
    pregunta = orig.pregunta;
    indicePregunta = orig.indicePregunta;
    infPregunta = orig.infPregunta;
    stopWords = orig.stopWords;
    stopWords_originales = orig.stopWords_originales;
    ficheroStopWords = orig.ficheroStopWords;
    tok = orig.tok;
    directorioIndice = orig.directorioIndice;
    tipoStemmer = orig.tipoStemmer;
    almacenarEnDisco = orig.almacenarEnDisco;
    almacenarPosTerm = orig.almacenarPosTerm;
    id_ficheros_indice = orig.id_ficheros_indice;
    indice_guardados = orig.indice_guardados;
    indiceDocs_guardados = orig.indiceDocs_guardados;
    indice_actualizar = orig.indice_actualizar;
}

IndexadorHash::~IndexadorHash()
{
    indice.clear();
    indiceDocs.clear();
    informacionColeccionDocs = InfColeccionDocs();
    pregunta = "";
    indicePregunta.clear();
    infPregunta = InformacionPregunta();
    stopWords.clear();
    stopWords_originales.clear();
    ficheroStopWords = "";
    tok = Tokenizador();
    directorioIndice = "";
    tipoStemmer = 0;
    almacenarEnDisco = false;
    almacenarPosTerm = false;
}

IndexadorHash &IndexadorHash::operator=(const IndexadorHash &orig)
{
    indice = orig.indice;
    indiceDocs = orig.indiceDocs;
    informacionColeccionDocs = orig.informacionColeccionDocs;
    pregunta = orig.pregunta;
    indicePregunta = orig.indicePregunta;
    infPregunta = orig.infPregunta;
    stopWords = orig.stopWords;
    stopWords_originales = orig.stopWords_originales;
    ficheroStopWords = orig.ficheroStopWords;
    tok = orig.tok;
    directorioIndice = orig.directorioIndice;
    tipoStemmer = orig.tipoStemmer;
    almacenarEnDisco = orig.almacenarEnDisco;
    almacenarPosTerm = orig.almacenarPosTerm;
    id_ficheros_indice = orig.id_ficheros_indice;
    indice_guardados = orig.indice_guardados;
    indiceDocs_guardados = orig.indiceDocs_guardados;
    indice_actualizar = orig.indice_actualizar;
    return *this;
}

/*Constructor para inicializar IndexadorHash a partir de una indexaciï¿½n
previamente realizada que habrï¿½ sido almacenada en
?directorioIndexacion? mediante el mï¿½todo ?bool GuardarIndexacion()?.
Con ello toda la parte privada se inicializarï¿½ convenientemente, igual
que si se acabase de indexar la colecciï¿½n de documentos. En caso que no
exista el directorio o que no contenga los datos de la indexaciï¿½n se
tratarï¿½ la excepciï¿½n correspondiente
*/
IndexadorHash::IndexadorHash(const string &directorioIndexacion)
{
    RecuperarIndexacion(directorioIndexacion);
}

ostream &operator<<(ostream &s, const IndexadorHash &p)
{
    s << "Fichero con el listado de palabras de parada: " << p.ficheroStopWords << endl;
    s << "Tokenizador: " << p.tok << endl;
    s << "Directorio donde se almacenara el indice generado: " << p.directorioIndice << endl;
    s << "Stemmer utilizado: " << p.tipoStemmer << endl;
    s << "Informacion de la coleccion indexada: " << p.informacionColeccionDocs << endl;
    s << "Se almacenara parte del indice en disco duro: " << p.almacenarEnDisco << endl;
    s << "Se almacenaran las posiciones de los terminos: " << p.almacenarPosTerm;
    return s;
}

/* Devuelve true si nomDoc existe en la colecciï¿½n y muestra por pantalla
el contenido del campo privado ?indiceDocs? para el documento con nombre
?nomDoc?: cout << nomDoc << ?\t? << InfDoc << endl; . Si no existe no se
muestra nada*/
bool IndexadorHash::ListarDocs(const string &nomDoc) const
{
    string nomDoc2 = nomDoc;
    auto it = indiceDocs.find(nomDoc);
    if (it != indiceDocs.end())
    {
        cout << it->first << '\t' << it->second << "\n";
        return true;
    }
    auto itG = indiceDocs_guardados.find(nomDoc);
    if (itG != indiceDocs_guardados.end())
    {
        // Leer el fichero
        ifstream f;
        replace(nomDoc2.begin(), nomDoc2.end(), '/', '-');
        f.open(directorioIndice + "/indiceDocs/" + nomDoc2);
        if (!f)
        {
            cerr << "Error al abrir el fichero " << nomDoc2 << endl;
            return false;
        }
        string linea;
        getline(f, linea);
        InfDoc infDoc;
        infDoc.cargar(linea);
        cout << *itG << '\t' << infDoc << "\n";
        return true;
    }
    return false;
}

/* Devuelve true si consigue crear el ï¿½ndice para la colecciï¿½n de
    documentos detallada en ficheroDocumentos, el cual contendrï¿½ un nombre
    de documento por lï¿½nea. Los aï¿½adirï¿½ a los ya existentes anteriormente en
    el ï¿½ndice.
    // Devuelve falso si no finaliza la indexaciï¿½n (p.ej. por falta de
    memoria), mostrando el mensaje de error correspondiente, indicando el
    documento y tï¿½rmino en el que se ha quedado, dejando en memoria lo que
    se haya indexado hasta ese momento.
    // En el caso que aparezcan documentos repetidos, documentos que no
    existen o que ya estuviesen previamente indexados (ha de coincidir el
    nombre del documento y el directorio en que se encuentre), se devolverï¿½
    true, mostrando el mensaje de excepciï¿½n correspondiente, y se reindexarï¿½n (borrar el documento previamente indexado e indexar el nuevo)
    en caso que la fecha de modificaciï¿½n del documento sea mï¿½s reciente que
    la almacenada previamente (class ?InfDoc? campo ?fechaModificacion?).
    Los casos de reindexaciï¿½n mantendrï¿½n el mismo idDoc.*/
bool IndexadorHash::Indexar(const string &ficheroDocumentos)
{
    ifstream f(ficheroDocumentos);
    if (!f)
    {
        cerr << "Error al abrir el fichero de documentos " << ficheroDocumentos << endl;
        return false;
    }
    bool success = true;
    int contador = 0;
    string linea;

    while (std::getline(f, linea))
    {
        // Comprobar si el documento ya estï¿½ indexado
        auto itDoc = indiceDocs.find(linea);
        if (itDoc != indiceDocs.end())
        {
            // Comprobar si el documento ha sido modificado
            struct stat atributos;
            stat(linea.c_str(), &atributos);
            time_t fechaModificacion = atributos.st_mtime;
            // Si la fecha de modificaciï¿½n es anterior a la del documento indexado, no se indexa
            if (fechaModificacion > itDoc->second.getFechaModificacion())
            {
                // Obtener el idDoc
                int idDoc = itDoc->second.getIdDoc();
                // Borrar el documento previamente indexado
                if (!BorraDocIndice(linea, idDoc, itDoc))
                    return false;
                // Indexar el nuevo manteniendo el idDoc
                if (!IndexarDoc(linea, idDoc))
                    return false;
            }
        }
        else if (indiceDocs_guardados.find(linea) != indiceDocs_guardados.end())
        {
            // Comprobar si el documento ha sido modificado
            struct stat atributos;
            stat(linea.c_str(), &atributos);
            time_t fechaModificacion = atributos.st_mtime;
            InfDoc infDocGuardado = InfDoc();
            // Abrir el fichero de indiceDocs
            ifstream i;
            string fichero = linea;
            replace(fichero.begin(), fichero.end(), '/', '-');
            i.open((directorioIndice + "/indiceDocs/" + fichero).c_str());
            if (!i)
            {
                cerr << "Error al abrir el fichero de indiceDocs " << linea << endl;
                return false;
            }
            // Leer el fichero de indiceDocs
            string line;
            if (std::getline(i, line))
            {
                infDocGuardado.cargar(line);
            }
            i.close();
            // Si la fecha de modificaciï¿½n es anterior a la del documento indexado, no se indexa
            if (fechaModificacion > infDocGuardado.getFechaModificacion())
            {
                // Obtener el idDoc
                int idDoc = infDocGuardado.getIdDoc();
                // Borrar el documento previamente indexado
                if (!BorraDocGuardado(linea, idDoc, infDocGuardado))
                    return false;
                // Indexar el nuevo manteniendo el idDoc
                if (!IndexarDoc(linea, idDoc))
                    return false;
            }
        }
        else
        {
            // Indexar el nuevo
            IndexarDoc(linea);
        }
        ++contador;
        if (almacenarEnDisco && contador % LIMITE_INDEXACION_MEMORIA == 0)
        {
            if (!AlmacenarEnDisco())
                cerr << "ERROR: al almacenar en disco" << endl;
        }
        // cout << "Indexados " << contador << " documentos" << endl;
    }
    f.close();
    if (almacenarEnDisco)
        if (!AlmacenarEnDisco())
            cerr << "ERROR: al almacenar en disco" << endl;
    return true;
}

bool IndexadorHash::IndexarDoc(const string &nomDoc, int idDoc)
{
    // Abrir el documento
    ifstream f;
    f.open(nomDoc.c_str());
    if (!f)
    {
        cerr << "Error al abrir el fichero para indexar " << nomDoc << endl;
        return false;
    }
    // Obtener el idDoc
    if (idDoc == -1)
        idDoc = indiceDocs.size() + indiceDocs_guardados.size() + 1;
    // Obtener la fecha de modificaciï¿½n
    struct stat atributos;
    stat(nomDoc.c_str(), &atributos);
    time_t fechaModificacion = atributos.st_mtime;
    vector<string> tokens;
    // Tokenizar el documento
    if (tok.Tokenizar(nomDoc, tokens))
    {
        // Documento .tk que contiene un término por línea
        /*ifstream f(nomDoc + ".tk");
        if (!f)
        {
            cerr << "Error al abrir el fichero de tokens " << nomDoc << ".tk" << endl;
            return false;
        }
        string linea;*/
        // Inicializar stemmer
        stemmerPorter stemmer = stemmerPorter();

        // Nï¿½mero de palabras del documento
        int numPalabras = 0;
        // Nï¿½mero de palabras que no son stopWords
        int numPalabrasSinStopWords = 0;
        // Nï¿½mero de palabras distintas del documento
        int numPalabrasDistintas = 0;
        // Nï¿½mero de tï¿½rminos distintos total
        int numTotalPalDiferentes = 0;

        // Palabras distintas del documento
        unordered_set<string> palabrasDistintas;
        for (auto &linea : tokens)
        {
            // Aplicar el stemmer
            stemmer.stemmer(linea, tipoStemmer);
            // Comprobar si el tï¿½rmino es una stopWord
            if (stopWords.find(linea) == stopWords.end())
            {
                numPalabrasSinStopWords++;
                // Comprobar si el tï¿½rmino estï¿½ en el ï¿½ndice
                auto it = indice.find(linea);
                if (it != indice.end())
                {
                    // Obtener la informaciï¿½n del tï¿½rmino del ï¿½ndice
                    InformacionTermino *infTerm = &(indice[linea]);
                    // Actualizar la informaciï¿½n del tï¿½rmino
                    infTerm->addFtc();
                    // Comprobar si el tï¿½rmino estï¿½ en el documento (si existe el idDoc en el map)
                    if (infTerm->existIdDoc(idDoc))
                    {
                        // Obtener la informaciï¿½n del tï¿½rmino en el documento
                        InfTermDoc *infTermDoc = infTerm->getInfTermDoc(idDoc);
                        // Actualizar la informaciï¿½n del tï¿½rmino en el documento
                        infTermDoc->addFt();
                        if (almacenarPosTerm)
                            infTermDoc->addPosTerm(numPalabras);
                    }
                    else
                    {
                        // Rellenar la informaciï¿½n del tï¿½rmino en el documento
                        InfTermDoc infTermDoc = InfTermDoc();
                        infTermDoc.addFt();
                        if (almacenarPosTerm)
                            infTermDoc.addPosTerm(numPalabras);
                        // Actualizar la informaciï¿½n del tï¿½rmino en el documento
                        infTerm->addInfTermDoc(idDoc, infTermDoc);
                    }
                }
                else
                {
                    auto itG = indice_guardados.find(linea);
                    if (itG != indice_guardados.end())
                    {
                        auto ac = indice_actualizar.find(linea);
                        if (ac != indice_actualizar.end())
                        {
                            // Obtener la informaciï¿½n del tï¿½rmino del ï¿½ndice
                            InformacionTermino *infTerm = &(indice_actualizar[linea]);
                            // Actualizar la informaciï¿½n del tï¿½rmino
                            infTerm->addFtc();
                            // Comprobar si el tï¿½rmino estï¿½ en el documento (si existe el idDoc en el map)
                            if (infTerm->existIdDoc(idDoc))
                            {
                                // Obtener la informaciï¿½n del tï¿½rmino en el documento
                                InfTermDoc *infTermDoc = infTerm->getInfTermDoc(idDoc);
                                // Actualizar la informaciï¿½n del tï¿½rmino en el documento
                                infTermDoc->addFt();
                                if (almacenarPosTerm)
                                    infTermDoc->addPosTerm(numPalabras);
                            }
                            else
                            {
                                // Rellenar la informaciï¿½n del tï¿½rmino en el documento
                                InfTermDoc infTermDoc = InfTermDoc();
                                infTermDoc.addFt();
                                if (almacenarPosTerm)
                                    infTermDoc.addPosTerm(numPalabras);
                                // Actualizar la informaciï¿½n del tï¿½rmino en el documento
                                infTerm->addInfTermDoc(idDoc, infTermDoc);
                            }
                        }
                        else
                        {
                            // Rellenar la informaciï¿½n del tï¿½rmino en el documento
                            InfTermDoc infTermDoc = InfTermDoc();
                            infTermDoc.addFt();
                            if (almacenarPosTerm)
                                infTermDoc.addPosTerm(numPalabras);
                            // Rellenar la informaciï¿½n del tï¿½rmino
                            InformacionTermino infTerm = InformacionTermino(1);
                            infTerm.addInfTermDoc(idDoc, infTermDoc);
                            // Aplicar tratamiento de mayï¿½sculas y acentos
                            if (tok.PasarAminuscSinAcentos())
                                string wordTratada = pasar_a_minusculas_sin_acentos(linea);
                            // Aplicar stemming
                            stemmerPorter stemmer = stemmerPorter();
                            string term = linea;
                            stemmer.stemmer(term, tipoStemmer);
                            // Aï¿½adir el tï¿½rmino al ï¿½ndice
                            indice_actualizar.insert(pair<string, InformacionTermino>(term, infTerm));
                        }
                    }
                    else
                    {
                        numTotalPalDiferentes++;
                        // Rellenar la informaciï¿½n del tï¿½rmino en el documento
                        InfTermDoc infTermDoc = InfTermDoc();
                        infTermDoc.addFt();
                        if (almacenarPosTerm)
                            infTermDoc.addPosTerm(numPalabras);
                        // Rellenar la informaciï¿½n del tï¿½rmino
                        InformacionTermino infTerm = InformacionTermino(1);
                        infTerm.addInfTermDoc(idDoc, infTermDoc);
                        // Aï¿½adir el tï¿½rmino al ï¿½ndice
                        indice.insert(pair<string, InformacionTermino>(linea, infTerm));
                    }
                }

                // Comprobar si la palabra es ï¿½nica en el documento
                if (palabrasDistintas.find(linea) == palabrasDistintas.end())
                {
                    // Aï¿½adir la palabra al conjunto de palabras distintas del documento
                    palabrasDistintas.insert(linea);
                    numPalabrasDistintas++;
                }
            }
            numPalabras++;
        }
        f.close();
        // Obtener fecha de modificaciï¿½n del documento y tamaï¿½o en bytes
        struct stat atributos;
        stat(nomDoc.c_str(), &atributos);
        int tamBytes = atributos.st_size;
        time_t fechaModificacion = atributos.st_mtime;
        // Aï¿½adir el documento al ï¿½ndice de documentos
        indiceDocs.insert(make_pair(nomDoc, InfDoc(informacionColeccionDocs.getNumDocs() + 1,
                                                   numPalabras, numPalabrasSinStopWords, numPalabrasDistintas,
                                                   tamBytes, fechaModificacion)));
        // Actualizar la informaciï¿½n de la colecciï¿½n de documentos
        informacionColeccionDocs.addDoc();
        informacionColeccionDocs.addNumTotalPal(numPalabras);
        informacionColeccionDocs.addNumTotalPalSinParada(numPalabrasSinStopWords);
        informacionColeccionDocs.addNumTotalPalDiferentes(numTotalPalDiferentes);
        informacionColeccionDocs.addTamBytes(tamBytes);
    }
    else
    {
        cerr << "Error al tokenizar el documento " << nomDoc << endl;
        return false;
    }
    return true;
}

/* Devuelve true si nomDoc estï¿½ indexado y se realiza el borrado de
todos los tï¿½rminos del documento y del documento en los campos privados
?indiceDocs? e ?informacionColeccionDocs?*/
bool IndexadorHash::BorraDoc(const string &nomDoc)
{
    // Comprobar si el documento estï¿½ indexado
    auto itDoc = indiceDocs.find(nomDoc);
    if (itDoc != indiceDocs.end())
    {
        // Obtener el idDoc
        int idDoc = itDoc->second.getIdDoc();
        if (BorraDocIndice(nomDoc, idDoc, itDoc))
            return true;
    }
    auto itDoc2 = indiceDocs_guardados.find(nomDoc);
    if (itDoc2 != indiceDocs_guardados.end())
    {
        InfDoc infDocGuardado = InfDoc();
        // Abrir el fichero de indiceDocs
        ifstream i;
        string fichero = nomDoc;
        replace(fichero.begin(), fichero.end(), '/', '-');
        i.open((directorioIndice + "/indiceDocs/" + fichero).c_str());
        if (!i)
        {
            cerr << "Error al abrir el fichero de indiceDocs " << nomDoc << endl;
            return false;
        }
        // Leer el fichero de indiceDocs
        string line;
        if (std::getline(i, line))
        {
            infDocGuardado.cargar(line);
        }
        i.close();
        // Obtener el idDoc
        int idDoc = infDocGuardado.getIdDoc();
        if (BorraDocGuardado(fichero, idDoc, infDocGuardado))
            return true;
    }
    return false;
}

bool IndexadorHash::BorraDocIndice(const string &nomDoc, const int &idDoc, const std::unordered_map<std::string, InfDoc>::iterator &itDoc)
{
    // Actualizar la informaciï¿½n de la colecciï¿½n de documentos
    informacionColeccionDocs.subDoc();
    informacionColeccionDocs.subNumTotalPal(itDoc->second.getNumPal());
    informacionColeccionDocs.subNumTotalPalSinParada(itDoc->second.getNumPalSinParada());
    informacionColeccionDocs.subTamBytes(itDoc->second.getTamBytes());
    // Borrar el documento del ï¿½ndice de documentos
    indiceDocs.erase(nomDoc);
    // Borrar los tï¿½rminos del documento del ï¿½ndice
    auto it = indice.begin();
    while (it != indice.end())
    {
        // Comprobar si el tï¿½rmino estï¿½ en el documento
        if (it->second.existIdDoc(idDoc))
        {
            // Borrar el tï¿½rmino del documento
            it->second.eraseInfTermDoc(idDoc);
            // Comprobar si el tï¿½rmino estï¿½ en otros documentos
            if (it->second.getNumDocs() <= 0)
            {
                // Borrar el tï¿½rmino del ï¿½ndice
                it = indice.erase(it);
                informacionColeccionDocs.subNumTotalPalDiferentes();
                continue;
            }
        }
        it++;
    }
    // Borramos los tï¿½rminos del documento del ï¿½ndice guardado
    auto itTerm = indice_guardados.begin();
    while (itTerm != indice_guardados.end())
    {
        // Obtener la informaciï¿½n del tï¿½rmino
        InformacionTermino infTerm = InformacionTermino();
        // Abrir el fichero del tï¿½rmino
        ifstream f;
        string fichero = directorioIndice + "/indiceDocs/" + to_string(itTerm->second);
        f.open(fichero.c_str());
        if (!f.is_open())
        {
            cerr << "Error al abrir el fichero del tï¿½rmino " << fichero << endl;
            return false;
        }
        // Leer la informaciï¿½n del tï¿½rmino
        string linea;
        if (std::getline(f, linea))
        {
            infTerm.cargar(linea);
        }
        else
        {
            cerr << "Error al leer la informaciï¿½n del tï¿½rmino " << fichero << endl;
            return false;
        }
        // Cerrar el fichero del tï¿½rmino
        f.close();
        // Comprobar si el tï¿½rmino estï¿½ en el documento
        if (infTerm.existIdDoc(idDoc))
        {
            // Borrar el tï¿½rmino del documento
            infTerm.eraseInfTermDoc(idDoc);
            // Comprobar si el tï¿½rmino estï¿½ en otros documentos
            if (infTerm.getNumDocs() > 0)
            {
                // Actualizar el tï¿½rmino en el ï¿½ndice sobreescribiendo el fichero
                ofstream f;
                f.open(fichero.c_str());
                if (!f.is_open())
                {
                    cerr << "Error al abrir el fichero del tï¿½rmino " << fichero << endl;
                    return false;
                }
                // Escribir la informaciï¿½n del tï¿½rmino
                f << infTerm;
                // Cerrar el fichero del tï¿½rmino
                f.close();
            }
            else
            {
                // Borrar el tï¿½rmino del ï¿½ndice
                itTerm = indice_guardados.erase(itTerm);
                informacionColeccionDocs.subNumTotalPalDiferentes();
                // Borrar el fichero del tï¿½rmino
                if (std::remove(fichero.c_str()) != 0)
                {
                    cerr << "Error al borrar el fichero del tï¿½rmino " << fichero << endl;
                    return false;
                }
                continue;
            }
        }
        ++it;
    }
    return true;
}

bool IndexadorHash::BorraDocGuardado(string &nomDoc, const int &idDoc, const InfDoc &infDoc)
{
    // Borrar el documento del ï¿½ndice de documentos
    indiceDocs_guardados.erase(nomDoc);
    string fichero = nomDoc;
    replace(fichero.begin(), fichero.end(), '/', '-');
    // Borrar el fichero del documento
    remove((directorioIndice + "/indiceDocs/" + fichero).c_str());
    // Actualizar la informaciï¿½n de la colecciï¿½n de documentos
    informacionColeccionDocs.subDoc();
    informacionColeccionDocs.subNumTotalPal(infDoc.getNumPal());
    informacionColeccionDocs.subNumTotalPalSinParada(infDoc.getNumPalSinParada());
    informacionColeccionDocs.subTamBytes(infDoc.getTamBytes());
    // Borrar los tï¿½rminos del documento del ï¿½ndice
    auto it = indice.begin();
    while (it != indice.end())
    {
        // Obtener la informaciï¿½n del tï¿½rmino
        InformacionTermino infTerm = it->second;
        // Comprobar si el tï¿½rmino estï¿½ en el documento
        if (infTerm.existIdDoc(idDoc))
        {
            // Borrar el tï¿½rmino del documento
            infTerm.eraseInfTermDoc(idDoc);
            // Comprobar si el tï¿½rmino estï¿½ en otros documentos
            if (infTerm.getNumDocs() > 0)
            {
                // Actualizar el tï¿½rmino en el ï¿½ndice
                indice[it->first] = infTerm;
            }
            else
            {
                // Borrar el tï¿½rmino del ï¿½ndice
                it = indice.erase(it);
                informacionColeccionDocs.subNumTotalPalDiferentes();

                continue;
            }
        }
        it++;
    }
    // Borramos los tï¿½rminos del documento del ï¿½ndice guardado
    auto itTerm = indice_guardados.begin();
    while (itTerm != indice_guardados.end())
    {
        // Obtener la informaciï¿½n del tï¿½rmino
        InformacionTermino infTerm = InformacionTermino();
        // Abrir el fichero del tï¿½rmino
        ifstream f;
        string fichero = directorioIndice + "/indiceDocs/" + to_string(itTerm->second);
        f.open(fichero.c_str());
        if (!f.is_open())
        {
            cerr << "Error al abrir el fichero del tï¿½rmino " << fichero << endl;
            return false;
        }
        // Leer la informaciï¿½n del tï¿½rmino
        string linea;
        if (std::getline(f, linea))
        {
            infTerm.cargar(linea);
        }
        else
        {
            cerr << "Error al leer la informaciï¿½n del tï¿½rmino " << fichero << endl;
            return false;
        }
        // Cerrar el fichero del tï¿½rmino
        f.close();
        // Comprobar si el tï¿½rmino estï¿½ en el documento
        if (infTerm.existIdDoc(idDoc))
        {
            // Borrar el tï¿½rmino del documento
            infTerm.eraseInfTermDoc(idDoc);
            // Comprobar si el tï¿½rmino estï¿½ en otros documentos
            if (infTerm.getNumDocs() > 0)
            {
                // Actualizar el tï¿½rmino en el ï¿½ndice sobreescribiendo el fichero
                ofstream f;
                f.open(fichero.c_str());
                if (!f.is_open())
                {
                    cerr << "Error al abrir el fichero del tï¿½rmino " << fichero << endl;
                    return false;
                }
                // Escribir la informaciï¿½n del tï¿½rmino
                f << infTerm;
                // Cerrar el fichero del tï¿½rmino
                f.close();
            }
            else
            {
                // Borrar el tï¿½rmino del ï¿½ndice
                itTerm = indice_guardados.erase(itTerm);
                informacionColeccionDocs.subNumTotalPalDiferentes();
                // Borrar el fichero del tï¿½rmino
                if (std::remove(fichero.c_str()) != 0)
                {
                    cerr << "Error al borrar el fichero del tï¿½rmino " << fichero << endl;
                    return false;
                }
                continue;
            }
        }
        ++it;
    }
    return true;
}

/* Mostrarï¿½ por pantalla las palabras de parada almacenadas (originales,
sin aplicar stemming): una palabra por lï¿½nea (salto de lï¿½nea al final de
cada palabra) Las stopWords estï¿½n en la variable privada unordered_set stopWords*/
void IndexadorHash::ListarPalParada() const
{
    for (unordered_set<string>::const_iterator it = stopWords_originales.begin(); it != stopWords_originales.end(); ++it)
        cout << *it << "\n";
}

/* Se guardarï¿½ en disco duro (directorio contenido en la variable
privada ?directorioIndice?) la indexaciï¿½n actualmente en memoria
(incluidos todos los parï¿½metros de la parte privada). La forma de
almacenamiento la determinarï¿½ el alumno. El objetivo es que esta
indexaciï¿½n se pueda recuperar posteriormente mediante el constructor
?IndexadorHash(const string& directorioIndexacion)?. Por ejemplo,
supongamos que se ejecuta esta secuencia de comandos: ?IndexadorHash
a(?./fichStopWords.txt?, ?[ ,.?, ?./dirIndexPrueba?, 0, false);
a.Indexar(?./fichConDocsAIndexar.txt?); a.GuardarIndexacion();?,
entonces mediante el comando: ?IndexadorHash b(?./dirIndexPrueba?);? se
recuperarï¿½ la indexaciï¿½n realizada en la secuencia anterior, cargï¿½ndola
en ?b?
// Devuelve falso si no finaliza la operaciï¿½n (p.ej. por falta de
memoria, o el nombre del directorio contenido en ?directorioIndice? no
es correcto), mostrando el mensaje de error correspondiente, vaciando
los ficheros generados.
// En caso que no existiese el directorio directorioIndice, habrï¿½a que
crearlo previamente
*/
bool IndexadorHash::GuardarIndexacion() const
{
    // Comprobar si el directorio existe y si no crearlo
    if (mkdir(directorioIndice.c_str(), 0777) == -1 && errno != EEXIST)
    {
        cerr << "Error al crear el directorio " << directorioIndice << "\n";
        return false;
    }

    // Crear el fichero
    ofstream f(directorioIndice + "/indexador.dat");
    f.exceptions(std::ios::failbit);
    try
    {
        if (!f)
        {
            cerr << "Error al abrir el fichero " << directorioIndice + "/indexador.dat"
                 << "\n";
            return false;
        }
        // Guardar el ununordered_map indice
        f << indice.size() << "\n";
        for (auto it = indice.begin(); it != indice.end(); ++it)
        {
            f << it->first << "\n";
            f << it->second << "\n";
        }
        // Guardar el unordered_map indiceDocs
        f << indiceDocs.size() << "\n";
        for (auto it = indiceDocs.begin(); it != indiceDocs.end(); ++it)
        {
            f << it->first << "\n";
            f << it->second << "\tfechaModificacion: " << it->second.getFechaModificacion() << "\n";
        }
        // Guardar el unordered_map indicePregunta
        f << indicePregunta.size() << "\n";
        for (auto it = indicePregunta.begin(); it != indicePregunta.end(); ++it)
        {
            f << it->first << "\n";
            f << it->second << "\n";
        }
        // Guardar el tipoStemmer
        f << tipoStemmer << "\n";
        // Guardar el unordered_set stopWords_originales
        f << stopWords_originales.size() << "\n";
        for (auto it = stopWords_originales.begin(); it != stopWords_originales.end(); ++it)
        {
            f << *it << "\n";
        }
        // Guardar la informacionColeccionDocs
        f << informacionColeccionDocs << "\n";
        // Guardar la pregunta
        f << pregunta << "\n";
        // Guardar la informacionPregunta
        f << infPregunta << "\n";
        // Guardar el ficheroStopWords
        f << ficheroStopWords << "\n";
        // Guardar el tokenizador
        f << tok << "\n";
        // Guardar el directorioIndice
        f << directorioIndice << "\n";
        // Guardar el almacenarEnDisco
        f << almacenarEnDisco << "\n";
        // Guardar el almacenarPosTerm
        f << almacenarPosTerm;
    }
    catch (const std::ios::failure &e)
    {
        cerr << "Error al escribir en el fichero " << directorioIndice + "/indexador.dat: " << e.what() << "\n";
        // Borrar el fichero
        remove((directorioIndice + "/indexador.dat").c_str());
        return false;
    }
    f.close();
    return true;
}

// Funciï¿½n auxiliar para almacenar en disco
bool IndexadorHash::AlmacenarEnDisco()
{
    // Comprobar si el directorio existe y si no crearlo
    if (mkdir(directorioIndice.c_str(), 0777) == -1 && errno != EEXIST)
    {
        cerr << "Error al crear el directorio " << directorioIndice << "\n";
        return false;
    }
    // Guardar el unordered_map indice
    // Creamos una carpeta donde se guardaran los ficheros de los terminos. Los ficheros tendrï¿½n el nombre del termino y contendrï¿½n la informacion del termino
    if (mkdir((directorioIndice + "/indice").c_str(), 0777) == -1 && errno != EEXIST)
    {
        cerr << "Error al crear el directorio " << directorioIndice + "/indice"
             << "\n";
        return false;
    }
    auto it = indice.begin();
    while (it != indice.end())
    {
        ofstream f(directorioIndice + "/indice/" + to_string(id_ficheros_indice));
        f.exceptions(std::ios::failbit);
        try
        {
            if (!f)
            {
                cerr << "Error al abrir el fichero " << directorioIndice + "/indice/" + to_string(id_ficheros_indice)
                     << "\n";
                return false;
            }
            f << it->second;
        }
        catch (const std::ios::failure &e)
        {
            cerr << "Error al escribir en el fichero " << directorioIndice + "/indice/" + to_string(id_ficheros_indice) << ": " << e.what() << "\n";
            // Borrar el fichero
            remove((directorioIndice + "/indice/" + to_string(id_ficheros_indice)).c_str());
            return false;
        }
        f.close();
        // Borrar el termino del indice y guardar el nombre en el set indice_guardados
        indice_guardados.insert(pair<string, int>(it->first, id_ficheros_indice));
        ++id_ficheros_indice;
        it = indice.erase(it);
    }
    // Recorrer el indice de actualizaciones y guardar los terminos en el fichero correspondiente
    auto it2 = indice_actualizar.begin();
    while (it2 != indice_actualizar.end())
    {
        InformacionTermino inf;
        ifstream f(directorioIndice + "/indice/" + to_string(indice_guardados[it2->first]));
        f.exceptions(std::ios::failbit);
        try
        {
            if (!f)
            {
                cerr << "Error al abrir el fichero " << directorioIndice + "/indice/" + to_string(indice_guardados[it2->first])
                     << "\n";
                return false;
            }
            string linea;
            getline(f, linea);
            inf.cargar(linea);
        }
        catch (const std::ios::failure &e)
        {
            cerr << "Error al escribir en el fichero " << directorioIndice + "/indice/" + to_string(indice_guardados[it2->first]) << ": " << e.what() << "\n";
            // Borrar el fichero
            remove((directorioIndice + "/indice/" + to_string(indice_guardados[it2->first])).c_str());
            return false;
        }
        f.close();
        // Actualizar el termino sumando la informacion del indice_actualizar
        inf.setFtc(it2->second.getFtc() + inf.getFtc());
        // Actualizar l_docs
        unordered_set<int> l_docs_actualizar;
        it2->second.getIdDocs(l_docs_actualizar);
        for (auto item : l_docs_actualizar)
        {
            if (inf.existIdDoc(item))
            {
                InfTermDoc *infTermDoc = inf.getInfTermDoc(item);
                infTermDoc->setFt(infTermDoc->getFt() + it2->second.getInfTermDoc(item)->getFt());
                if (almacenarPosTerm)
                {
                    list<int> *posTerm = it2->second.getInfTermDoc(item)->getPosTerm();
                    for (auto item2 : *posTerm)
                    {
                        infTermDoc->addPosTerm(item2);
                    }
                }
            }
            else
            {
                InfTermDoc infTermDoc = InfTermDoc();
                infTermDoc.setFt(it2->second.getInfTermDoc(item)->getFt());
                if (almacenarPosTerm)
                {
                    infTermDoc.setPosTerm(*(it2->second.getInfTermDoc(item)->getPosTerm()));
                }
                inf.addInfTermDoc(item, infTermDoc);
            }
        }
        // Escribir la informacion del termino en el fichero
        ofstream f2(directorioIndice + "/indice/" + to_string(indice_guardados[it2->first]));
        f2.exceptions(std::ios::failbit);
        try
        {
            if (!f2)
            {
                cerr << "Error al abrir el fichero " << directorioIndice + "/indice/" + to_string(indice_guardados[it2->first])
                     << "\n";
                return false;
            }
            f2 << inf;
        }
        catch (const std::ios::failure &e)
        {
            cerr << "Error al escribir en el fichero " << directorioIndice + "/indice/" + to_string(indice_guardados[it2->first]) << ": " << e.what() << "\n";
            // Borrar el fichero
            remove((directorioIndice + "/indice/" + to_string(indice_guardados[it2->first])).c_str());
            return false;
        }
        f2.close();
        // Borrar el termino del indice_actualizar
        it2 = indice_actualizar.erase(it2);
    }
    // Guardar el unordered_map indiceDocs
    // Creamos una carpeta donde se guardaran los ficheros de los documentos. Los ficheros tendrï¿½n el nombre del documento y contendrï¿½n la informacion del documento
    if (mkdir((directorioIndice + "/indiceDocs").c_str(), 0777) == -1 && errno != EEXIST)
    {
        cerr << "Error al crear el directorio " << directorioIndice + "/indiceDocs"
             << "\n";
        return false;
    }
    auto itDocs = indiceDocs.begin();
    while (itDocs != indiceDocs.end())
    {
        string fichero = itDocs->first;
        replace(fichero.begin(), fichero.end(), '/', '-');
        ofstream f((directorioIndice + "/indiceDocs/" + fichero).c_str());
        if (f.fail())
        {
            cerr << "Error al abrir el fichero " << directorioIndice + "/indiceDocs/" + itDocs->first
                 << "\n";
            return false;
        }
        f.exceptions(std::ios::failbit);
        try
        {
            if (!f)
            {
                cerr << "Error al abrir el fichero " << directorioIndice + "/indiceDocs/" + itDocs->first
                     << "\n";
                return false;
            }
            f << itDocs->second << "\tfechaModificacion: " << itDocs->second.getFechaModificacion();
        }
        catch (const std::ios::failure &e)
        {
            cerr << "Error al escribir en el fichero " << directorioIndice + "/indiceDocs/" + itDocs->first << ": " << e.what() << "\n";
            // Borrar el fichero
            std::remove((directorioIndice + "/indiceDocs/" + fichero).c_str());
            return false;
        }
        f.close();
        // Borrar el doc del indice y guardar el nombre en el set indiceDocs_guardados
        indiceDocs_guardados.insert(itDocs->first);
        itDocs = indiceDocs.erase(itDocs);
    }
    return true;
}

bool IndexadorHash::Devuelve(const string &word, const string &nomDoc, InfTermDoc &infDoc)
{
    InformacionTermino inf;
    int idDoc;
    auto itDocs = indiceDocs.find(nomDoc);
    if (itDocs != indiceDocs.end())
    {
        idDoc = itDocs->second.getIdDoc();
        return Devuelve(word, inf) ? inf.getInfTermDoc(idDoc, infDoc) : false;
    }
    string fichero = nomDoc;
    replace(fichero.begin(), fichero.end(), '/', '-');
    if (indiceDocs_guardados.find(nomDoc) != indiceDocs_guardados.end())
    {
        ifstream f((directorioIndice + "/indiceDocs/" + fichero).c_str());
        if (f.fail())
        {
            cerr << "Error al abrir el fichero " << directorioIndice + "/indiceDocs/" + fichero
                 << "\n";
            return false;
        }
        f.exceptions(std::ios::failbit);
        try
        {
            if (!f)
            {
                cerr << "Error al abrir el fichero " << directorioIndice + "/indiceDocs/" + fichero
                     << "\n";
                return false;
            }
            InfDoc infDocAux = InfDoc();
            string line;
            if (std::getline(f, line))
            {
                infDocAux.cargar(line);
                idDoc = infDocAux.getIdDoc();
            }
            else
                return false;
        }
        catch (const std::ios::failure &e)
        {
            cerr << "Error al leer en el fichero " << directorioIndice + "/indiceDocs/" + fichero << ": " << e.what() << "\n";
            return false;
        }
        f.close();
    }
    else
        return false;
    return Devuelve(word, inf) ? inf.getInfTermDoc(idDoc, infDoc) : false;
}

void IndexadorHash::ImprimirIndexacion() const
{
    cout << "Terminos indexados: " << endl;
    /* A continuaciï¿½n aparecerï¿½ un listado del contenido del campo
    privado ?ï¿½ndice? donde para cada tï¿½rmino indexado se imprimirï¿½: */
    for (unordered_map<string, InformacionTermino>::const_iterator it = indice.begin(); it != indice.end(); ++it)
        cout << it->first << "\t" << it->second << endl;
    // cout << termino << "\t" << InformacionTermino << endl;
    // cout << "Documentos indexados: " << endl;
    /* A continuaciï¿½n aparecerï¿½ un listado del contenido del campo
    privado ?indiceDocs? donde para cada documento indexado se imprimirï¿½:*/
    for (unordered_map<string, InfDoc>::const_iterator it = indiceDocs.begin(); it != indiceDocs.end(); ++it)
        cout << it->first << "\t" << it->second << endl;
    // cout << nomDoc << "\t" << InfDoc << endl;
}

/*// Devuelve true si consigue crear el ï¿½ndice para la colecciï¿½n de
documentos que se encuentra en el directorio (y subdirectorios que
contenga) dirAIndexar (independientemente de la extensiï¿½n de los
mismos). Se considerarï¿½ que todos los documentos del directorio serï¿½n
ficheros de texto. Los aï¿½adirï¿½ a los ya existentes anteriormente en el
ï¿½ndice.
// Devuelve falso si no finaliza la indexaciï¿½n (p.ej. por falta de
memoria o porque no exista ?dirAIndexar?), mostrando el mensaje de error
correspondiente, indicando el documento y tï¿½rmino en el que se ha
quedado, dejando en memoria lo que se haya indexado hasta ese momento.
// En el caso que aparezcan documentos repetidos o que ya estuviesen
previamente indexados (ha de coincidir el nombre del documento y el
directorio en que se encuentre), se mostrarï¿½ el mensaje de excepciï¿½n
correspondiente, y se re-indexarï¿½n (borrar el documento previamente
indexado e indexar el nuevo) en caso que la fecha de modificaciï¿½n del
documento sea mï¿½s reciente que la almacenada previamente (class ?InfDoc?
campo ?fechaModificacion?). Los casos de reindexaciï¿½n mantendrï¿½n el
mismo idDoc.*/
bool IndexadorHash::IndexarDirectorio(const string &dirAIndexar)
{
    char *_emergencyMemory = new char[16384];
    string documentoActual;
    string listaFicheros = ".lista_fich";
    try
    {
        struct stat dir;
        // Compruebo la existencia del directorio
        int err = stat(dirAIndexar.c_str(), &dir);
        if (err == -1 || !S_ISDIR(dir.st_mode))
        {
            cerr << "ERROR: No existe el directorio: " << dirAIndexar << endl;
            return false;
        }
        else
        {
            // Hago una lista en un fichero con find>fich
            string cmd = "find " + dirAIndexar + " -follow |sort > .lista_fich";
            std::system(cmd.c_str());
            // Elimino la primera linea del fichero
            cmd = "sed -i '1d' .lista_fich";
            std::system(cmd.c_str());
            Indexar(listaFicheros);
        }
        // Borramos la memoria reservada
        delete[] _emergencyMemory;
        return true;
    }
    catch (bad_alloc &ex)
    {
        // Delete the reserved memory so we can print an error message before exiting
        delete[] _emergencyMemory;

        cerr << "Out of memory while indexing " << documentoActual << endl;
        return false;
    }
    catch (exception &ex)
    {
        // Delete the reserved memory so we can print an error message before exiting
        delete[] _emergencyMemory;

        cerr << "No se ha podido indexar: " << ex.what() << endl;
        return false;
    }
}

/*// Devuelve true si consigue crear el ï¿½ndice para la pregunta ?preg?.
Antes de realizar la indexaciï¿½n vaciarï¿½ los campos privados
indicePregunta e infPregunta
// Generarï¿½ la misma informaciï¿½n que en la indexaciï¿½n de documentos,
pero dejï¿½ndola toda accesible en memoria principal (mediante las
variables privadas ?pregunta, indicePregunta, infPregunta?)
// Devuelve falso si no finaliza la operaciï¿½n (p.ej. por falta de
memoria o bien si la pregunta no contiene ningï¿½n tï¿½rmino con contenido),
mostrando el mensaje de error correspondiente
*/
bool IndexadorHash::IndexarPregunta(const string &preg)
{
    char *_emergencyMemory = new char[16384];
    try
    {
        // Limpiar los campos privados
        indicePregunta.clear();
        infPregunta = InformacionPregunta();
        pregunta = preg;
        // Tokenizar la pregunta
        list<string> tokens;
        tok.Tokenizar(preg, tokens);
        // Indexar la pregunta
        int pos = 0;
        int numTotalPal = 0;
        int numTotalPalSinParada = 0;
        int numTotalPalDiferentes = 0;
        stemmerPorter stemmer = stemmerPorter();
        for (string term : tokens)
        {
            // Aplicar stemming
            stemmer.stemmer(term, tipoStemmer);
            // Comprobar si el tï¿½rmino es una stopword
            if (stopWords.find(term) == stopWords.end())
            {
                ++numTotalPalSinParada;
                // Comprobar si el tï¿½rmino estï¿½ en el ï¿½ndice
                if (indicePregunta.find(term) == indicePregunta.end())
                {
                    ++numTotalPalDiferentes;
                    // Crear la InformacionTerminoPregunta
                    InformacionTerminoPregunta infTerminoPregunta;
                    infTerminoPregunta.addFt();
                    if (almacenarPosTerm)
                        infTerminoPregunta.addPosTerm(pos);
                    // Aï¿½adir el tï¿½rmino al ï¿½ndice
                    indicePregunta.insert(pair<string, InformacionTerminoPregunta>(term, infTerminoPregunta));
                }
                else
                {
                    // Aumentar el ft del tï¿½rmino
                    indicePregunta.find(term)->second.addFt();
                    if (almacenarPosTerm)
                        indicePregunta.find(term)->second.addPosTerm(pos);
                }
            }
            ++pos;
            ++numTotalPal;
        }
        // Rellenar la InformacionPregunta
        infPregunta.setNumTotalPal(numTotalPal);
        infPregunta.setNumTotalPalSinParada(numTotalPalSinParada);
        infPregunta.setNumTotalPalDiferentes(numTotalPalDiferentes);

        // Borramos la memoria reservada
        delete[] _emergencyMemory;
        return true;
    }
    catch (bad_alloc &ex)
    {
        // Delete the reserved memory so we can print an error message before exiting
        delete[] _emergencyMemory;

        cerr << "Out of memory while indexing the question" << endl;
        return false;
    }
    catch (exception &ex)
    {
        // Delete the reserved memory so we can print an error message before exiting
        delete[] _emergencyMemory;

        cerr << "No se ha podido indexar la pregunta: " << ex.what() << endl;
        return false;
    }
}

bool IndexadorHash::Devuelve(const string &word, InformacionTermino &inf)
{
    // Aplicar tratamiento de mayï¿½sculas y acentos
    if (tok.PasarAminuscSinAcentos())
        string wordTratada = pasar_a_minusculas_sin_acentos(word);
    // Aplicar stemming
    stemmerPorter stemmer = stemmerPorter();
    string term = word;
    stemmer.stemmer(term, tipoStemmer);
    if (!almacenarEnDisco)
        return Existe(term) ? inf = indice.at(term), true : false;
    else
    {
        auto itG = indice_guardados.find(term);
        if (itG != indice_guardados.end())
        {
            // Abrir el fichero
            ifstream f;
            string fichero = directorioIndice + "/indice/" + to_string(itG->second);
            f.open(fichero.c_str());
            if (!f.is_open())
            {
                cerr << "No se ha podido abrir el fichero " << fichero << endl;
                return false;
            }
            // Leer el fichero
            string linea;
            if (std::getline(f, linea))
            {
                inf.cargar(linea);
                return true;
            }
            else
            {
                cerr << "No se ha podido leer el fichero " << fichero << endl;
                return false;
            }
        }
        else
        {
            auto it = indice.find(term);
            if (it != indice.end())
            {
                inf = it->second;
                return true;
            }
            else
                return false;
        }
    }
}

bool IndexadorHash::Devuelve(const string &word, InformacionTermino * * inf)
{
    // Aplicar tratamiento de mayï¿½sculas y acentos
    if (tok.PasarAminuscSinAcentos())
        string wordTratada = pasar_a_minusculas_sin_acentos(word);
    // Aplicar stemming
    stemmerPorter stemmer = stemmerPorter();
    string term = word;
    stemmer.stemmer(term, tipoStemmer);
    if (!almacenarEnDisco)
        return Existe(term) ? *inf = &(indice.at(term)), true : false;
    else
    {
        auto itG = indice_guardados.find(term);
        if (itG != indice_guardados.end())
        {
            // Abrir el fichero
            ifstream f;
            string fichero = directorioIndice + "/indice/" + to_string(itG->second);
            f.open(fichero.c_str());
            if (!f.is_open())
            {
                cerr << "No se ha podido abrir el fichero " << fichero << endl;
                return false;
            }
            // Leer el fichero
            string linea;
            if (std::getline(f, linea))
            {
                (*inf)->cargar(linea);
                return true;
            }
            else
            {
                cerr << "No se ha podido leer el fichero " << fichero << endl;
                return false;
            }
        }
        else
        {
            auto it = indice.find(term);
            if (it != indice.end())
            {
                *inf = &(it->second);
                return true;
            }
            else
                return false;
        }
    }
}

// Devuelve true si word (aplicï¿½ndole el tratamiento de stemming y mayï¿½sculas correspondiente) aparece como tï¿½rmino indexado
bool IndexadorHash::Existe(const string &word)
{
    // Comprobar si el tï¿½rmino estï¿½ en el ï¿½ndice
    if (indice.find(word) != indice.end())
        return true;
    else if (indice_guardados.find(word) != indice_guardados.end())
        return true;
    else
        return false;
}

string IndexadorHash::pasar_a_minusculas_sin_acentos(const string &str) const
{
    static const unsigned char acentos[256] = {
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ','!','"','#','$','%','&',' ',
        '(',')','*','+',',','-','.','/','0','1',
        '2','3','4','5','6','7','8','9',':',';',
        '<','=','>','?','@','a','b','c','d','e',
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
}

/* Serï¿½ true si se realiza la inserciï¿½n (p.ej. si word, aplicï¿½ndole el
tratamiento de stemming y mayï¿½sculas correspondiente, no estaba
previamente indexado)*/
bool IndexadorHash::Inserta(const string &word, const InformacionTermino &inf)
{
    string wordTratada = word;
    // Aplicar tratamiento de mayï¿½sculas y acentos
    if (tok.PasarAminuscSinAcentos())
        wordTratada = pasar_a_minusculas_sin_acentos(wordTratada);
    // Aplicar stemming
    stemmerPorter stemmer = stemmerPorter();
    string term = wordTratada;
    stemmer.stemmer(term, tipoStemmer);
    // Comprobar si el tï¿½rmino estï¿½ en el ï¿½ndice
    if (indiceDocs.find(term) == indiceDocs.end())
    {
        // Aï¿½adir el tï¿½rmino al ï¿½ndice
        indice.insert(pair<string, InformacionTermino>(term, inf));
        return true;
    }
    else
        return false;
}

/*// Devuelve true si hay una pregunta indexada (con al menos un tï¿½rmino
que no sea palabra de parada, o sea, que haya algï¿½n tï¿½rmino indexado en
indicePregunta), devolviï¿½ndo ?pregunta? en ?preg?
*/
bool IndexadorHash::DevuelvePregunta(string &preg) const
{
    if (indicePregunta.size() > 0)
    {
        preg = pregunta;
        return true;
    }
    else
        return false;
}

/*// Devuelve true si word (aplicï¿½ndole el tratamiento de stemming y
mayï¿½sculas correspondiente) estï¿½ indexado en la pregunta, devolviendo su
informaciï¿½n almacenada ?inf?. En caso que no estï¿½, devolverï¿½a ?inf?
vacï¿½o*/
bool IndexadorHash::DevuelvePregunta(const string &word, InformacionTerminoPregunta &inf) const
{
    // Aplicar tratamiento de mayï¿½sculas y acentos
    string wordTratada = word;
    if (tok.PasarAminuscSinAcentos())
        wordTratada = pasar_a_minusculas_sin_acentos(wordTratada);
    // Aplicar stemming
    stemmerPorter stemmer = stemmerPorter();
    string term = wordTratada;
    stemmer.stemmer(term, tipoStemmer);
    // Comprobar si el tï¿½rmino estï¿½ en el ï¿½ndice
    if (indicePregunta.find(term) != indicePregunta.end())
    {
        inf = InformacionTerminoPregunta(indicePregunta.find(term)->second);
        return true;
    }
    else
    {
        inf = InformacionTerminoPregunta();
        return false;
    }
}

/*// Devuelve true si hay una pregunta indexada, devolviendo su
informaciï¿½n almacenada (campo privado ?infPregunta?) en ?inf?. En caso
que no estï¿½, devolverï¿½a ?inf? vacï¿½o*/
bool IndexadorHash::DevuelvePregunta(InformacionPregunta &inf) const
{
    if (indicePregunta.size() > 0)
    {
        inf = InformacionPregunta(infPregunta);
        return true;
    }
    else
    {
        inf = InformacionPregunta();
        return false;
    }
}

/*// Vacï¿½a la indexaciï¿½n que tuviese en ese momento e inicializa
IndexadorHash a partir de una indexaciï¿½n previamente realizada que habrï¿½
sido almacenada en ?directorioIndexacion? mediante el mï¿½todo ?bool
GuardarIndexacion()?. Con ello toda la parte privada se inicializarï¿½
convenientemente, igual que si se acabase de indexar la colecciï¿½n de
documentos. En caso que no exista el directorio o que no contenga los
datos de la indexaciï¿½n se tratarï¿½ la excepciï¿½n correspondiente, y se
devolverï¿½ false, dejando la indexaciï¿½n vacï¿½a.
*/
bool IndexadorHash::RecuperarIndexacion(const string &directorioIndexacion)
{
    // Comprobamos que el directorio exista
    if (opendir(directorioIndexacion.c_str()) == NULL)
    {
        cerr << "El directorio " << directorioIndexacion << " no existe" << endl;
        return false;
    }
    // Abrimos el fichero indexador.dat. Si no existe mostramos error.
    string ficheroIndexador = directorioIndexacion + "/indexador.dat";
    ifstream f(ficheroIndexador.c_str());
    if (!f)
    {
        cerr << "No se ha podido abrir el fichero " << ficheroIndexador << endl;
        return false;
    }
    f.exceptions(std::ios::failbit);
    try
    {
        // Leemos el indice del fichero
        // La primera linea indica el tamaï¿½o del indice
        string linea;
        std::getline(f, linea);
        int tam = atoi(linea.c_str());
        // El resto tam lineas indican el termino y la serializacion de InformacionTermino
        for (int i = 0; i < tam; i++)
        {
            std::getline(f, linea);
            string term = linea;
            InformacionTermino it;
            std::getline(f, linea);
            it.cargar(linea);
            indice[term] = it;
        }
        // Leemos el indice de documentos del fichero
        // La primera linea indica el tamaï¿½o del indice
        std::getline(f, linea);
        tam = atoi(linea.c_str());
        // El resto tam lineas indican el nombre del documento y la serializacion de InfDoc
        for (int i = 0; i < tam; i++)
        {
            std::getline(f, linea);
            string nomDoc = linea;
            InfDoc id;
            std::getline(f, linea);
            id.cargar(linea);
            indiceDocs[nomDoc] = id;
        }
        // Leemos el indice de preguntas del fichero
        // La primera linea indica el tamaï¿½o del indice
        std::getline(f, linea);
        tam = atoi(linea.c_str());
        // El resto tam lineas indican el termino y la serializacion de InformacionTerminoPregunta
        for (int i = 0; i < tam; i++)
        {
            std::getline(f, linea);
            string term = linea;
            InformacionTerminoPregunta itp;
            std::getline(f, linea);
            itp.cargar(linea);
            indicePregunta[term] = itp;
        }
        // Leemos el tipo de stemmer
        std::getline(f, linea);
        tipoStemmer = atoi(linea.c_str());
        // Leemos los stopwords del fichero
        // La primera linea indica el tamaï¿½o del unordered_set
        std::getline(f, linea);
        tam = atoi(linea.c_str());
        // El resto tam lineas indican los stopwords
        stemmerPorter stemmer = stemmerPorter();
        for (int i = 0; i < tam; i++)
        {
            std::getline(f, linea);
            stopWords_originales.insert(linea);
            stemmer.stemmer(linea, tipoStemmer);
            stopWords.insert(linea);
        }
        // Leemos la informacion de la coleccion de documentos
        std::getline(f, linea);
        informacionColeccionDocs.cargar(linea);
        // Leemos la pregunta
        std::getline(f, linea);
        pregunta = linea;
        // Leemos la informacion de la pregunta
        std::getline(f, linea);
        infPregunta.cargar(linea);
        // Leemos el fichero de stopwords
        std::getline(f, linea);
        ficheroStopWords = linea;
        // Leemos el tokenizador
        std::getline(f, linea);
        string delimitadoresPalabra;
        bool kcasosEspeciales;
        bool minuscSinAcentos;
        delimitadoresPalabra = linea.substr(14, linea.find(" TRATA CASOS ESPECIALES: ") - 14);
        if (linea.find(" TRATA CASOS ESPECIALES: ") == string::npos) {
            delimitadoresPalabra += "\n";
            std::getline(f, linea);
            if (linea.find(" TRATA CASOS ESPECIALES: ") == string::npos) {
                delimitadoresPalabra += "\r";
                std::getline(f, linea);
                delimitadoresPalabra += linea.substr(0, linea.find(" TRATA CASOS ESPECIALES: "));
            } else {
                delimitadoresPalabra += linea.substr(0, linea.find(" TRATA CASOS ESPECIALES: "));
            }
            
        }
        int pos = linea.find(" TRATA CASOS ESPECIALES: ") + 25;
        kcasosEspeciales = (linea.substr(pos, linea.find(" MINUSC SIN ACENTOS: ") - pos) == "1") ? true : false;
        pos = linea.find(" PASAR A MINUSCULAS Y SIN ACENTOS: ") + 35;
        minuscSinAcentos = (linea.substr(pos) == "1") ? true : false;
        tok = Tokenizador(delimitadoresPalabra, kcasosEspeciales, minuscSinAcentos);
        // Leemos el directorio donde se almacena el indice
        std::getline(f, linea);
        directorioIndice = linea;

        // Leemos si se almacena en disco
        std::getline(f, linea);
        almacenarEnDisco = (linea == "1") ? true : false;
        // Leemos si se almacenan las posiciones de los terminos
        std::getline(f, linea);
        almacenarPosTerm = (linea == "1") ? true : false;
        // Cerramos el fichero
    }
    catch (std::ios::failure &e)
    {
        cerr << "Error al leer el fichero " << ficheroIndexador << ": " << e.what() << endl;
        return false;
    }
    f.close();
    return true;
}

void IndexadorHash::ImprimirIndexacionPregunta()
{
    cout << "Pregunta indexada: " << pregunta << endl;
    cout << "Terminos indexados en la pregunta: " << endl;
    // A continuaciï¿½n aparecerï¿½ un listado del contenido de
    //"indicePregunta" donde para cada tï¿½rmino indexado se imprimirï¿½:
    for (auto termino : indicePregunta)
        cout << termino.first << '\t' << termino.second << endl;
    cout << "Informacion de la pregunta: " << infPregunta << endl;
}

/*// Devuelve true si se realiza el borrado (p.ej. si word, aplicï¿½ndole el
tratamiento de stemming y mayï¿½sculas correspondiente, aparecï¿½a como
tï¿½rmino indexado)
*/
bool IndexadorHash::Borra(const string &word)
{
    // Aplicamos el tratamiento de stemming y mayï¿½sculas correspondiente
    string termino;
    if (tok.PasarAminuscSinAcentos())
        termino = pasar_a_minusculas_sin_acentos(word);
    stemmerPorter stemmer = stemmerPorter();
    stemmer.stemmer(termino, tipoStemmer);
    // Si el termino esta en el indice
    if (indice.find(termino) != indice.end())
    {
        InformacionTermino *infTerm = &indice[termino];
        // Recorremos los documentos en los que aparece el tï¿½rmino
        for (auto it = indiceDocs.begin(); it != indiceDocs.end(); it++)
        {
            // Si el termino esta en el documento, lo borramos
            if (infTerm->existIdDoc(it->second.getIdDoc()))
            {
                int aux = (*infTerm->getInfTermDoc(it->second.getIdDoc())).getFt() * -1;
                // Actualizamos la informacion de la coleccion de documentos
                informacionColeccionDocs.addNumTotalPal(aux);
                informacionColeccionDocs.addNumTotalPalSinParada(aux);
                informacionColeccionDocs.addNumTotalPalDiferentes(-1);
                // Actualizamos la informacion del documento
                it->second.addNumPal(aux);
                it->second.addNumPalDiferentes(-1);
                it->second.addNumPalSinParada(aux);
            }
        }
        for (auto itGuardados : indiceDocs_guardados)
        {
            string fichero = itGuardados;
            replace(fichero.begin(), fichero.end(), '/', '-');
            InfDoc infDoc;
            // Abrimos el fichero del documento
            ifstream f;
            f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try
            {
                f.open((directorioIndice + "/indiceDocs/" + fichero).c_str());
                string linea;
                std::getline(f, linea);
                f.close();
                infDoc.cargar(linea);
            }
            catch (std::ios::failure &e)
            {
                cerr << "Error al abrir el fichero " << directorioIndice + "/indiceDocs/" + fichero << ": " << e.what() << endl;
                return false;
            }
            // Si el termino esta en el documento, lo borramos
            if (infTerm->existIdDoc(infDoc.getIdDoc()))
            {
                int aux = (*infTerm->getInfTermDoc(infDoc.getIdDoc())).getFt() * -1;
                // Actualizamos la informacion de la coleccion de documentos
                informacionColeccionDocs.addNumTotalPal(aux);
                informacionColeccionDocs.addNumTotalPalSinParada(aux);
                informacionColeccionDocs.addNumTotalPalDiferentes(-1);
                // Actualizamos la informacion del documento
                infDoc.addNumPal(aux);
                infDoc.addNumPalDiferentes(-1);
                infDoc.addNumPalSinParada(aux);
                // Guardamos la informacion en el fichero
                ofstream f2;
                f2.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                try
                {
                    f2.open((directorioIndice + "/indiceDocs/" + fichero).c_str());
                    string linea;
                    f2 << infDoc << "\tfechaModificacion: " << infDoc.getFechaModificacion();
                    f2.close();
                }
                catch (std::ios::failure &e)
                {
                    cerr << "Error al abrir el fichero " << directorioIndice + "/indiceDocs/" + fichero << ": " << e.what() << endl;
                    return false;
                }
            }
        }
        // Borrarlo del indice
        indice.erase(termino);
    }
    else
    {
        auto itG = indice_guardados.find(termino);
        if (itG != indice_guardados.end())
        { // Abrir el fichero
            ifstream f;
            string fichero = directorioIndice + "/indice/" + to_string(itG->second);
            f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try
            {
                f.open(fichero.c_str());
                string linea;
                std::getline(f, linea);
                f.close();
                // Cargar la informacion del termino
                InformacionTermino infTerm;
                infTerm.cargar(linea);
                // Recorremos los documentos en los que aparece el tï¿½rmino
                for (auto it = indiceDocs.begin(); it != indiceDocs.end(); it++)
                {
                    // Si el termino esta en el documento, lo borramos
                    if (infTerm.existIdDoc(it->second.getIdDoc()))
                    {
                        int aux = (*infTerm.getInfTermDoc(it->second.getIdDoc())).getFt() * -1;
                        // Actualizamos la informacion de la coleccion de documentos
                        informacionColeccionDocs.addNumTotalPal(aux);
                        informacionColeccionDocs.addNumTotalPalSinParada(aux);
                        informacionColeccionDocs.addNumTotalPalDiferentes(-1);
                        // Actualizamos la informacion del documento
                        it->second.addNumPal(aux);
                        it->second.addNumPalDiferentes(-1);
                        it->second.addNumPalSinParada(aux);
                    }
                }
                for (auto itGuardados : indiceDocs_guardados)
                {
                    string fichero = itGuardados;
                    replace(fichero.begin(), fichero.end(), '/', '-');
                    InfDoc infDoc;
                    // Abrimos el fichero del documento
                    ifstream f;
                    f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                    try
                    {
                        f.open((directorioIndice + "/indiceDocs/" + fichero).c_str());
                        string linea;
                        std::getline(f, linea);
                        f.close();
                        infDoc.cargar(linea);
                    }
                    catch (std::ios::failure &e)
                    {
                        cerr << "Error al abrir el fichero " << directorioIndice + "/indiceDocs/" + fichero << ": " << e.what() << endl;
                        return false;
                    }
                    // Si el termino esta en el documento, lo borramos
                    if (infTerm.existIdDoc(infDoc.getIdDoc()))
                    {
                        int aux = (*infTerm.getInfTermDoc(infDoc.getIdDoc())).getFt() * -1;
                        // Actualizamos la informacion de la coleccion de documentos
                        informacionColeccionDocs.addNumTotalPal(aux);
                        informacionColeccionDocs.addNumTotalPalSinParada(aux);
                        informacionColeccionDocs.addNumTotalPalDiferentes(-1);
                        // Actualizamos la informacion del documento
                        infDoc.addNumPal(aux);
                        infDoc.addNumPalDiferentes(-1);
                        infDoc.addNumPalSinParada(aux);
                        // Guardamos la informacion en el fichero
                        ofstream f2;
                        f2.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                        try
                        {
                            f2.open((directorioIndice + "/indiceDocs/" + fichero).c_str());
                            string linea;
                            f2 << infDoc << "\n";
                            f2.close();
                        }
                        catch (std::ios::failure &e)
                        {
                            cerr << "Error al abrir el fichero " << directorioIndice + "/indiceDocs/" + fichero << ": " << e.what() << endl;
                            return false;
                        }
                    }
                }
            }
            catch (std::ios::failure &e)
            {
                cerr << "Error al abrir el fichero " << directorioIndice + "/indice/" + termino << ": " << e.what() << endl;
                return false;
            }
            // Borrar el fichero
            remove(fichero.c_str());
            // Borrarlo del indice
            indice_guardados.erase(itG);
        }
        else
        {
            // El termino no existe
            return false;
        }
    }
    // Devolvemos true
    return true;
}

void IndexadorHash::VaciarIndiceDocs()
{
    // Vaciamos el indice de documentos
    indiceDocs.clear();
    // Borramos cada documento
    string fichero;
    for (auto it : indiceDocs_guardados)
    {
        fichero = it;
        replace(fichero.begin(), fichero.end(), '/', '-');
        remove((directorioIndice + "/indiceDocs/" + fichero).c_str());
    }
    // Vaciamos el indice de documentos guardados
    indiceDocs_guardados.clear();
    // Borramos el directorio de documentos
    remove((directorioIndice + "/indiceDocs").c_str());
    // Vaciamos la informacion de la coleccion de documentos
    informacionColeccionDocs = InfColeccionDocs();
    // Vaciamos el indice de palabras
    indice.clear();
    // Borramos el fichero de palabras
    for (auto it : indice_guardados)
    {
        remove((directorioIndice + "/indice/" + to_string(it.second)).c_str());
    }
    // Vaciamos el indice de palabras guardadas
    indice_guardados.clear();
    indice_actualizar.clear();
    // Borramos el directorio de palabras
    remove((directorioIndice + "/indice").c_str());
}

void IndexadorHash::VaciarIndicePreg()
{
    // Vaciamos el indice de pregunta
    indicePregunta.clear();
}

void IndexadorHash::ListarTerminos()
{
    InformacionTermino infTerm;
    for (auto it : indice_guardados)
    {
        Devuelve(it.first, infTerm);
        cout << it.first << '\t' << infTerm << endl;
    }
    // Recorremos el indice
    for (auto it = indice.begin(); it != indice.end(); it++)
    {
        // Mostramos el termino
        cout << it->first << '\t' << it->second << endl;
    }
}

bool IndexadorHash::ListarTerminos(const string &nomDoc)
{
    if (indiceDocs.find(nomDoc) != indiceDocs.end() || indiceDocs_guardados.find(nomDoc) != indiceDocs_guardados.end())
    {
        // Recorremos el indice
        InfTermDoc infTermDoc;
        for (auto it = indice_guardados.begin(); it != indice_guardados.end(); it++)
        {
            // Si el termino esta en el documento, lo mostramos
            if (Devuelve(it->first, nomDoc, infTermDoc))
            {
                cout << it->first << '\t' << infTermDoc << endl;
            }
        }
        for (auto it = indice.begin(); it != indice.end(); it++)
        {
            // Si el termino esta en el documento, lo mostramos
            if (Devuelve(it->first, nomDoc, infTermDoc))
            {
                cout << it->first << '\t' << infTermDoc << endl;
            }
        }
        // Devolvemos true
        return true;
    }
    else
    {
        // Devolvemos false
        return false;
    }
}