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

    bool Devuelve(const string &word, const string &nomDoc, InfTermDoc &InfDoc) const;

    // Devuelve true si word (aplic�ndole el tratamiento de stemming y may�sculas correspondiente) aparece como t�rmino indexado
    bool Existe(const string &word) const { return indice.find(word) != indice.end(); }

    bool Borra(const string &word);

    bool BorraDoc(const string &nomDoc);

    void VaciarIndiceDocs();

    void VaciarIndicePreg();

    /* Ser� true si word (aplic�ndole el tratamiento de stemming y 
    may�sculas correspondiente) est� indexado, sustituyendo la informaci�n 
    almacenada por ?inf?*/
    bool Actualiza(const string &word, const InformacionTermino &inf) { return Existe(word) ? indice[word] = inf, true : false; }

    /* Ser� true si se realiza la inserci�n (p.ej. si word, aplic�ndole el 
    tratamiento de stemming y may�sculas correspondiente, no estaba 
    previamente indexado)*/
    bool Inserta(const string &word, const InformacionTermino &inf) { return indice.insert(make_pair(word, inf)).second; }

    // Devolver� el n�mero de t�rminos diferentes indexados (cardinalidad de campo privado ?�ndice?)
    int NumPalIndexadas() const { return indice.size(); }

    // Devuelve el contenido del campo privado ?ficheroStopWords?
    string DevolverFichPalParada() const { return ficheroStopWords; }

    void ListarPalParada() const;

    // Devolver� el n�mero de palabras de parada almacenadas
    int NumPalParada() const { return stopWords.size(); }

    // Devuelve los delimitadores utilizados por el tokenizador
    string DevolverDelimitadores() const { return tok.DelimitadoresPalabra(); }

    // Devuelve si el tokenizador analiza los casos especiales
    bool DevolverCasosEspeciales() const { return tok.CasosEspeciales(); }

    // Devuelve si el tokenizador pasa a min�sculas y sin acentos
    bool DevolverPasarAminuscSinAcentos() const { return tok.PasarAminuscSinAcentos(); }

    // Devuelve el valor de almacenarPosTerm
    bool DevolverAlmacenarPosTerm() const { return almacenarPosTerm; }

    // Devuelve ?directorioIndice? (el directorio del disco duro donde se almacenar� el �ndice)
    string DevolverDirIndice() const { return directorioIndice; }

    // Devolver� el tipo de stemming realizado en la indexaci�n de acuerdo con el valor indicado en la variable privada ?tipoStemmer?
    int DevolverTipoStemming() const { return tipoStemmer; }

    // Devolver� el valor indicado en la variable privada ?almEnDisco?
    bool DevolverAlmEnDisco() const { return almacenarEnDisco; }

    // Mostrar por pantalla: cout << informacionColeccionDocs << endl;
    void ListarInfColeccDocs() const { cout << informacionColeccionDocs << endl; }

    void ListarTerminos() const;

    bool ListarTerminos(const string &nomDoc) const;

    // Mostrar por pantalla el contenido el contenido del campo privado ?indiceDocs?: cout << nomDoc << ?\t? << InfDoc << endl;
    void ListarDocs() const { for (auto it = indiceDocs.begin(); it != indiceDocs.end(); ++it) cout << it->first << '\t' << it->second << "\n"; }

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