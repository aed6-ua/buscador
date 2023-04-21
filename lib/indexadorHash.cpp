#include <sys/stat.h>
#include <unordered_set>
#include <dirent.h>
#include "indexadorHash.h"
#include "tokenizador.h"
#include "stemmer.h"
using namespace std;

// IndexadorHash
/* ?fichStopWords? ser� el nombre del archivo que contendr� todas las
palabras de parada (una palabra por cada l�nea del fichero) y se
almacenar� en el campo privado ?ficheroStopWords?. Asimismo, almacenar�
todas las palabras de parada que contenga el archivo en el campo privado
?stopWords?, el �ndice de palabras de parada.
?delimitadores? ser� el string que contiene todos los delimitadores
utilizados por el tokenizador (campo privado ?tok?)
detectComp y minuscSinAcentos ser�n los par�metros que se pasar�n al
tokenizador
?dirIndice? ser� el directorio del disco duro donde se almacenar� el
�ndice (campo privado ?directorioIndice?). Si dirIndice=?? entonces se
almacenar� en el directorio donde se ejecute el programa
?tStemmer? inicializar� la variable privada ?tipoStemmer?:
0 = no se aplica stemmer: se indexa el t�rmino tal y como
aparece tokenizado
1 = stemmer de Porter para espa�ol
2 = stemmer de Porter para ingl�s
?almEnDisco? inicializar� la variable privada ?almacenarEnDisco?
?almPosTerm? inicializar� la variable privada ?almacenarPosTerm?
Los �ndices (p.ej. �ndice, indiceDocs e informacionColeccionDocs)
quedar�n vac�os*/

IndexadorHash::IndexadorHash()
{
    indice = unordered_map<string, InformacionTermino>();
    // �ndice de t�rminos indexados accesible por el t�rmino
    indiceDocs = unordered_map<string, InfDoc>();
    // �ndice de documentos indexados accesible por el nombre del documento
    informacionColeccionDocs = InfColeccionDocs();
    // Informaci�n recogida de la colecci�n de documentos indexada
    pregunta = "";
    // Pregunta indexada actualmente. Si no hay ninguna indexada, contendr�a el valor ??
    indicePregunta = unordered_map<string, InformacionTerminoPregunta>();
    // �ndice de t�rminos indexados en una pregunta. Se almacenar� en memoria principal
    infPregunta = InformacionPregunta();
    // Informaci�n recogida de la pregunta indexada. Se almacenar� en memoria principal
    stopWords = unordered_set<string>();
    /* Palabras de parada. Se almacenar� en memoria principal. El filtrado
    de palabras de parada se realizar�, tanto en la pregunta como en los
    documentos, teniendo en cuenta el par�metro minuscSinAcentos y
    tipoStemmer. Es decir que se aplicar� el mismo proceso a las palabras de
    parada almacenadas en el fichero antes de realizar el filtrado (p.ej. si
    se aplica el pasar a min�sculas los t�rminos del documento/pregunta a
    indexar, para comprobar si se ha de eliminar el t�rmino, �ste se
    comparar� con la versi�n de palabras de parada en min�sculas). Esto se
    pide as� para casos en los que en el documento/pregunta aparezca: ?La
    casa de �l? y est�n almacenadas como stopWords ?la, el?, si se activa el
    par�metro minuscSinAcentos, entonces deber�a filtrar ?La, �l?, si no
    hubiese estado activo ese par�metro, entonces no se hubiesen filtrado.*/
    ficheroStopWords = "";
    // Nombre del fichero que contiene las palabras de parada
    tok = Tokenizador();
    /* Tokenizador que se usar� en la indexaci�n. Se inicializar� con los
    par�metros del constructor: detectComp y minuscSinAcentos, los cuales
    determinar�n qu� t�rmino se ha de indexar (p.ej. si se activa
    minuscSinAcentos, entonces se guardar�n los t�rminos en min�sculas y sin
    acentos)*/
    directorioIndice = "";
    /* ?directorioIndice? ser� el directorio del disco duro donde se
    almacenar� el �ndice. En caso que contenga la cadena vac�a se crear� en
    el directorio donde se ejecute el indexador*/
    tipoStemmer = 0;
    /* 0 = no se aplica stemmer: se indexa el t�rmino tal y como aparece
    tokenizado
    // Los siguientes valores har�n que los t�rminos a indexar se les
    aplique el stemmer y se almacene solo dicho stem.
    // 1 = stemmer de Porter para espa�ol
    // 2 = stemmer de Porter para ingl�s
    // Para el stemmer de Porter se utilizar�n los archivos
    stemmer.cpp y stemmer.h, concretamente las funciones de nombre
    ?stemmer?*/
    almacenarEnDisco = false;
    /* Esta opci�n (cuando almacenarEnDisco == true) est� ideada para poder
    indexar colecciones de documentos lo suficientemente grandes para que su
    indexaci�n no quepa en memoria, por lo que si es true, mientras se va
    generando el �ndice, se almacenar� la m�nima parte de los �ndices de los
    documentos en memoria principal, p.ej. solo la estructura ?indice? para
    saber las palabras indexadas, guardando �nicamente punteros a las
    posiciones de los archivos almacenados en disco que contendr�n el resto
    de informaci�n asociada a cada t�rmino (lo mismo para indiceDocs). Se
    valorar� el equilibrio para conseguir una indexaci�n independientemente
    del tama�o del corpus a indexar, pero reduciendo el n�mero de accesos a
    disco. Para los datos de la indexaci�n de la pregunta no habr�a que
    hacer nada. En caso de que esta variable tenga valor false, se
    almacenar� todo el �ndice en memoria principal (tal y como se ha
    descrito anteriormente).*/
    almacenarPosTerm = false;
    // Si es true se almacenar� la posici�n en la que aparecen los t�rminos dentro del documento en la clase InfTermDoc
}

IndexadorHash::IndexadorHash(const string &fichStopWords, const string &delimitadores,
                             const bool &detectComp, const bool &minuscSinAcentos, const string &dirIndice, const int &tStemmer, const bool &almEnDisco, const bool &almPosTerm)
{
    indice = unordered_map<string, InformacionTermino>();
    // �ndice de t�rminos indexados accesible por el t�rmino
    indiceDocs = unordered_map<string, InfDoc>();
    // �ndice de documentos indexados accesible por el nombre del documento
    informacionColeccionDocs = InfColeccionDocs();
    // Informaci�n recogida de la colecci�n de documentos indexada
    pregunta = "";
    // Pregunta indexada actualmente. Si no hay ninguna indexada, contendr�a el valor ??
    indicePregunta = unordered_map<string, InformacionTerminoPregunta>();
    // �ndice de t�rminos indexados en una pregunta. Se almacenar� en memoria principal
    infPregunta = InformacionPregunta();
    // Informaci�n recogida de la pregunta indexada. Se almacenar� en memoria principal

    ficheroStopWords = fichStopWords;
    // Nombre del fichero que contiene las palabras de parada

    /* Palabras de parada. Se almacenar� en memoria principal. El filtrado
    de palabras de parada se realizar�, tanto en la pregunta como en los
    documentos, teniendo en cuenta el par�metro minuscSinAcentos y
    tipoStemmer. Es decir que se aplicar� el mismo proceso a las palabras de
    parada almacenadas en el fichero antes de realizar el filtrado (p.ej. si
    se aplica el pasar a min�sculas los t�rminos del documento/pregunta a
    indexar, para comprobar si se ha de eliminar el t�rmino, �ste se
    comparar� con la versi�n de palabras de parada en min�sculas). Esto se
    pide as� para casos en los que en el documento/pregunta aparezca: ?La
    casa de �l? y est�n almacenadas como stopWords ?la, el?, si se activa el
    par�metro minuscSinAcentos, entonces deber�a filtrar ?La, �l?, si no
    hubiese estado activo ese par�metro, entonces no se hubiesen filtrado.
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
    /* Tokenizador que se usar� en la indexaci�n. Se inicializar� con los
    par�metros del constructor: detectComp y minuscSinAcentos, los cuales
    determinar�n qu� t�rmino se ha de indexar (p.ej. si se activa
    minuscSinAcentos, entonces se guardar�n los t�rminos en min�sculas y sin
    acentos)*/
    directorioIndice = dirIndice;
    /* ?directorioIndice? ser� el directorio del disco duro donde se
    almacenar� el �ndice. En caso que contenga la cadena vac�a se crear� en
    el directorio donde se ejecute el indexador*/
    tipoStemmer = tStemmer;
    /* 0 = no se aplica stemmer: se indexa el t�rmino tal y como aparece
    tokenizado
    // Los siguientes valores har�n que los t�rminos a indexar se les
    aplique el stemmer y se almacene solo dicho stem.
    // 1 = stemmer de Porter para espa�ol
    // 2 = stemmer de Porter para ingl�s
    // Para el stemmer de Porter se utilizar�n los archivos
    stemmer.cpp y stemmer.h, concretamente las funciones de nombre
    ?stemmer?*/
    almacenarEnDisco = almEnDisco;
    /* Esta opci�n (cuando almacenarEnDisco == true) est� ideada para poder
    indexar colecciones de documentos lo suficientemente grandes para que su
    indexaci�n no quepa en memoria, por lo que si es true, mientras se va
    generando el �ndice, se almacenar� la m�nima parte de los �ndices de los
    documentos en memoria principal, p.ej. solo la estructura ?indice? para
    saber las palabras indexadas, guardando �nicamente punteros a las
    posiciones de los archivos almacenados en disco que contendr�n el resto
    de informaci�n asociada a cada t�rmino (lo mismo para indiceDocs). Se
    valorar� el equilibrio para conseguir una indexaci�n independientemente
    del tama�o del corpus a indexar, pero reduciendo el n�mero de accesos a
    disco. Para los datos de la indexaci�n de la pregunta no habr�a que
    hacer nada. En caso de que esta variable tenga valor false, se
    almacenar� todo el �ndice en memoria principal (tal y como se ha
    descrito anteriormente).*/
    almacenarPosTerm = almPosTerm;
    // Si es true se almacenar� la posici�n en la que aparecen los t�rminos dentro del documento en la clase InfTermDoc
}

/* Constructor para inicializar IndexadorHash a partir de una indexaci�n
previamente realizada que habr� sido almacenada en
?directorioIndexacion? mediante el m�todo ?bool GuardarIndexacion()?.
Con ello toda la parte privada se inicializar� convenientemente, igual
que si se acabase de indexar la colecci�n de documentos. En caso que no
exista el directorio o que no contenga los datos de la indexaci�n se
tratar� la excepci�n correspondiente*/
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

/*Constructor para inicializar IndexadorHash a partir de una indexaci�n 
previamente realizada que habr� sido almacenada en 
?directorioIndexacion? mediante el m�todo ?bool GuardarIndexacion()?. 
Con ello toda la parte privada se inicializar� convenientemente, igual
que si se acabase de indexar la colecci�n de documentos. En caso que no 
exista el directorio o que no contenga los datos de la indexaci�n se 
tratar� la excepci�n correspondiente
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
    // La primera linea indica el tama�o del indice
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

/* Devuelve true si nomDoc existe en la colecci�n y muestra por pantalla
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

/* Devuelve true si consigue crear el �ndice para la colecci�n de
    documentos detallada en ficheroDocumentos, el cual contendr� un nombre
    de documento por l�nea. Los a�adir� a los ya existentes anteriormente en
    el �ndice.
    // Devuelve falso si no finaliza la indexaci�n (p.ej. por falta de
    memoria), mostrando el mensaje de error correspondiente, indicando el
    documento y t�rmino en el que se ha quedado, dejando en memoria lo que
    se haya indexado hasta ese momento.
    // En el caso que aparezcan documentos repetidos, documentos que no
    existen o que ya estuviesen previamente indexados (ha de coincidir el
    nombre del documento y el directorio en que se encuentre), se devolver�
    true, mostrando el mensaje de excepci�n correspondiente, y se reindexar�n (borrar el documento previamente indexado e indexar el nuevo)
    en caso que la fecha de modificaci�n del documento sea m�s reciente que
    la almacenada previamente (class ?InfDoc? campo ?fechaModificacion?).
    Los casos de reindexaci�n mantendr�n el mismo idDoc.*/

bool IndexadorHash::Indexar(const string &ficheroDocumentos)
{
    /*Devuelve true si consigue crear el �ndice para la colecci�n de
    documentos detallada en ficheroDocumentos, el cual contendr� un nombre
    de documento por l�nea. Los a�adir� a los ya existentes anteriormente en
    el �ndice.*/
    ifstream f(ficheroDocumentos.c_str());
    if (!f)
    {
        cerr << "Error al abrir el fichero de documentos " << ficheroDocumentos << endl;
        return false;
    }
    string linea;
    while (getline(f, linea))
    {
        // Comprobar si el documento ya est� indexado
        if (indiceDocs.find(linea) != indiceDocs.end())
        {
            // Comprobar si el documento ha sido modificado
            struct stat atributos;
            stat(linea.c_str(), &atributos);
            time_t fechaModificacion = atributos.st_mtime;
            // Si la fecha de modificaci�n es anterior a la del documento indexado, no se indexa
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
    // Obtener la fecha de modificaci�n
    struct stat atributos;
    stat(nomDoc.c_str(), &atributos);
    time_t fechaModificacion = atributos.st_mtime;
    // Tokenizar el documento
    if (tok.Tokenizar(nomDoc))
    {
        // Documento .tk que contiene un t�rmino por l�nea
        ifstream f(nomDoc + ".tk");
        if (!f)
        {
            cerr << "Error al abrir el fichero de tokens " << nomDoc << ".tk" << endl;
            return false;
        }
        string linea;
        // Inicializar stemmer
        stemmerPorter stemmer = stemmerPorter();

        // N�mero de palabras del documento
        int numPalabras = 0;
        // N�mero de palabras que no son stopWords
        int numPalabrasSinStopWords = 0;
        // N�mero de palabras distintas del documento
        int numPalabrasDistintas = 0;
        // N�mero de t�rminos distintos total
        int numTotalPalDiferentes = 0;

        // Palabras distintas del documento
        unordered_set<string> palabrasDistintas;
        while (getline(f, linea))
        {
            
            // Comprobar si el t�rmino es una stopWord
            if (stopWords.find(linea) == stopWords.end())
            {
                numPalabrasSinStopWords++;
                // Aplicar el stemmer
                stemmer.stemmer(linea, tipoStemmer);
                // Comprobar si el t�rmino est� en el �ndice
                if (indice.find(linea) != indice.end())
                {
                    // Obtener la informaci�n del t�rmino del �ndice
                    InformacionTermino infTerm = indice[linea];
                    // Actualizar la informaci�n del t�rmino
                    infTerm.addFtc();
                    // Comprobar si el t�rmino est� en el documento (si existe el idDoc en el map)
                    if (infTerm.existIdDoc(idDoc))
                    {
                        // Obtener la informaci�n del t�rmino en el documento
                        InfTermDoc* infTermDoc = infTerm.getInfTermDoc(idDoc);
                        // Actualizar la informaci�n del t�rmino en el documento
                        infTermDoc->addFt();
                        infTermDoc->addPosTerm(numPalabras);
                    }
                    else
                    {
                        // Rellenar la informaci�n del t�rmino en el documento
                        InfTermDoc infTermDoc = InfTermDoc();
                        infTermDoc.addFt();
                        infTermDoc.addPosTerm(numPalabras);
                        // Actualizar la informaci�n del t�rmino en el documento
                        infTerm.addInfTermDoc(idDoc, infTermDoc);
                    }
                    Actualiza(linea, infTerm);
                }
                else
                {
                    numTotalPalDiferentes++;
                    // Rellenar la informaci�n del t�rmino en el documento
                    InfTermDoc infTermDoc = InfTermDoc();
                    infTermDoc.addFt();
                    infTermDoc.addPosTerm(numPalabras);
                    // Rellenar la informaci�n del t�rmino
                    InformacionTermino infTerm = InformacionTermino(1);
                    infTerm.addInfTermDoc(idDoc, infTermDoc);
                    // A�adir el t�rmino al �ndice
                    Inserta(linea, infTerm);
                }
                // Comprobar si la palabra es �nica en el documento
                if (palabrasDistintas.find(linea) == palabrasDistintas.end())
                {
                    // A�adir la palabra al conjunto de palabras distintas del documento
                    palabrasDistintas.insert(linea);
                    numPalabrasDistintas++;
                }
            }
            numPalabras++;
        }
        // Obtener fecha de modificaci�n del documento y tama�o en bytes
        struct stat atributos;
        stat(nomDoc.c_str(), &atributos);
        int tamBytes = atributos.st_size;
        time_t fechaModificacion = atributos.st_mtime;
        // A�adir el documento al �ndice de documentos
        indiceDocs.insert(make_pair(nomDoc, InfDoc(informacionColeccionDocs.getNumDocs() + 1, 
                                                    numPalabras, numPalabrasSinStopWords, numPalabrasDistintas, 
                                                    tamBytes, fechaModificacion)));
        // Actualizar la informaci�n de la colecci�n de documentos
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

/* Devuelve true si nomDoc est� indexado y se realiza el borrado de
todos los t�rminos del documento y del documento en los campos privados 
?indiceDocs? e ?informacionColeccionDocs?*/
bool IndexadorHash::BorraDoc(const string &nomDoc)
{
    // Comprobar si el documento est� indexado
    if (indiceDocs.find(nomDoc) != indiceDocs.end()) {
        // Obtener el idDoc
        int idDoc = indiceDocs.find(nomDoc)->second.getIdDoc();
        // Obtener la informaci�n del documento
        InfDoc infDoc = indiceDocs.find(nomDoc)->second;
        // Borrar el documento del �ndice de documentos
        indiceDocs.erase(nomDoc);
        // Actualizar la informaci�n de la colecci�n de documentos
        informacionColeccionDocs.subDoc();
        informacionColeccionDocs.subNumTotalPal(infDoc.getNumPal());
        informacionColeccionDocs.subNumTotalPalSinParada(infDoc.getNumPalSinParada());
        informacionColeccionDocs.subTamBytes(infDoc.getTamBytes());
        // Borrar los t�rminos del documento del �ndice
        auto it = indice.begin();
        while (it != indice.end())
        {
            // Obtener la informaci�n del t�rmino
            InformacionTermino infTerm = it->second;
            // Comprobar si el t�rmino est� en el documento
            if (infTerm.existIdDoc(idDoc))
            {
                // Borrar el t�rmino del documento
                infTerm.eraseInfTermDoc(idDoc);
                // Comprobar si el t�rmino est� en otros documentos
                if (infTerm.getNumDocs() > 0)
                {
                    // Actualizar el t�rmino en el �ndice
                    indice[it->first] = infTerm;
                }
                else
                {
                    // Borrar el t�rmino del �ndice
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

/* Mostrar� por pantalla las palabras de parada almacenadas (originales,
sin aplicar stemming): una palabra por l�nea (salto de l�nea al final de
cada palabra) Las stopWords est�n en la variable privada unordered_set stopWords*/
void IndexadorHash::ListarPalParada() const
{
    for (unordered_set<string>::const_iterator it = stopWords.begin(); it != stopWords.end(); ++it)
        cout << *it << "\n";
}

/* Se guardar� en disco duro (directorio contenido en la variable
privada ?directorioIndice?) la indexaci�n actualmente en memoria
(incluidos todos los par�metros de la parte privada). La forma de
almacenamiento la determinar� el alumno. El objetivo es que esta
indexaci�n se pueda recuperar posteriormente mediante el constructor
?IndexadorHash(const string& directorioIndexacion)?. Por ejemplo,
supongamos que se ejecuta esta secuencia de comandos: ?IndexadorHash
a(?./fichStopWords.txt?, ?[ ,.?, ?./dirIndexPrueba?, 0, false);
a.Indexar(?./fichConDocsAIndexar.txt?); a.GuardarIndexacion();?,
entonces mediante el comando: ?IndexadorHash b(?./dirIndexPrueba?);? se
recuperar� la indexaci�n realizada en la secuencia anterior, carg�ndola
en ?b?
// Devuelve falso si no finaliza la operaci�n (p.ej. por falta de
memoria, o el nombre del directorio contenido en ?directorioIndice? no
es correcto), mostrando el mensaje de error correspondiente, vaciando
los ficheros generados.
// En caso que no existiese el directorio directorioIndice, habr�a que
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
    /* A continuaci�n aparecer� un listado del contenido del campo 
    privado ?�ndice? donde para cada t�rmino indexado se imprimir�: */
    for (unordered_map<string, InformacionTermino>::const_iterator it = indice.begin(); it != indice.end(); ++it)
        cout << it->first << "\t" << it->second << endl;
    //cout << termino << "\t" << InformacionTermino << endl;
    //cout << "Documentos indexados: " << endl;
    /* A continuaci�n aparecer� un listado del contenido del campo 
    privado ?indiceDocs? donde para cada documento indexado se imprimir�:*/
    for (unordered_map<string, InfDoc>::const_iterator it = indiceDocs.begin(); it != indiceDocs.end(); ++it)
        cout << it->first << "\t" << it->second << endl;
    //cout << nomDoc << "\t" << InfDoc << endl;

}
