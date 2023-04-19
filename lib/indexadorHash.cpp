#include <sys/stat.h>
#include <unordered_set>
#include "indexadorHash.h"
#include "tokenizador.h"
#include "stemmer.h"
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
    stopWords = unordered_set<string>();
    // Almacenar las stopWords en un unordered_set a partir del fichero de stopWords
    ifstream f(ficheroStopWords.c_str());
    if (!f)
    {
        cerr << "Error al abrir el fichero " << ficheroStopWords << endl;
    }
    else
    {
        string palabra;
        while (getline(f, palabra))
        {
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

/* Constructor para inicializar IndexadorHash a partir de una indexación
previamente realizada que habrá sido almacenada en
?directorioIndexacion? mediante el método ?bool GuardarIndexacion()?.
Con ello toda la parte privada se inicializará convenientemente, igual
que si se acabase de indexar la colección de documentos. En caso que no
exista el directorio o que no contenga los datos de la indexación se
tratará la excepción correspondiente*/
IndexadorHash::IndexadorHash(const string &directorioIndexacion)
{
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
    ficheroStopWords = orig.ficheroStopWords;
    tok = orig.tok;
    directorioIndice = orig.directorioIndice;
    tipoStemmer = orig.tipoStemmer;
    almacenarEnDisco = orig.almacenarEnDisco;
    almacenarPosTerm = orig.almacenarPosTerm;
    return *this;
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
        cout << "Error al abrir el fichero de documentos" << endl;
        return false;
    }
    string linea;
    while (getline(f, linea))
    {
        // Comprobar si el documento ya está indexado
        if (indiceDocs.find(linea) != indiceDocs.end())
        {
            // Comprobar si el documento ha sido modificado
            struct stat atributos;
            stat(linea.c_str(), &atributos);
            time_t fechaModificacion = atributos.st_mtime;
            // Si la fecha de modificación es anterior a la del documento indexado, no se indexa
            if (fechaModificacion > indiceDocs[linea].getFechaModificacion())
            {
                // Borrar el documento previamente indexado
                if (!BorraDoc(linea))
                    return false;
                // Indexar el nuevo manteniendo el idDoc
                if (!IndexarDoc(linea))
                    return false;
            }
        }
        else
        {
            // Indexar el nuevo
            IndexarDoc(linea);
        }
    }
}

bool IndexadorHash::IndexarDoc(const string &nomDoc)
{
    // Abrir el documento
    ifstream f(nomDoc.c_str());
    if (!f)
    {
        cout << "Error al abrir el fichero " << nomDoc << endl;
        return false;
    }
    // Obtener el idDoc
    int idDoc = indiceDocs.size();
    // Obtener la fecha de modificación
    struct stat atributos;
    stat(nomDoc.c_str(), &atributos);
    time_t fechaModificacion = atributos.st_mtime;
    // Tokenizar el documento
    if (tok.Tokenizar(nomDoc))
    {
        // Documento .tk que contiene un término por línea
        ifstream f(nomDoc + ".tk");
        string linea;
        // Inicializar stemmer
        stemmerPorter stemmer = stemmerPorter();
        while (getline(f, linea))
        {
            // Quitar el salto de línea
            string token = linea.substr(0, linea.size() - 1);
            // Comprobar si el término es una stopWord
            if (stopWords.find(linea) == stopWords.end())
            {
                // Aplicar el stemmer
                stemmer.stemmer(token, tipoStemmer);
                // Comprobar si el término está en el índice
                if (indice.find(token) != indice.end())
                {
                    // Obtener la información del término del índice
                    InformacionTermino infTerm = indice[token];
                    // Actualizar la información del término
                    infTerm.addFtc();
                    Actualiza(token, infTerm);
                }
                else
                {
                    // Añadir el término al índice
                    Inserta(token, InformacionTermino());
                }
            }
        }
    }
    else
    {
        cout << "Error al tokenizar el documento " << nomDoc << endl;
        return false;
    }
}

bool IndexadorHash::BorraDoc(const string &nomDoc)
{
    return false;
}

bool IndexadorHash::Actualiza(const string &termino, const InformacionTermino &infTerm)
{
    return false;
}

bool IndexadorHash::Inserta(const string &termino, const InformacionTermino &infTerm)
{
    return false;
}

/* Mostrará por pantalla las palabras de parada almacenadas (originales,
sin aplicar stemming): una palabra por línea (salto de línea al final de
cada palabra) Las stopWords están en la variable privada unordered_set stopWords*/
void IndexadorHash::ListarPalParada() const
{
    for (unordered_set<string>::const_iterator it = stopWords.begin(); it != stopWords.end(); ++it)
        cout << *it << "\n";
}