#include <sys/stat.h>
#include <unordered_set>
#include <dirent.h>
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

/*Constructor para inicializar IndexadorHash a partir de una indexación 
previamente realizada que habrá sido almacenada en 
?directorioIndexacion? mediante el método ?bool GuardarIndexacion()?. 
Con ello toda la parte privada se inicializará convenientemente, igual
que si se acabase de indexar la colección de documentos. En caso que no 
exista el directorio o que no contenga los datos de la indexación se 
tratará la excepción correspondiente
*/
IndexadorHash::IndexadorHash(const string &directorioIndexacion) {
    // Comprobamos que el directorio exista
    if (opendir(directorioIndexacion.c_str()) == NULL) {
        cerr << "El directorio " << directorioIndexacion << " no existe" << endl;
    }
    // Abrimos el fichero indexador.dat. Si no existe mostramos error.
    string ficheroIndexador = directorioIndexacion + "/indexador.dat";
    ifstream f(ficheroIndexador.c_str());
    if (!f) {
        cerr << "No se ha podido abrir el fichero " << ficheroIndexador << endl;
    }
    // Leemos el indice del fichero
    // La primera linea indica el tamaño del indice
    string linea;
    getline(f, linea);
    int tam = atoi(linea.c_str());
    // El resto tam lineas indican el termino y la serializacion de InformacionTermino
    for (int i = 0; i < tam; i++) {
        getline(f, linea);
        InformacionTermino it;
        it.Cargar(linea);
        indice[linea] = it;
    }
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
    return true;
}

bool IndexadorHash::IndexarDoc(const string &nomDoc)
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
    int idDoc = indiceDocs.size()+1;
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
        while (getline(f, linea))
        {
            
            // Comprobar si el término es una stopWord
            if (stopWords.find(linea) == stopWords.end())
            {
                numPalabrasSinStopWords++;
                // Aplicar el stemmer
                stemmer.stemmer(linea, tipoStemmer);
                // Comprobar si el término está en el índice
                if (indice.find(linea) != indice.end())
                {
                    // Obtener la información del término del índice
                    InformacionTermino infTerm = indice[linea];
                    // Actualizar la información del término
                    infTerm.addFtc();
                    // Comprobar si el término está en el documento (si existe el idDoc en el map)
                    if (infTerm.existIdDoc(idDoc))
                    {
                        // Obtener la información del término en el documento
                        InfTermDoc* infTermDoc = infTerm.getInfTermDoc(idDoc);
                        // Actualizar la información del término en el documento
                        infTermDoc->addFt();
                        infTermDoc->addPosTerm(numPalabras);
                    }
                    else
                    {
                        // Rellenar la información del término en el documento
                        InfTermDoc infTermDoc = InfTermDoc();
                        infTermDoc.addFt();
                        infTermDoc.addPosTerm(numPalabras);
                        // Actualizar la información del término en el documento
                        infTerm.addInfTermDoc(idDoc, infTermDoc);
                    }
                    Actualiza(linea, infTerm);
                }
                else
                {
                    numTotalPalDiferentes++;
                    // Rellenar la información del término en el documento
                    InfTermDoc infTermDoc = InfTermDoc();
                    infTermDoc.addFt();
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
            }
            numPalabras++;
        }
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
    if (indiceDocs.find(nomDoc) != indiceDocs.end()) {
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

/* Mostrará por pantalla las palabras de parada almacenadas (originales,
sin aplicar stemming): una palabra por línea (salto de línea al final de
cada palabra) Las stopWords están en la variable privada unordered_set stopWords*/
void IndexadorHash::ListarPalParada() const
{
    for (unordered_set<string>::const_iterator it = stopWords.begin(); it != stopWords.end(); ++it)
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
                cerr << "Error al abrir el fichero " << directorioIndice + "/indexador.dat" << "\n";
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
                f << it->second << "\n";
            }
            // Guardar el unordered_map indicePregunta
            f << indicePregunta.size() << "\n";
            for (auto it = indicePregunta.begin(); it != indicePregunta.end(); ++it)
            {
                f << it->first << "\n";
                f << it->second << "\n";
            }
            // Guardar el unordered_set stopWords
            f << stopWords.size() << "\n";
            for (auto it = stopWords.begin(); it != stopWords.end(); ++it)
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
            // Guardar el tipoStemmer
            f << tipoStemmer << "\n";
            // Guardar el almacenarEnDisco
            f << almacenarEnDisco << "\n";
            // Guardar el almacenarPosTerm
            f << almacenarPosTerm << "\n";
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
bool IndexadorHash::Devuelve(const string &word, const string &nomDoc, InfTermDoc &infDoc) const {
    InformacionTermino inf;
    int idDoc;
    if (indiceDocs.find(nomDoc) != indiceDocs.end())
        idDoc = indiceDocs.find(nomDoc)->second.getIdDoc();
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
    //cout << termino << "\t" << InformacionTermino << endl;
    //cout << "Documentos indexados: " << endl;
    /* A continuación aparecerá un listado del contenido del campo 
    privado ?indiceDocs? donde para cada documento indexado se imprimirá:*/
    for (unordered_map<string, InfDoc>::const_iterator it = indiceDocs.begin(); it != indiceDocs.end(); ++it)
        cout << it->first << "\t" << it->second << endl;
    //cout << nomDoc << "\t" << InfDoc << endl;

}
