#include "buscador.h"
#include <stdexcept>
#include <cmath>

ResultadoRI::ResultadoRI(const double &kvSimilitud, const long int &kidDoc,
                         const int &np)
{
    vSimilitud = kvSimilitud;
    idDoc = kidDoc;
    numPregunta = np;
}
double
ResultadoRI::VSimilitud() const
{
    return vSimilitud;
}
long int
ResultadoRI::IdDoc() const
{
    return idDoc;
}
bool ResultadoRI::operator<(const ResultadoRI &lhs) const
{
    if (numPregunta == lhs.numPregunta)
        return (vSimilitud < lhs.vSimilitud);
    else
        return (numPregunta > lhs.numPregunta);
}
ostream &operator<<(ostream &os, const ResultadoRI &res)
{
    os << res.vSimilitud << "\t\t" << res.idDoc << "\t" << res.numPregunta
       << endl;
    return os;
}

// BUSCADOR

ostream &operator<<(ostream &s, const Buscador &p)
{
    string preg;
    s << "Buscador : " << endl;
    if (p.DevuelvePregunta(preg))
        s << "\tPregunta indexada : " << preg << endl;
    else
        s << "\tNo hay ninguna pregunta indexada" << endl;
    s << "\tDatos del indexador : " << endl
      << (IndexadorHash)p;
    // Invoca a la sobrecarga de la salida del Indexador
    return s;
}

Buscador::Buscador(const string &directorioIndexacion, const int &f) : IndexadorHash(directorioIndexacion)
{
    if (f == 0 || f == 1)
    {
        formSimilitud = f;
    }
    else
    {
        throw std::invalid_argument("Valor incorrecto para la variable formSimilitud. Debe ser 0 (DFR) o 1 (BM25).");
    }

    // Inicializar constantes del modelo
    c = 2;
    k1 = 1.2;
    b = 0.75;
}

Buscador::Buscador(const Buscador &buscador) : IndexadorHash(buscador)
{
    try
    {
        formSimilitud = buscador.formSimilitud;
        c = buscador.c;
        k1 = buscador.k1;
        b = buscador.b;
        docsOrdenados = buscador.docsOrdenados;
    }
    catch (const std::exception &e)
    {
        cerr << "ERROR: " << e.what() << endl;
    }
}

Buscador::~Buscador()
{
    // No es necesario implementar nada en el destructor, ya que el destructor de la clase base maneja la liberaciÃ³n de memoria
}

Buscador &Buscador::operator=(const Buscador &buscador)
{
    if (this != &buscador)
    {
        try
        {
            IndexadorHash::operator=(buscador); // Llamar al operador de asignaciÃ³n de la clase base
            formSimilitud = buscador.formSimilitud;
            c = buscador.c;
            k1 = buscador.k1;
            b = buscador.b;
            docsOrdenados = buscador.docsOrdenados;
        }
        catch (const std::exception &e)
        {
            cerr << "ERROR: " << e.what() << endl;
        }
    }
    return *this;
}

bool Buscador::Buscar(const int& numDocumentos) {
    try {
        // Limpiar el vector docsOrdenados antes de empezar una nueva búsqueda
        docsOrdenados = priority_queue<ResultadoRI>();

        // Verificar si hay una pregunta indexada
        InformacionPregunta preguntaInf;
        if (!DevuelvePregunta(preguntaInf)) {
            cerr << "ERROR: No hay una pregunta indexada para buscar" << endl;
            return false;
        }

        string pregunta;
        DevuelvePregunta(pregunta);
        //Lista de terminos de pregunta
        unordered_map<string, InformacionTerminoPregunta> indicePregunta;
        DevolverIndicePregunta(indicePregunta);
        // Obtener la lista de documentos indexados
        unordered_map<string, InfDoc> indiceDocs;
        DevolverIndiceDocs(indiceDocs);

        // Obtener la lista de documentos indexados
        //vector<string> listaDocs = ListarDocs();

        // Calcular el valor de los documentos de acuerdo con la fórmula de similitud elegida
        for (const auto& doc : indiceDocs) {
            double valor = 0.0;
            //InfDoc docInf;
            //DevuelveInfoDoc(docID, docInf);

            // Recorrer los términos de la pregunta
            for (auto& palabra : indicePregunta) {
                InfTermDoc infTermDoc;

                // Si el término aparece en el documento actual
                if (Devuelve(palabra.first, doc.first, infTermDoc)) {
                    InformacionTermino terminoInf;
                    Devuelve(palabra.first, terminoInf);

                    double idf = log10((NumDocs() - infTermDoc.getFt() + 0.5) / (infTermDoc.getFt() + 0.5));
                    double tf = infTermDoc.getFt();
                    double docLength = doc.second.getNumPalSinParada();
                    double avgDocLength = informacionColeccionDocs.MediaPalSinParada();

                    if (formSimilitud == 0) {
                        valor += idf * (tf * (k1 + 1)) / (tf + k1 * (1 - b + b * docLength / avgDocLength));
                    } else {
                        valor += (tf / (tf + c)) * idf;
                    }
                }
            }

            // Almacenar el valor de similitud calculado para el documento actual
            if (valor > 0) {
                docsOrdenados.push_back(make_pair(docID, valor));
            }
        }

        // Ordenar la lista de documentos por sus valores de similitud en orden descendente
        sort(docsOrdenados.begin(), docsOrdenados.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
            return a.second > b.second;
        });

        // Recortar la lista de documentos a los numDocumentos más relevantes
        if (docsOrdenados.size() > numDocumentos) {
            docsOrdenados.resize(numDocumentos);
        }

        return true;
    } catch (...) {
        cerr << "ERROR: Hubo un error al realizar la búsqueda" << endl;
        return false;
    }
}

