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
    ResultadoRI(const double &kvSimilitud, const long int &kidDoc, const int &np, const string &nd);
    double VSimilitud() const;
    long int IdDoc() const;
    int NumPregunta() const { return numPregunta; }
    bool operator<(const ResultadoRI &lhs) const;
    bool operator>(const ResultadoRI &lhs) const;
    string NombreDoc() const { return nombreDoc; }

private:
    double vSimilitud;
    long int idDoc;
    int numPregunta;
    string nombreDoc;
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

    void ImprimirResultadoBusqueda(const int &numDocumentos = 99999);
    bool ImprimirResultadoBusqueda(const int &numDocumentos, const string &nombreFichero);

    int DevolverFormulaSimilitud() const { return formSimilitud; }
    bool CambiarFormulaSimilitud(const int &f)
    {
        if (f == 0 || f == 1)
        {
            formSimilitud = f;
            return true;
        }
        else
            return false;
    }

    void CambiarParametrosDFR(const double &kc);
    double DevolverParametrosDFR() const { return c; }

    void CambiarParametrosBM25(const double &kk1, const double &kb);
    void DevolverParametrosBM25(double &kk1, double &kb) const
    {
        kk1 = k1;
        kb = b;
    }

private:
    Buscador();

    //priority_queue<ResultadoRI> docsOrdenados;
    vector<ResultadoRI> docsOrdenadosVector;
    int formSimilitud;
    double c;
    double k1;
    double b;
};

#endif // BUSCADOR_H
