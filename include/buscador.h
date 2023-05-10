#ifndef BUSCADOR_H
#define BUSCADOR_H

#include <iostream>
#include <string>
#include <queue>
#include "indexadorHash.h"

using namespace std;
class ResultadoRI
{
    friend ostream &operator<<(ostream &, const ResultadoRI &);

public:
    ResultadoRI(const double &kvSimilitud, const long int &kidDoc, const int &np);
    double VSimilitud() const;
    long int IdDoc() const;
    bool operator<(const ResultadoRI &lhs) const;
private : 
    double vSimilitud;
    long int idDoc;
    int numPregunta;
};

class Buscador : public IndexadorHash
{
    friend ostream &operator<<(ostream &s, const Buscador &p);

public:
    Buscador(const string &directorioIndexacion, const int &f);
    Buscador(const Buscador &);
    ~Buscador();
    Buscador &operator=(const Buscador &);

    bool Buscar(const int &numDocumentos = 99999);
    bool Buscar(const string &dirPreguntas, const int &numDocumentos, const int &numPregInicio, const int &numPregFin);

    void ImprimirResultadoBusqueda(const int &numDocumentos = 99999) const;
    bool ImprimirResultadoBusqueda(const int &numDocumentos, const string &nombreFichero) const;

    int DevolverFormulaSimilitud() const;
    bool CambiarFormulaSimilitud(const int &f);

    void CambiarParametrosDFR(const double &kc);
    double DevolverParametrosDFR() const;

    void CambiarParametrosBM25(const double &kk1, const double &kb);
    void DevolverParametrosBM25(double &kk1, double &kb) const;

private:
    Buscador();

    priority_queue<ResultadoRI> docsOrdenados;
    int formSimilitud;
    double c;
    double k1;
    double b;
};

#endif // BUSCADOR_H
