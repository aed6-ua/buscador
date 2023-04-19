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
    friend ostream &operator<<(ostream &s, const IndexadorHash &p)
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

public:
    IndexadorHash(const string &fichStopWords, const string &delimitadores,
                  const bool &detectComp, const bool &minuscSinAcentos, const string &dirIndice, const int &tStemmer, const bool &almEnDisco, const bool &almPosTerm);
    IndexadorHash(const string &directorioIndexacion);
    IndexadorHash(const IndexadorHash &);
    ~IndexadorHash();
    IndexadorHash &operator=(const IndexadorHash &);
    bool Indexar(const string &ficheroDocumentos);

    bool IndexarDirectorio(const string &dirAIndexar);

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
    bool Devuelve(const string &word, InformacionTermino &inf) const;

    bool Devuelve(const string &word, const string &nomDoc, InfTermDoc &InfDoc)
        const;

    bool Existe(const string &word) const;

    bool Borra(const string &word);

    bool BorraDoc(const string &nomDoc);

    void VaciarIndiceDocs();

    void VaciarIndicePreg();

    bool Actualiza(const string &word, const InformacionTermino &inf);

    bool Inserta(const string &word, const InformacionTermino &inf);

    int NumPalIndexadas() const;

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

    void ListarInfColeccDocs() const;

    void ListarTerminos() const;

    bool ListarTerminos(const string &nomDoc) const;

    void ListarDocs() const;

    bool ListarDocs(const string &nomDoc) const;

    bool IndexarDoc(const string &nomDoc);

private:
    IndexadorHash();
    unordered_map<string, InformacionTermino> indice;

    unordered_map<string, InfDoc> indiceDocs;
    InfColeccionDocs informacionColeccionDocs;
    string pregunta;
    unordered_map<string, InformacionTerminoPregunta> indicePregunta;
    InformacionPregunta infPregunta;
    unordered_set<string> stopWords;
    string ficheroStopWords;
    Tokenizador tok;
    string directorioIndice;
    int tipoStemmer;
    bool almacenarEnDisco;
    bool almacenarPosTerm;
};

#endif