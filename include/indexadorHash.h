#ifndef _INDEXADORHASH_H_
#define _INDEXADORHASH_H_

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "indexadorInformacion.h"
#include "tokenizador.h"

using namespace std;

class IndexadorHash
{
    friend ostream &operator<<(ostream &s, const IndexadorHash &p);

public:
    IndexadorHash(const string &fichStopWords, const string &delimitadores,
                  const bool &detectComp, const bool &minuscSinAcentos, const string &dirIndice, const int &tStemmer, const bool &almEnDisco, const bool &almPosTerm);
    IndexadorHash(const string &directorioIndexacion);
    IndexadorHash(const IndexadorHash &);
    ~IndexadorHash();
    IndexadorHash &operator=(const IndexadorHash &);
    bool Indexar(const string &ficheroDocumentos);

    bool IndexarDirectorio(const string &dirAIndexar);

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
    bool GuardarIndexacion() const;

    bool RecuperarIndexacion(const string &directorioIndexacion);

    void ImprimirIndexacion() const;
    bool IndexarPregunta(const string &preg);

    bool DevuelvePregunta(string &preg) const;

    bool DevuelvePregunta(const string &word, InformacionTerminoPregunta &inf)
        const;

    bool DevuelvePregunta(InformacionPregunta &inf) const;

    void ImprimirIndexacionPregunta();
    void ImprimirPregunta();

    /* Devuelve true si word (aplicándole el tratamiento de stemming y 
    mayúsculas correspondiente) está indexado, devolviendo su información 
    almacenada ?inf?. En caso que no esté, devolvería ?inf? vacío*/
    bool Devuelve(const string &word, InformacionTermino &inf) const { return Existe(word) ? inf = indice.at(word), true : false; }

    /* Devuelve true si word (aplicándole el tratamiento de stemming y 
    mayúsculas correspondiente) está indexado y aparece en el documento de 
    nombre nomDoc, en cuyo caso devuelve la información almacenada para word 
    en el documento. En caso que no esté, devolvería ?InfDoc? vacío*/
    bool Devuelve(const string &word, const string &nomDoc, InfTermDoc &infDoc) const;

    // Devuelve true si word (aplicándole el tratamiento de stemming y mayúsculas correspondiente) aparece como término indexado
    bool Existe(const string &word) const { return indice.find(word) != indice.end(); }

    bool Borra(const string &word);

    bool BorraDoc(const string &nomDoc);

    void VaciarIndiceDocs();

    void VaciarIndicePreg();

    /* Será true si word (aplicándole el tratamiento de stemming y
    mayúsculas correspondiente) está indexado, sustituyendo la información
    almacenada por ?inf?*/
    bool Actualiza(const string &word, const InformacionTermino &inf) { return Existe(word) ? indice[word] = inf, true : false; }

    /* Será true si se realiza la inserción (p.ej. si word, aplicándole el
    tratamiento de stemming y mayúsculas correspondiente, no estaba
    previamente indexado)*/
    bool Inserta(const string &word, const InformacionTermino &inf) { return indice.insert(make_pair(word, inf)).second; }

    // Devolverá el número de términos diferentes indexados (cardinalidad de campo privado ?índice?)
    int NumPalIndexadas() const { return indice.size(); }

    // Devuelve el contenido del campo privado ?ficheroStopWords?
    string DevolverFichPalParada() const { return ficheroStopWords; }

    void ListarPalParada() const;

    // Devolverá el número de palabras de parada almacenadas
    int NumPalParada() const { return stopWords.size(); }

    // Devuelve los delimitadores utilizados por el tokenizador
    string DevolverDelimitadores() const { return tok.DelimitadoresPalabra(); }

    // Devuelve si el tokenizador analiza los casos especiales
    bool DevolverCasosEspeciales() const { return tok.CasosEspeciales(); }

    // Devuelve si el tokenizador pasa a minúsculas y sin acentos
    bool DevolverPasarAminuscSinAcentos() const { return tok.PasarAminuscSinAcentos(); }

    // Devuelve el valor de almacenarPosTerm
    bool DevolverAlmacenarPosTerm() const { return almacenarPosTerm; }

    // Devuelve ?directorioIndice? (el directorio del disco duro donde se almacenará el índice)
    string DevolverDirIndice() const { return directorioIndice; }

    // Devolverá el tipo de stemming realizado en la indexación de acuerdo con el valor indicado en la variable privada ?tipoStemmer?
    int DevolverTipoStemming() const { return tipoStemmer; }

    // Devolverá el valor indicado en la variable privada ?almEnDisco?
    bool DevolverAlmEnDisco() const { return almacenarEnDisco; }

    // Mostrar por pantalla: cout << informacionColeccionDocs << "\n";
    void ListarInfColeccDocs() const { cout << informacionColeccionDocs << "\n"; }

    void ListarTerminos() const;

    bool ListarTerminos(const string &nomDoc) const;

    // Mostrar por pantalla el contenido el contenido del campo privado ?indiceDocs?: cout << nomDoc << ?\t? << InfDoc << "\n";
    void ListarDocs() const
    {
        for (auto it = indiceDocs.begin(); it != indiceDocs.end(); ++it)
            cout << it->first << '\t' << it->second << "\n";
    }

    bool ListarDocs(const string &nomDoc) const;

    bool IndexarDoc(const string &nomDoc);

private:
    IndexadorHash();
    unordered_map<string, InformacionTermino> indice;
    unordered_map<string, InfDoc> indiceDocs;
    unordered_map<string, InformacionTerminoPregunta> indicePregunta;
    unordered_set<string> stopWords;
    InfColeccionDocs informacionColeccionDocs;
    string pregunta;
    InformacionPregunta infPregunta;
    string ficheroStopWords;
    Tokenizador tok;
    string directorioIndice;
    int tipoStemmer;
    bool almacenarEnDisco;
    bool almacenarPosTerm;
};

#endif