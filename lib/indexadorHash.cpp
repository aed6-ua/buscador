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
int LIMITE_INDEXACION_MEMORIA = 1;
using namespace std;

// IndexadorHash
/* ?fichStopWords? será el nombre del archivo que contendrá todas las
palabras de parada (una palabra por cada línea del fichero) y se
almacenará en el campo privado ?ficheroStopWords?. Asimismo, almacenará
todas las palabras de parada que contenga el archivo en el campo privado
?stopWords?, el índice de palabras de parada.
?delimitadores? será el string que contiene todos los delimitadores
utilizados por el tokenizador (campo privado ?tok?)
detectComp y minuscSinAcentos serán los parámetros que se pasarán al
tokenizador
?dirIndice? será el directorio del disco duro donde se almacenará el
índice (campo privado ?directorioIndice?). Si dirIndice=?? entonces se
almacenará en el directorio donde se ejecute el programa
?tStemmer? inicializará la variable privada ?tipoStemmer?:
0 = no se aplica stemmer: se indexa el término tal y como
aparece tokenizado
1 = stemmer de Porter para español
2 = stemmer de Porter para inglés
?almEnDisco? inicializará la variable privada ?almacenarEnDisco?
?almPosTerm? inicializará la variable privada ?almacenarPosTerm?
Los índices (p.ej. índice, indiceDocs e informacionColeccionDocs)
quedarán vacíos*/

IndexadorHash::IndexadorHash()
{
    indice = unordered_map<string, InformacionTermino>();
    // Índice de términos indexados accesible por el término
    indiceDocs = unordered_map<string, InfDoc>();
    // Índice de documentos indexados accesible por el nombre del documento
    informacionColeccionDocs = InfColeccionDocs();
    // Información recogida de la colección de documentos indexada
    pregunta = "";
    // Pregunta indexada actualmente. Si no hay ninguna indexada, contendría el valor ??
    indicePregunta = unordered_map<string, InformacionTerminoPregunta>();
    // Índice de términos indexados en una pregunta. Se almacenará en memoria principal
    infPregunta = InformacionPregunta();
    // Información recogida de la pregunta indexada. Se almacenará en memoria principal
    stopWords = unordered_set<string>();
    /* Palabras de parada. Se almacenará en memoria principal. El filtrado
    de palabras de parada se realizará, tanto en la pregunta como en los
    documentos, teniendo en cuenta el parámetro minuscSinAcentos y
    tipoStemmer. Es decir que se aplicará el mismo proceso a las palabras de
    parada almacenadas en el fichero antes de realizar el filtrado (p.ej. si
    se aplica el pasar a minúsculas los términos del documento/pregunta a
    indexar, para comprobar si se ha de eliminar el término, éste se
    comparará con la versión de palabras de parada en minúsculas). Esto se
    pide así para casos en los que en el documento/pregunta aparezca: ?La
    casa de Él? y estén almacenadas como stopWords ?la, el?, si se activa el
    parámetro minuscSinAcentos, entonces debería filtrar ?La, Él?, si no
    hubiese estado activo ese parámetro, entonces no se hubiesen filtrado.*/
    ficheroStopWords = "";
    // Nombre del fichero que contiene las palabras de parada
    tok = Tokenizador();
    /* Tokenizador que se usará en la indexación. Se inicializará con los
    parámetros del constructor: detectComp y minuscSinAcentos, los cuales
    determinarán qué término se ha de indexar (p.ej. si se activa
    minuscSinAcentos, entonces se guardarán los términos en minúsculas y sin
    acentos)*/
    directorioIndice = "";
    /* ?directorioIndice? será el directorio del disco duro donde se
    almacenará el índice. En caso que contenga la cadena vacía se creará en
    el directorio donde se ejecute el indexador*/
    tipoStemmer = 0;
    /* 0 = no se aplica stemmer: se indexa el término tal y como aparece
    tokenizado
    // Los siguientes valores harán que los términos a indexar se les
    aplique el stemmer y se almacene solo dicho stem.
    // 1 = stemmer de Porter para español
    // 2 = stemmer de Porter para inglés
    // Para el stemmer de Porter se utilizarán los archivos
    stemmer.cpp y stemmer.h, concretamente las funciones de nombre
    ?stemmer?*/
    almacenarEnDisco = false;
    /* Esta opción (cuando almacenarEnDisco == true) está ideada para poder
    indexar colecciones de documentos lo suficientemente grandes para que su
    indexación no quepa en memoria, por lo que si es true, mientras se va
    generando el índice, se almacenará la mínima parte de los índices de los
    documentos en memoria principal, p.ej. solo la estructura ?indice? para
    saber las palabras indexadas, guardando únicamente punteros a las
    posiciones de los archivos almacenados en disco que contendrán el resto
    de información asociada a cada término (lo mismo para indiceDocs). Se
    valorará el equilibrio para conseguir una indexación independientemente
    del tamaño del corpus a indexar, pero reduciendo el número de accesos a
    disco. Para los datos de la indexación de la pregunta no habría que
    hacer nada. En caso de que esta variable tenga valor false, se
    almacenará todo el índice en memoria principal (tal y como se ha
    descrito anteriormente).*/
    almacenarPosTerm = false;
    // Si es true se almacenará la posición en la que aparecen los términos dentro del documento en la clase InfTermDoc
}

IndexadorHash::IndexadorHash(const string &fichStopWords, const string &delimitadores,
                             const bool &detectComp, const bool &minuscSinAcentos, const string &dirIndice, const int &tStemmer, const bool &almEnDisco, const bool &almPosTerm)
{
    indice = unordered_map<string, InformacionTermino>();
    // Índice de términos indexados accesible por el término
    indiceDocs = unordered_map<string, InfDoc>();
    // Índice de documentos indexados accesible por el nombre del documento
    informacionColeccionDocs = InfColeccionDocs();
    // Información recogida de la colección de documentos indexada
    pregunta = "";
    // Pregunta indexada actualmente. Si no hay ninguna indexada, contendría el valor ??
    indicePregunta = unordered_map<string, InformacionTerminoPregunta>();
    // Índice de términos indexados en una pregunta. Se almacenará en memoria principal
    infPregunta = InformacionPregunta();
    // Información recogida de la pregunta indexada. Se almacenará en memoria principal

    ficheroStopWords = fichStopWords;
    // Nombre del fichero que contiene las palabras de parada

    /* Palabras de parada. Se almacenará en memoria principal. El filtrado
    de palabras de parada se realizará, tanto en la pregunta como en los
    documentos, teniendo en cuenta el parámetro minuscSinAcentos y
    tipoStemmer. Es decir que se aplicará el mismo proceso a las palabras de
    parada almacenadas en el fichero antes de realizar el filtrado (p.ej. si
    se aplica el pasar a minúsculas los términos del documento/pregunta a
    indexar, para comprobar si se ha de eliminar el término, éste se
    comparará con la versión de palabras de parada en minúsculas). Esto se
    pide así para casos en los que en el documento/pregunta aparezca: ?La
    casa de Él? y estén almacenadas como stopWords ?la, el?, si se activa el
    parámetro minuscSinAcentos, entonces debería filtrar ?La, Él?, si no
    hubiese estado activo ese parámetro, entonces no se hubiesen filtrado.
    */
    tipoStemmer = tStemmer;
    /* 0 = no se aplica stemmer: se indexa el término tal y como aparece
    tokenizado
    // Los siguientes valores harán que los términos a indexar se les
    aplique el stemmer y se almacene solo dicho stem.
    // 1 = stemmer de Porter para español
    // 2 = stemmer de Porter para inglés
    // Para el stemmer de Porter se utilizarán los archivos
    stemmer.cpp y stemmer.h, concretamente las funciones de nombre
    ?stemmer?*/
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
    /* Tokenizador que se usará en la indexación. Se inicializará con los
    parámetros del constructor: detectComp y minuscSinAcentos, los cuales
    determinarán qué término se ha de indexar (p.ej. si se activa
    minuscSinAcentos, entonces se guardarán los términos en minúsculas y sin
    acentos)*/
    directorioIndice = dirIndice;
    /* ?directorioIndice? será el directorio del disco duro donde se
    almacenará el índice. En caso que contenga la cadena vacía se creará en
    el directorio donde se ejecute el indexador*/

    almacenarEnDisco = almEnDisco;
    /* Esta opción (cuando almacenarEnDisco == true) está ideada para poder
    indexar colecciones de documentos lo suficientemente grandes para que su
    indexación no quepa en memoria, por lo que si es true, mientras se va
    generando el índice, se almacenará la mínima parte de los índices de los
    documentos en memoria principal, p.ej. solo la estructura ?indice? para
    saber las palabras indexadas, guardando únicamente punteros a las
    posiciones de los archivos almacenados en disco que contendrán el resto
    de información asociada a cada término (lo mismo para indiceDocs). Se
    valorará el equilibrio para conseguir una indexación independientemente
    del tamaño del corpus a indexar, pero reduciendo el número de accesos a
    disco. Para los datos de la indexación de la pregunta no habría que
    hacer nada. En caso de que esta variable tenga valor false, se
    almacenará todo el índice en memoria principal (tal y como se ha
    descrito anteriormente).*/
    almacenarPosTerm = almPosTerm;
    // Si es true se almacenará la posición en la que aparecen los términos dentro del documento en la clase InfTermDoc
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
    return *this;
}

/*Constructor para inicializar IndexadorHash a partir de una indexación
previamente realizada que habrá sido almacenada en
?directorioIndexacion? mediante el método ?bool GuardarIndexacion()?.
Con ello toda la parte privada se inicializará convenientemente, igual
que si se acabase de indexar la colección de documentos. En caso que no
exista el directorio o que no contenga los datos de la indexación se
tratará la excepción correspondiente
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

/* Devuelve true si nomDoc existe en la colección y muestra por pantalla
el contenido del campo privado ?indiceDocs? para el documento con nombre
?nomDoc?: cout << nomDoc << ?\t? << InfDoc << endl; . Si no existe no se
muestra nada*/
bool IndexadorHash::ListarDocs(const string &nomDoc) const
{
    auto it = indiceDocs.find(nomDoc);
    if (it != indiceDocs.end())
    {
        cout << it->first << '\t' << it->second << "\n";
        return true;
    }
    return false;
}

/* Devuelve true si consigue crear el índice para la colección de
    documentos detallada en ficheroDocumentos, el cual contendrá un nombre
    de documento por línea. Los añadirá a los ya existentes anteriormente en
    el índice.
    // Devuelve falso si no finaliza la indexación (p.ej. por falta de
    memoria), mostrando el mensaje de error correspondiente, indicando el
    documento y término en el que se ha quedado, dejando en memoria lo que
    se haya indexado hasta ese momento.
    // En el caso que aparezcan documentos repetidos, documentos que no
    existen o que ya estuviesen previamente indexados (ha de coincidir el
    nombre del documento y el directorio en que se encuentre), se devolverá
    true, mostrando el mensaje de excepción correspondiente, y se reindexarán (borrar el documento previamente indexado e indexar el nuevo)
    en caso que la fecha de modificación del documento sea más reciente que
    la almacenada previamente (class ?InfDoc? campo ?fechaModificacion?).
    Los casos de reindexación mantendrán el mismo idDoc.*/

bool IndexadorHash::Indexar(const string &ficheroDocumentos)
{
    /*Devuelve true si consigue crear el índice para la colección de
    documentos detallada en ficheroDocumentos, el cual contendrá un nombre
    de documento por línea. Los añadirá a los ya existentes anteriormente en
    el índice.*/
    ifstream f(ficheroDocumentos.c_str());
    if (!f)
    {
        cerr << "Error al abrir el fichero de documentos " << ficheroDocumentos << endl;
        return false;
    }
    string linea;
    int contador = 0;
    while (std::getline(f, linea))
    {
        // Comprobar si el documento ya está indexado
        if ((indiceDocs.find(linea) != indiceDocs.end()))
        {
            // Comprobar si el documento ha sido modificado
            struct stat atributos;
            stat(linea.c_str(), &atributos);
            time_t fechaModificacion = atributos.st_mtime;
            // Si la fecha de modificación es anterior a la del documento indexado, no se indexa
            if (fechaModificacion > indiceDocs[linea].getFechaModificacion())
            {
                // Obtener el idDoc
                int idDoc = indiceDocs[linea].getIdDoc();
                // Borrar el documento previamente indexado
                if (!BorraDoc(linea))
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
            ifstream f;
            string fichero = linea;
            replace(fichero.begin(), fichero.end(), '/', '-');
            f.open((directorioIndice + "/indiceDocs/" + fichero).c_str());
            if (!f)
            {
                cerr << "Error al abrir el fichero de indiceDocs " << linea << endl;
                return false;
            }
            // Leer el fichero de indiceDocs
            string line;
            if (std::getline(f, line))
            {
                infDocGuardado.cargar(line);
            }
            f.close();
            // Si la fecha de modificación es anterior a la del documento indexado, no se indexa
            if (fechaModificacion > infDocGuardado.getFechaModificacion())
            {
                // Obtener el idDoc
                int idDoc = infDocGuardado.getIdDoc();
                // Borrar el documento previamente indexado
                if (!BorraDocGuardado(linea, infDocGuardado))
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
            AlmacenarEnDisco();
    }
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
    // Obtener la fecha de modificación
    struct stat atributos;
    stat(nomDoc.c_str(), &atributos);
    time_t fechaModificacion = atributos.st_mtime;
    // Tokenizar el documento
    if (tok.Tokenizar(nomDoc))
    {
        // Documento .tk que contiene un término por línea
        ifstream f(nomDoc + ".tk");
        if (!f)
        {
            cerr << "Error al abrir el fichero de tokens " << nomDoc << ".tk" << endl;
            return false;
        }
        string linea;
        // Inicializar stemmer
        stemmerPorter stemmer = stemmerPorter();

        // Número de palabras del documento
        int numPalabras = 0;
        // Número de palabras que no son stopWords
        int numPalabrasSinStopWords = 0;
        // Número de palabras distintas del documento
        int numPalabrasDistintas = 0;
        // Número de términos distintos total
        int numTotalPalDiferentes = 0;

        // Palabras distintas del documento
        unordered_set<string> palabrasDistintas;
        while (std::getline(f, linea))
        {
            // Aplicar el stemmer
            stemmer.stemmer(linea, tipoStemmer);
            // Comprobar si el término es una stopWord
            if (stopWords.find(linea) == stopWords.end())
            {
                numPalabrasSinStopWords++;
                // Comprobar si el término está en el índice
                if (indice.find(linea) != indice.end())
                {
                    // Obtener la información del término del índice
                    InformacionTermino *infTerm = &(indice[linea]);
                    // Actualizar la información del término
                    infTerm->addFtc();
                    // Comprobar si el término está en el documento (si existe el idDoc en el map)
                    if (infTerm->existIdDoc(idDoc))
                    {
                        // Obtener la información del término en el documento
                        InfTermDoc *infTermDoc = infTerm->getInfTermDoc(idDoc);
                        // Actualizar la información del término en el documento
                        infTermDoc->addFt();
                        if (almacenarPosTerm)
                            infTermDoc->addPosTerm(numPalabras);
                    }
                    else
                    {
                        // Rellenar la información del término en el documento
                        InfTermDoc infTermDoc = InfTermDoc();
                        infTermDoc.addFt();
                        if (almacenarPosTerm)
                            infTermDoc.addPosTerm(numPalabras);
                        // Actualizar la información del término en el documento
                        infTerm->addInfTermDoc(idDoc, infTermDoc);
                    }
                }
                else if (indice_guardados.find(linea) != indice_guardados.end())
                {
                    // Obtener la información del término del índice
                    InformacionTermino infTerm = InformacionTermino();
                    // Abrir el fichero de indice
                    ifstream f3;
                    f3.open((directorioIndice + "/indice/" + linea).c_str());
                    if (!f3)
                    {
                        cerr << "Error al abrir el fichero de indice " << linea << endl;
                        return false;
                    }
                    // Leer el fichero de indice
                    string linea;
                    if (std::getline(f3, linea))
                    {
                        infTerm.cargar(linea);
                    }
                    f3.close();
                    // Actualizar la información del término
                    infTerm.addFtc();
                    // Comprobar si el término está en el documento (si existe el idDoc en el map)
                    if (infTerm.existIdDoc(idDoc))
                    {
                        // Obtener la información del término en el documento
                        InfTermDoc *infTermDoc = infTerm.getInfTermDoc(idDoc);
                        // Actualizar la información del término en el documento
                        infTermDoc->addFt();
                        if (almacenarPosTerm)
                            infTermDoc->addPosTerm(numPalabras);
                    }
                    else
                    {
                        // Rellenar la información del término en el documento
                        InfTermDoc infTermDoc = InfTermDoc();
                        infTermDoc.addFt();
                        if (almacenarPosTerm)
                            infTermDoc.addPosTerm(numPalabras);
                        // Actualizar la información del término en el documento
                        infTerm.addInfTermDoc(idDoc, infTermDoc);
                    }
                    // Guardar el fichero de indice
                    ofstream f2;
                    f2.open((directorioIndice + "/indice/" + linea).c_str());
                    if (!f2)
                    {
                        cerr << "Error al abrir el fichero de indice " << linea << endl;
                        return false;
                    }
                    f2 << infTerm;
                    f2.close();
                }
                else
                {
                    numTotalPalDiferentes++;
                    // Rellenar la información del término en el documento
                    InfTermDoc infTermDoc = InfTermDoc();
                    infTermDoc.addFt();
                    if (almacenarPosTerm)
                        infTermDoc.addPosTerm(numPalabras);
                    // Rellenar la información del término
                    InformacionTermino infTerm = InformacionTermino(1);
                    infTerm.addInfTermDoc(idDoc, infTermDoc);
                    // Añadir el término al índice
                    Inserta(linea, infTerm);
                }
                // Comprobar si la palabra es única en el documento
                if (palabrasDistintas.find(linea) == palabrasDistintas.end())
                {
                    // Añadir la palabra al conjunto de palabras distintas del documento
                    palabrasDistintas.insert(linea);
                    numPalabrasDistintas++;
                }
                // Guardar indexación si se ha alcanzado el número de palabras indicado y almacenarEnDisco es true
                if (almacenarEnDisco && numPalabrasSinStopWords >= LIMITE_INDEXACION_MEMORIA)
                {
                    // Guardar el índice en disco
                    if (!GuardarIndexacion())
                        return false;
                    // Vaciar el índice de memoria
                    // todo
                }
            }
            numPalabras++;
        }
        f.close();
        // Obtener fecha de modificación del documento y tamaño en bytes
        struct stat atributos;
        stat(nomDoc.c_str(), &atributos);
        int tamBytes = atributos.st_size;
        time_t fechaModificacion = atributos.st_mtime;
        // Añadir el documento al índice de documentos
        indiceDocs.insert(make_pair(nomDoc, InfDoc(informacionColeccionDocs.getNumDocs() + 1,
                                                   numPalabras, numPalabrasSinStopWords, numPalabrasDistintas,
                                                   tamBytes, fechaModificacion)));
        // Actualizar la información de la colección de documentos
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

/* Devuelve true si nomDoc está indexado y se realiza el borrado de
todos los términos del documento y del documento en los campos privados
?indiceDocs? e ?informacionColeccionDocs?*/
bool IndexadorHash::BorraDoc(const string &nomDoc)
{
    // Comprobar si el documento está indexado
    if (indiceDocs.find(nomDoc) != indiceDocs.end())
    {
        // Obtener el idDoc
        int idDoc = indiceDocs.find(nomDoc)->second.getIdDoc();
        // Obtener la información del documento
        InfDoc infDoc = indiceDocs.find(nomDoc)->second;
        // Borrar el documento del índice de documentos
        indiceDocs.erase(nomDoc);
        // Actualizar la información de la colección de documentos
        informacionColeccionDocs.subDoc();
        informacionColeccionDocs.subNumTotalPal(infDoc.getNumPal());
        informacionColeccionDocs.subNumTotalPalSinParada(infDoc.getNumPalSinParada());
        informacionColeccionDocs.subTamBytes(infDoc.getTamBytes());
        // Borrar los términos del documento del índice
        auto it = indice.begin();
        while (it != indice.end())
        {
            // Obtener la información del término
            InformacionTermino infTerm = it->second;
            // Comprobar si el término está en el documento
            if (infTerm.existIdDoc(idDoc))
            {
                // Borrar el término del documento
                infTerm.eraseInfTermDoc(idDoc);
                // Comprobar si el término está en otros documentos
                if (infTerm.getNumDocs() > 0)
                {
                    // Actualizar el término en el índice
                    indice[it->first] = infTerm;
                }
                else
                {
                    // Borrar el término del índice
                    it = indice.erase(it);
                    informacionColeccionDocs.subNumTotalPalDiferentes();
                    continue;
                }
            }
            it++;
        }
        return true;
    }
    return false;
}

bool IndexadorHash::BorraDocGuardado(string &nomDoc, const InfDoc &infDoc)
{
    // Obtener el idDoc
    int idDoc = infDoc.getIdDoc();
    // Borrar el documento del índice de documentos
    indiceDocs_guardados.erase(nomDoc);
    string fichero = nomDoc;
    replace(fichero.begin(), fichero.end(), '/', '-');
    // Borrar el fichero del documento
    remove((directorioIndice + "/indiceDocs/" + fichero).c_str());
    // Actualizar la información de la colección de documentos
    informacionColeccionDocs.subDoc();
    informacionColeccionDocs.subNumTotalPal(infDoc.getNumPal());
    informacionColeccionDocs.subNumTotalPalSinParada(infDoc.getNumPalSinParada());
    informacionColeccionDocs.subTamBytes(infDoc.getTamBytes());
    // Borrar los términos del documento del índice
    auto it = indice.begin();
    while (it != indice.end())
    {
        // Obtener la información del término
        InformacionTermino infTerm = it->second;
        // Comprobar si el término está en el documento
        if (infTerm.existIdDoc(idDoc))
        {
            // Borrar el término del documento
            infTerm.eraseInfTermDoc(idDoc);
            // Comprobar si el término está en otros documentos
            if (infTerm.getNumDocs() > 0)
            {
                // Actualizar el término en el índice
                indice[it->first] = infTerm;
            }
            else
            {
                // Borrar el término del índice
                it = indice.erase(it);
                informacionColeccionDocs.subNumTotalPalDiferentes();

                continue;
            }
        }
        it++;
    }
    // Borramos los términos del documento del índice guardado
    auto itTerm = indice_guardados.begin();
    while (itTerm != indice_guardados.end())
    {
        // Obtener la información del término
        InformacionTermino infTerm = InformacionTermino();
        // Abrir el fichero del término
        ifstream f;
        string fichero = *itTerm;
        replace(fichero.begin(), fichero.end(), '/', '-');
        f.open((directorioIndice + "/indiceDocs/" + fichero).c_str());
        if (!f.is_open())
        {
            cerr << "Error al abrir el fichero del término " << *itTerm << endl;
            return false;
        }
        // Leer la información del término
        string linea;
        if (std::getline(f, linea))
        {
            infTerm.cargar(linea);
        }
        else
        {
            cerr << "Error al leer la información del término " << *itTerm << endl;
            return false;
        }
        // Cerrar el fichero del término
        f.close();
        // Comprobar si el término está en el documento
        if (infTerm.existIdDoc(idDoc))
        {
            // Borrar el término del documento
            infTerm.eraseInfTermDoc(idDoc);
            // Comprobar si el término está en otros documentos
            if (infTerm.getNumDocs() > 0)
            {
                // Actualizar el término en el índice sobreescribiendo el fichero
                ofstream f;
                f.open((directorioIndice + "/indiceDocs/" + fichero).c_str());
                if (!f.is_open())
                {
                    cerr << "Error al abrir el fichero del término " << *itTerm << endl;
                    return false;
                }
                // Escribir la información del término
                f << infTerm;
                // Cerrar el fichero del término
                f.close();
            }
            else
            {
                // Borrar el término del índice
                itTerm = indice_guardados.erase(itTerm);
                informacionColeccionDocs.subNumTotalPalDiferentes();
                // Borrar el fichero del término
                if (std::remove((directorioIndice + "/indiceDocs/" + fichero).c_str()) != 0)
                {
                    cerr << "Error al borrar el fichero del término " << *itTerm << endl;
                    return false;
                }
                continue;
            }
        }
        it++;
    }
    return true;
}

/* Mostrará por pantalla las palabras de parada almacenadas (originales,
sin aplicar stemming): una palabra por línea (salto de línea al final de
cada palabra) Las stopWords están en la variable privada unordered_set stopWords*/
void IndexadorHash::ListarPalParada() const
{
    for (unordered_set<string>::const_iterator it = stopWords_originales.begin(); it != stopWords_originales.end(); ++it)
        cout << *it << "\n";
}

/* Se guardará en disco duro (directorio contenido en la variable
privada ?directorioIndice?) la indexación actualmente en memoria
(incluidos todos los parámetros de la parte privada). La forma de
almacenamiento la determinará el alumno. El objetivo es que esta
indexación se pueda recuperar posteriormente mediante el constructor
?IndexadorHash(const string& directorioIndexacion)?. Por ejemplo,
supongamos que se ejecuta esta secuencia de comandos: ?IndexadorHash
a(?./fichStopWords.txt?, ?[ ,.?, ?./dirIndexPrueba?, 0, false);
a.Indexar(?./fichConDocsAIndexar.txt?); a.GuardarIndexacion();?,
entonces mediante el comando: ?IndexadorHash b(?./dirIndexPrueba?);? se
recuperará la indexación realizada en la secuencia anterior, cargándola
en ?b?
// Devuelve falso si no finaliza la operación (p.ej. por falta de
memoria, o el nombre del directorio contenido en ?directorioIndice? no
es correcto), mostrando el mensaje de error correspondiente, vaciando
los ficheros generados.
// En caso que no existiese el directorio directorioIndice, habría que
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

/* Se guardará en disco duro (directorio contenido en la variable
privada ?directorioIndice?) la indexación actualmente en memoria
(incluidos todos los parámetros de la parte privada). La forma de
almacenamiento la determinará el alumno. El objetivo es que esta
indexación se pueda recuperar posteriormente mediante el constructor
?IndexadorHash(const string& directorioIndexacion)?. Por ejemplo,
supongamos que se ejecuta esta secuencia de comandos: ?IndexadorHash
a(?./fichStopWords.txt?, ?[ ,.?, ?./dirIndexPrueba?, 0, false);
a.Indexar(?./fichConDocsAIndexar.txt?); a.GuardarIndexacion();?,
entonces mediante el comando: ?IndexadorHash b(?./dirIndexPrueba?);? se
recuperará la indexación realizada en la secuencia anterior, cargándola
en ?b?
// Devuelve falso si no finaliza la operación (p.ej. por falta de
memoria, o el nombre del directorio contenido en ?directorioIndice? no
es correcto), mostrando el mensaje de error correspondiente, vaciando
los ficheros generados.
// En caso que no existiese el directorio directorioIndice, habría que
crearlo previamente
*/
bool IndexadorHash::AlmacenarEnDisco()
{
    // Comprobar si el directorio existe y si no crearlo
    if (mkdir(directorioIndice.c_str(), 0777) == -1 && errno != EEXIST)
    {
        cerr << "Error al crear el directorio " << directorioIndice << "\n";
        return false;
    }
    // Guardar el unordered_map indice
    // Creamos una carpeta donde se guardaran los ficheros de los terminos. Los ficheros tendrán el nombre del termino y contendrán la informacion del termino
    if (mkdir((directorioIndice + "/indice").c_str(), 0777) == -1 && errno != EEXIST)
    {
        cerr << "Error al crear el directorio " << directorioIndice + "/indice"
             << "\n";
        return false;
    }
    auto it = indice.begin();
    while (it != indice.end())
    {
        cout << directorioIndice + "/indice/" + it->first << "\n";
        ofstream f(directorioIndice + "/indice/" + it->first);
        f.exceptions(std::ios::failbit);
        try
        {
            if (!f)
            {
                cerr << "Error al abrir el fichero " << directorioIndice + "/indice/" + it->first
                     << "\n";
                return false;
            }
            f << it->second;
        }
        catch (const std::ios::failure &e)
        {
            cerr << "Error al escribir en el fichero " << directorioIndice + "/indice/" + it->first << ": " << e.what() << "\n";
            // Borrar el fichero
            remove((directorioIndice + "/indice/" + it->first).c_str());
            return false;
        }
        f.close();
        // Borrar el termino del indice y guardar el nombre en el set indice_guardados
        indice_guardados.insert(it->first);
        it = indice.erase(it);
    }
    // Guardar el unordered_map indiceDocs
    // Creamos una carpeta donde se guardaran los ficheros de los documentos. Los ficheros tendrán el nombre del documento y contendrán la informacion del documento
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
    string fichero = nomDoc;
    replace(fichero.begin(), fichero.end(), '/', '-');
    if (indiceDocs.find(nomDoc) != indiceDocs.end())
        idDoc = indiceDocs.find(nomDoc)->second.getIdDoc();
    else if (indiceDocs_guardados.find(nomDoc) != indiceDocs_guardados.end())
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
    /* A continuación aparecerá un listado del contenido del campo
    privado ?índice? donde para cada término indexado se imprimirá: */
    for (unordered_map<string, InformacionTermino>::const_iterator it = indice.begin(); it != indice.end(); ++it)
        cout << it->first << "\t" << it->second << endl;
    // cout << termino << "\t" << InformacionTermino << endl;
    // cout << "Documentos indexados: " << endl;
    /* A continuación aparecerá un listado del contenido del campo
    privado ?indiceDocs? donde para cada documento indexado se imprimirá:*/
    for (unordered_map<string, InfDoc>::const_iterator it = indiceDocs.begin(); it != indiceDocs.end(); ++it)
        cout << it->first << "\t" << it->second << endl;
    // cout << nomDoc << "\t" << InfDoc << endl;
}

/*// Devuelve true si consigue crear el índice para la colección de
documentos que se encuentra en el directorio (y subdirectorios que
contenga) dirAIndexar (independientemente de la extensión de los
mismos). Se considerará que todos los documentos del directorio serán
ficheros de texto. Los añadirá a los ya existentes anteriormente en el
índice.
// Devuelve falso si no finaliza la indexación (p.ej. por falta de
memoria o porque no exista ?dirAIndexar?), mostrando el mensaje de error
correspondiente, indicando el documento y término en el que se ha
quedado, dejando en memoria lo que se haya indexado hasta ese momento.
// En el caso que aparezcan documentos repetidos o que ya estuviesen
previamente indexados (ha de coincidir el nombre del documento y el
directorio en que se encuentre), se mostrará el mensaje de excepción
correspondiente, y se re-indexarán (borrar el documento previamente
indexado e indexar el nuevo) en caso que la fecha de modificación del
documento sea más reciente que la almacenada previamente (class ?InfDoc?
campo ?fechaModificacion?). Los casos de reindexación mantendrán el
mismo idDoc.*/
bool IndexadorHash::IndexarDirectorio(const string &dirAIndexar)
{
    char *_emergencyMemory = new char[16384];
    string documentoActual;
    try
    {
        // Hacer una lista de los ficheros a indexar en el directorio y en sus subdirectorios
        // y luego indexarlos
        list<string> listaFicheros;
        for (const auto &entry : filesystem::recursive_directory_iterator(dirAIndexar))
            listaFicheros.push_back(entry.path().string());

        // Ordenar la lista de ficheros
        listaFicheros.sort();

        // Indexar los ficheros
        for (auto it = listaFicheros.begin(); it != listaFicheros.end(); ++it)
        {
            documentoActual = *it;
            // Comprobar si el documento ya estaba indexado
            if (indiceDocs.find(*it) != indiceDocs.end())
            {
                // Comprobar si el documento ha sido modificado
                struct stat atributos;
                stat((*it).c_str(), &atributos);
                if (difftime(atributos.st_mtime, indiceDocs.find(*it)->second.getFechaModificacion()) > 0)
                {
                    // Obtener el idDoc del documento
                    int idDoc = indiceDocs.find(*it)->second.getIdDoc();
                    // Borrar el documento previamente indexado
                    BorraDoc(*it);
                    // Indexar el nuevo documento
                    IndexarDoc(*it, idDoc);
                }
            }
            else
            {
                // Indexar el nuevo documento
                IndexarDoc(*it);
            }
        }

        /*
        if ((dir = opendir(dirAIndexar.c_str())) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                // Si es un directorio, lo recorremos
                if (ent->d_type == DT_DIR) {
                    // Si no es . ni ..
                    if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                        // Recorremos el directorio
                        IndexarDirectorio(dirAIndexar + "/" + ent->d_name);
                    }
                }
                // Si es un fichero, lo indexamos
                else if (ent->d_type == DT_REG) {
                    // Comprobar si el documento ya estaba indexado
                    if (indiceDocs.find(dirAIndexar + "/" + ent->d_name) != indiceDocs.end()) {
                        // Comprobar si el documento ha sido modificado
                        struct stat atributos;
                        stat((dirAIndexar + "/" + ent->d_name).c_str(), &atributos);
                        time_t fechaModificacion = atributos.st_mtime;
                        // Si la fecha de modificación es anterior a la del documento indexado, no se indexa
                        if (fechaModificacion > indiceDocs[dirAIndexar + "/" + ent->d_name].getFechaModificacion()) {
                            // Obtenemos el idDoc del documento
                            int idDoc = indiceDocs[dirAIndexar + "/" + ent->d_name].getIdDoc();
                            // Borrar el documento previamente indexado
                            BorraDoc(dirAIndexar + "/" + ent->d_name);
                            // Indexar el nuevo manteniendo el idDoc
                            IndexarDoc(dirAIndexar + "/" + ent->d_name, idDoc);
                        }
                    }
                    else {
                        // Indexar el documento
                        IndexarDoc(dirAIndexar + "/" + ent->d_name);
                    }
                }
            }
            closedir(dir);
        } else {
            // No se pudo abrir el directorio
            cerr << "No se pudo abrir el directorio " << dirAIndexar << endl;
            return false;
        }*/
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

/*// Devuelve true si consigue crear el índice para la pregunta ?preg?.
Antes de realizar la indexación vaciará los campos privados
indicePregunta e infPregunta
// Generará la misma información que en la indexación de documentos,
pero dejándola toda accesible en memoria principal (mediante las
variables privadas ?pregunta, indicePregunta, infPregunta?)
// Devuelve falso si no finaliza la operación (p.ej. por falta de
memoria o bien si la pregunta no contiene ningún término con contenido),
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
            // Comprobar si el término es una stopword
            if (stopWords.find(term) == stopWords.end())
            {
                ++numTotalPalSinParada;
                // Comprobar si el término está en el índice
                if (indicePregunta.find(term) == indicePregunta.end())
                {
                    ++numTotalPalDiferentes;
                    // Crear la InformacionTerminoPregunta
                    InformacionTerminoPregunta infTerminoPregunta;
                    infTerminoPregunta.addFt();
                    if (almacenarPosTerm)
                        infTerminoPregunta.addPosTerm(pos);
                    // Añadir el término al índice
                    indicePregunta.insert(pair<string, InformacionTerminoPregunta>(term, infTerminoPregunta));
                }
                else
                {
                    // Aumentar el ft del término
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
    // Aplicar tratamiento de mayúsculas y acentos
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
        if (Existe(term))
        {
            // Abrir el fichero
            ifstream f;
            f.open((directorioIndice + "/indice/" + term).c_str());
            if (!f.is_open())
            {
                cerr << "No se ha podido abrir el fichero " << directorioIndice + "/indice/" + term << endl;
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
                cerr << "No se ha podido leer el fichero " << directorioIndice + "/indice/" + term << endl;
                return false;
            }
        }
        else
            return false;
    }
}

// Devuelve true si word (aplicándole el tratamiento de stemming y mayúsculas correspondiente) aparece como término indexado
bool IndexadorHash::Existe(const string &word)
{
    // Comprobar si el término está en el índice
    if (indice.find(word) != indice.end())
        return true;
    else if (indice_guardados.find(word) != indice_guardados.end())
        return true;
    else
        return false;
}

string IndexadorHash::pasar_a_minusculas_sin_acentos(const string &str) const
{
    string result = "";
    for (unsigned char c : str)
    {
        int code = int(c);
        if (code < 192)
            result += tolower(c);
        else if (code == 209)
            result += '?';
        else if (code < 224)
        {
            double y = floor((code - 192) / 6);
            if (y <= 2)
                result += string(1, ((y + 1) * 4) + 93);
            else
                result += string(1, (y * 6) + 93);
        }
        else if (code == 241)
            result += '?';
        else
        {
            double y = floor((code - 224) / 6);
            if (y <= 2)
                result += tolower(char(((y + 1) * 4) + 93));
            else
                result += tolower(char((y * 6) + 93));
        }
    }
    return result;
}

/* Será true si se realiza la inserción (p.ej. si word, aplicándole el
tratamiento de stemming y mayúsculas correspondiente, no estaba
previamente indexado)*/
bool IndexadorHash::Inserta(const string &word, const InformacionTermino &inf)
{
    // Aplicar tratamiento de mayúsculas y acentos
    if (tok.PasarAminuscSinAcentos())
        string wordTratada = pasar_a_minusculas_sin_acentos(word);
    // Aplicar stemming
    stemmerPorter stemmer = stemmerPorter();
    string term = word;
    stemmer.stemmer(term, tipoStemmer);
    // Comprobar si el término está en el índice
    if (indiceDocs.find(term) == indiceDocs.end())
    {
        // Añadir el término al índice
        indice.insert(pair<string, InformacionTermino>(term, inf));
        return true;
    }
    else
        return false;
}

/*// Devuelve true si hay una pregunta indexada (con al menos un término
que no sea palabra de parada, o sea, que haya algún término indexado en
indicePregunta), devolviéndo ?pregunta? en ?preg?
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

/*// Devuelve true si word (aplicándole el tratamiento de stemming y
mayúsculas correspondiente) está indexado en la pregunta, devolviendo su
información almacenada ?inf?. En caso que no esté, devolvería ?inf?
vacío*/
bool IndexadorHash::DevuelvePregunta(const string &word, InformacionTerminoPregunta &inf) const
{
    // Aplicar tratamiento de mayúsculas y acentos
    if (tok.PasarAminuscSinAcentos())
        string wordTratada = pasar_a_minusculas_sin_acentos(word);
    // Aplicar stemming
    stemmerPorter stemmer = stemmerPorter();
    string term = word;
    stemmer.stemmer(term, tipoStemmer);
    // Comprobar si el término está en el índice
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
información almacenada (campo privado ?infPregunta?) en ?inf?. En caso
que no esté, devolvería ?inf? vacío*/
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

/*// Vacía la indexación que tuviese en ese momento e inicializa
IndexadorHash a partir de una indexación previamente realizada que habrá
sido almacenada en ?directorioIndexacion? mediante el método ?bool
GuardarIndexacion()?. Con ello toda la parte privada se inicializará
convenientemente, igual que si se acabase de indexar la colección de
documentos. En caso que no exista el directorio o que no contenga los
datos de la indexación se tratará la excepción correspondiente, y se
devolverá false, dejando la indexación vacía.
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
        // La primera linea indica el tamaño del indice
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
        // La primera linea indica el tamaño del indice
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
        // La primera linea indica el tamaño del indice
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
        // La primera linea indica el tamaño del unordered_set
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
        int pos = linea.find(" TRATA CASOS ESPECIALES: ") + 25;
        kcasosEspeciales = (linea.substr(pos, linea.find(" MINUSC SIN ACENTOS: ") - pos) == "1") ? true : false;
        pos = linea.find(" PASAR A MINUSCULAS Y SIN ACENTOS: ") + 34;
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
    // A continuación aparecerá un listado del contenido de
    //"indicePregunta" donde para cada término indexado se imprimirá:
    for (auto termino : indicePregunta)
        cout << termino.first << '\t' << termino.second << endl;
    cout << "Informacion de la pregunta: " << infPregunta << endl;
}

/*// Devuelve true si se realiza el borrado (p.ej. si word, aplicándole el
tratamiento de stemming y mayúsculas correspondiente, aparecía como
término indexado)
*/
bool IndexadorHash::Borra(const string &word)
{
    // Aplicamos el tratamiento de stemming y mayúsculas correspondiente
    string termino;
    if (tok.PasarAminuscSinAcentos())
        termino = pasar_a_minusculas_sin_acentos(word);
    stemmerPorter stemmer = stemmerPorter();
    stemmer.stemmer(termino, tipoStemmer);
    // Si el termino esta en el indice
    if (indice.find(termino) != indice.end())
    {
        InformacionTermino *infTerm = &indice[termino];
        // Recorremos los documentos en los que aparece el término
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
        // Borrarlo del indice
        indice.erase(termino);
    }
    else if (indice_guardados.find(termino) != indice_guardados.end()) {
        // Abrir el fichero
        ifstream f;
        f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            f.open((directorioIndice + "/indice/" + termino).c_str());
            string linea;
            std::getline(f, linea);
            f.close();
            // Cargar la informacion del termino
            InformacionTermino infTerm;
            infTerm.cargar(linea);
            // Recorremos los documentos en los que aparece el término
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
        remove((directorioIndice + "/indice/" + termino).c_str());
        // Borrarlo del indice
        indice_guardados.erase(termino);

    }
    // Borramos el termino del indice
    indice.erase(termino);
    // Devolvemos true
    return true;
}

void IndexadorHash::VaciarIndiceDocs() {
    // Vaciamos el indice de documentos
    indiceDocs.clear();
    // Borramos cada documento
    string fichero;
    for (auto it : indiceDocs_guardados) {
        fichero = it;
        replace(fichero.begin(), fichero.end(), '/', '-');
        remove((directorioIndice + "/indiceDocs/" + fichero).c_str());
    }
    // Vaciamos el indice de documentos guardados
    indiceDocs_guardados.clear();
    // Borramos el directorio de documentos
    remove((directorioIndice + "/indiceDocs").c_str());
}

void IndexadorHash::VaciarIndicePreg() {
    // Vaciamos el indice de pregunta
    indice.clear();
    // Borramos los indices guardados
    for (auto it : indice_guardados) {
        remove((directorioIndice + "/indice/" + it).c_str());
    }
}

void IndexadorHash::ListarTerminos() {
    InformacionTermino infTerm;
    for (auto it : indice_guardados) {
        Devuelve(it, infTerm);
        cout << it << '\t' << infTerm << endl;
    }
    // Recorremos el indice
    for (auto it = indice.begin(); it != indice.end(); it++) {
        // Mostramos el termino
        cout << it->first << '\t' << it->second << endl;
    }
}

bool IndexadorHash::ListarTerminos(const string &nomDoc) {
    if (indiceDocs.find(nomDoc) != indiceDocs.end() || indiceDocs_guardados.find(nomDoc) != indiceDocs_guardados.end()) {
        // Recorremos el indice
        InfTermDoc infTermDoc;
        for (auto it = indice_guardados.begin(); it != indice_guardados.end(); it++) {
            // Si el termino esta en el documento, lo mostramos
            if (Devuelve(*it, nomDoc, infTermDoc)) {
                cout << *it << '\t' << infTermDoc << endl;
            }
        }
        for (auto it = indice.begin(); it != indice.end(); it++) {
            // Si el termino esta en el documento, lo mostramos
            if (Devuelve(it->first, nomDoc, infTermDoc)) {
                cout << it->first << '\t' << infTermDoc << endl;
            }
        }
        // Devolvemos true
        return true;
    }
    else {
        // Devolvemos false
        return false;
    }
}