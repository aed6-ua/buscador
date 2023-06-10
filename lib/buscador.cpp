#include "buscador.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <sstream>

ResultadoRI::ResultadoRI(const double &kvSimilitud, const long int &kidDoc,
                         const int &np, const string &nd)
{
    vSimilitud = kvSimilitud;
    idDoc = kidDoc;
    numPregunta = np;
    nombreDoc = nd;
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
bool ResultadoRI::operator>(const ResultadoRI &lhs) const
{
    if (numPregunta == lhs.numPregunta)
        return (vSimilitud > lhs.vSimilitud);
    else
        return (numPregunta < lhs.numPregunta);
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

Buscador::Buscador(const string &directorioVectores, const int &f, const bool &b) : IndexadorHash(directorioVectores, b)
{
    if (f == 3)
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
    this->b = 0.75;
    embeddingsActivo = true;
}

Buscador::Buscador(const Buscador &buscador) : IndexadorHash(buscador)
{
    try
    {
        formSimilitud = buscador.formSimilitud;
        c = buscador.c;
        k1 = buscador.k1;
        b = buscador.b;
        // docsOrdenados = buscador.docsOrdenados;
        docsOrdenadosVector = buscador.docsOrdenadosVector;
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
            // docsOrdenados = buscador.docsOrdenados;
            docsOrdenadosVector = buscador.docsOrdenadosVector;
        }
        catch (const std::exception &e)
        {
            cerr << "ERROR: " << e.what() << endl;
        }
    }
    return *this;
}

bool Buscador::Buscar(const int &numDocumentos)
{
    try
    {
        // Reservar memoria para el vector de resultados
        docsOrdenadosVector.reserve(numDocumentos);
        // Limpiar el vector docsOrdenados antes de empezar una nueva búsqueda
        // docsOrdenados = priority_queue<ResultadoRI>();
        docsOrdenadosVector.clear();

        // Verificar si hay una pregunta indexada
        InformacionPregunta preguntaInf;
        if (!DevuelvePregunta(preguntaInf))
        {
            cerr << "ERROR: No hay una pregunta indexada para buscar" << endl;
            return false;
        }

        string pregunta;
        DevuelvePregunta(pregunta);
        // Lista de terminos de pregunta
        unordered_map<string, InformacionTerminoPregunta> indicePregunta;
        DevolverIndicePregunta(indicePregunta);
        // Obtener la lista de documentos indexados
        unordered_map<string, InfDoc> indiceDocs;
        DevolverIndiceDocs(indiceDocs);
        int numTotalPal = NumTotalPalSinParada();
        int numDocs = NumDocs();
        double avgDocLength = (double)numTotalPal / (double)numDocs;

        // Obtener la lista de documentos indexados
        // vector<string> listaDocs = ListarDocs();

        // Calcular el valor de los documentos de acuerdo con la fórmula de similitud elegida
        for (const auto &doc : indiceDocs)
        {
            double valor = 0.0;
            // InfDoc docInf;
            // DevuelveInfoDoc(docID, docInf);

            // Recorrer los términos de la pregunta
            for (auto &palabra : indicePregunta)
            {

                InformacionTermino *terminoInf;

                // Si el término está indexado
                if (Devuelve(palabra.first, &terminoInf))
                {

                    // Obtener la información del término en el documento
                    if (terminoInf->existIdDoc(doc.second.getIdDoc()))
                    {
                        InfTermDoc *infTermDoc;
                        infTermDoc = terminoInf->getInfTermDoc(doc.second.getIdDoc());
                        double docLength = doc.second.getNumPalSinParada();
                        double termNumDocs = terminoInf->getNumDocs();

                        // Formula DFR
                        if (formSimilitud == 0)
                        {
                            // peso en la query del término palabra de la query (frecuencia del término en la query entre el número de términos de la query)
                            double w_tq = (double)palabra.second.getFt() / (double)preguntaInf.getNumTotalPalSinParada();
                            // peso en el documento del término palabra
                            double ftc = terminoInf->getFtc();
                            double lambda = ftc / (double)numDocs;
                            double f_td = infTermDoc->getFt() * log2(1 + (c * avgDocLength) / docLength);
                            double w_td = (log2(1 + lambda) + f_td * log2((1 + lambda) / lambda)) * ((ftc + 1) / (termNumDocs * (f_td + 1)));
                            valor += w_tq * w_td;
                        }
                        else
                        {
                            // Formula BM25
                            double idf = log2((numDocs - termNumDocs + 0.5) / (termNumDocs + 0.5));
                            double f_qd = infTermDoc->getFt();
                            double aux2 = (double)(f_qd * (k1 + 1)) / (double)(f_qd + k1 * (1 - b + b * ((double)docLength / (double)avgDocLength)));
                            valor += idf * aux2;
                        }
                    }
                }
            }
            // Almacenar el valor de similitud calculado para el documento actual
            // docsOrdenados.push(ResultadoRI(valor, doc.second.getIdDoc(), 0, doc.first));
            // docsOrdenadosVector.push_back(ResultadoRI(valor, doc.second.getIdDoc(), 0, doc.first));
            docsOrdenadosVector.emplace_back(valor, doc.second.getIdDoc(), 0, doc.first);
        }
        // Ordenar los documentos del vector docsOrdenadosVector de menor a mayor
        sort(docsOrdenadosVector.begin(), docsOrdenadosVector.end(), greater<ResultadoRI>());
        // Truncar el vector docsOrdenadosVector para que tenga el tamaño numDocumentos
        if (numDocumentos < docsOrdenadosVector.size())
            docsOrdenadosVector.erase(docsOrdenadosVector.begin() + numDocumentos, docsOrdenadosVector.end());
        return true;
    }
    catch (...)
    {
        cerr << "ERROR: Hubo un error al realizar la búsqueda" << endl;
        return false;
    }
}

bool Buscador::Buscar(const string &dirPreguntas, const int &numDocumentos, const int &numPregInicio, const int &numPregFin)
{

    try
    {
        // Limpiar el vector docsOrdenados antes de empezar una nueva búsqueda
        // docsOrdenados = priority_queue<ResultadoRI>();
        docsOrdenadosVector.clear();
        // Reservar memoria para el vector de resultados
        docsOrdenadosVector.reserve(numDocumentos * (numPregFin - numPregInicio + 1));
        // Obtener la lista de documentos indexados
        unordered_map<string, InfDoc> indiceDocs;
        DevolverIndiceDocs(indiceDocs);
        int numTotalPal = NumTotalPal();
        int numDocs = NumDocs();
        double avgDocLength = numTotalPal / numDocs;

        // Obtener la lista de documentos indexados
        // vector<string> listaDocs = ListarDocs();
        vector<ResultadoRI> docsOrdenadosVectorAux;
        docsOrdenadosVectorAux.reserve(numDocumentos);
        // Calcular el valor de los documentos de acuerdo con la fórmula de similitud elegida
        for (int i = numPregInicio; i <= numPregFin; i++)
        {
            docsOrdenadosVectorAux.clear();
            string pregunta;
            string nombrePregunta = dirPreguntas + to_string(i) + ".txt";
            ifstream f(nombrePregunta);
            if (!f.is_open())
            {
                cerr << "ERROR: No se pudo abrir el fichero " << nombrePregunta << endl;
                return false;
            }
            getline(f, pregunta);
            f.close();
            // Indexar la pregunta
            IndexarPregunta(pregunta);
            InformacionPregunta preguntaInf;
            DevuelvePregunta(preguntaInf);
            // Realizar la búsqueda
            // Lista de terminos de pregunta
            unordered_map<string, InformacionTerminoPregunta> indicePregunta;
            DevolverIndicePregunta(indicePregunta);
            // Obtener la lista de documentos indexados
            unordered_map<string, InfDoc> indiceDocs;
            DevolverIndiceDocs(indiceDocs);
            int numTotalPal = NumTotalPalSinParada();
            int numDocs = NumDocs();
            double avgDocLength = (double)numTotalPal / (double)numDocs;

            // Obtener la lista de documentos indexados
            // vector<string> listaDocs = ListarDocs();

            // Calcular el valor de los documentos de acuerdo con la fórmula de similitud elegida
            for (const auto &doc : indiceDocs)
            {
                double valor = 0.0;
                // InfDoc docInf;
                // DevuelveInfoDoc(docID, docInf);

                // Recorrer los términos de la pregunta
                for (auto &palabra : indicePregunta)
                {

                    InformacionTermino *terminoInf;

                    // Si el término está indexado
                    if (Devuelve(palabra.first, &terminoInf))
                    {

                        // Obtener la información del término en el documento
                        if (terminoInf->existIdDoc(doc.second.getIdDoc()))
                        {
                            InfTermDoc *infTermDoc;
                            infTermDoc = terminoInf->getInfTermDoc(doc.second.getIdDoc());
                            double docLength = doc.second.getNumPalSinParada();

                            // Formula DFR
                            if (formSimilitud == 0)
                            {
                                // peso en la query del término palabra de la query (frecuencia del término en la query entre el número de términos de la query)
                                double w_tq = (double)palabra.second.getFt() / (double)preguntaInf.getNumTotalPalSinParada();
                                // peso en el documento del término palabra
                                double lambda = (double)terminoInf->getFtc() / (double)numDocs;
                                double f_td = infTermDoc->getFt() * log2(1 + (c * avgDocLength) / docLength);
                                double w_td = (log2(1 + lambda) + f_td * log2((1 + lambda) / lambda)) * ((terminoInf->getFtc() + 1) / (terminoInf->getNumDocs() * (f_td + 1)));
                                valor += w_tq * w_td;
                            }
                            else
                            {
                                // Formula BM25
                                double idf = log2((numDocs - terminoInf->getNumDocs() + 0.5) / (terminoInf->getNumDocs() + 0.5));
                                double f_qd = infTermDoc->getFt();
                                double aux2 = (double)(f_qd * (k1 + 1)) / (double)(f_qd + k1 * (1 - b + b * ((double)docLength / (double)avgDocLength)));
                                valor += idf * aux2;
                            }
                        }
                    }
                }
                // Almacenar el valor de similitud calculado para el documento actual
                // docsOrdenados.push(ResultadoRI(valor, doc.second.getIdDoc(), 0, doc.first));
                // docsOrdenadosVectorAux.push_back(ResultadoRI(valor, doc.second.getIdDoc(), i, doc.first));
                docsOrdenadosVectorAux.emplace_back(valor, doc.second.getIdDoc(), i, doc.first);
            }
            // Ordenar los documentos del vector docsOrdenadosVector de menor a mayor
            sort(docsOrdenadosVectorAux.begin(), docsOrdenadosVectorAux.end(), greater<ResultadoRI>());
            // Truncar el vector docsOrdenadosVector para que tenga el tamaño numDocumentos
            if (numDocumentos < docsOrdenadosVectorAux.size())
                docsOrdenadosVectorAux.erase(docsOrdenadosVectorAux.begin() + numDocumentos, docsOrdenadosVectorAux.end());
            // Almacenar los resultados de la búsqueda en el vector docsOrdenadosVector
            /*
            for (auto &resultado : docsOrdenadosVectorAux)
            {
                docsOrdenadosVector.push_back(resultado);
            }*/
            // docsOrdenadosVector.insert(docsOrdenadosVector.end(), docsOrdenadosVectorAux.begin(), docsOrdenadosVectorAux.end());
            std::move(docsOrdenadosVectorAux.begin(), docsOrdenadosVectorAux.end(), std::back_inserter(docsOrdenadosVector));
        }
        return true;
    }
    catch (...)
    {
        cerr << "ERROR: Hubo un error al realizar la búsqueda" << endl;
        return false;
    }
}

bool Buscador::Buscar_vector(const string &dirPreguntas, const int &numDocumentos, const int &numPregInicio, const int &numPregFin)
{

    try
    {
        // Limpiar el vector docsOrdenados antes de empezar una nueva búsqueda
        // docsOrdenados = priority_queue<ResultadoRI>();
        docsOrdenadosVector.clear();
        // Reservar memoria para el vector de resultados
        docsOrdenadosVector.reserve(numDocumentos * (numPregFin - numPregInicio + 1));

        // Obtener la lista de documentos indexados
        // vector<string> listaDocs = ListarDocs();
        vector<ResultadoRI> docsOrdenadosVectorAux;
        docsOrdenadosVectorAux.reserve(numDocumentos);
        vector<double> question_embedding;
        // Calcular el valor de los documentos de acuerdo con la fórmula de similitud elegida
        for (int i = numPregInicio; i <= numPregFin; i++)
        {
            docsOrdenadosVectorAux.clear();
            string pregunta;
            string nombrePregunta = dirPreguntas + to_string(i) + ".vec";
            ifstream f(nombrePregunta);
            if (!f.is_open())
            {
                cerr << "ERROR: No se pudo abrir el fichero " << nombrePregunta << endl;
                return false;
            }
            std::string linea;
            question_embedding.clear();
            while (std::getline(f, linea))
            {
                if (linea[0] == '[')
                {
                    linea.erase(0, 1);
                }
                if (linea[linea.size() - 1] == ']')
                {
                    linea.erase(linea.size() - 1, 1);
                }
                std::stringstream ss(linea);
                double valor;
                while (ss >> valor)
                {
                    question_embedding.push_back(valor);
                }
            }
            f.close();

            // Obtener la lista de documentos indexados
            // vector<string> listaDocs = ListarDocs();

            // Calcular el valor de los documentos de acuerdo con la fórmula de similitud elegida
            for (const auto &doc : embeddings)
            {
                double valor = cosine_similarity(question_embedding, doc.second);

                // Almacenar el valor de similitud calculado para el documento actual
                // Eliminar .vec del nombre del documento
                string nombreDoc = doc.first.substr(0, doc.first.size() - 4);
                int idDoc = stoi(nombreDoc);
                docsOrdenadosVectorAux.emplace_back(valor, idDoc, i, nombreDoc);
            }
            // Ordenar los documentos del vector docsOrdenadosVector de menor a mayor
            sort(docsOrdenadosVectorAux.begin(), docsOrdenadosVectorAux.end(), greater<ResultadoRI>());
            // Truncar el vector docsOrdenadosVector para que tenga el tamaño numDocumentos
            if (numDocumentos < docsOrdenadosVectorAux.size())
                docsOrdenadosVectorAux.erase(docsOrdenadosVectorAux.begin() + numDocumentos, docsOrdenadosVectorAux.end());
            // Almacenar los resultados de la búsqueda en el vector docsOrdenadosVector
            /*
            for (auto &resultado : docsOrdenadosVectorAux)
            {
                docsOrdenadosVector.push_back(resultado);
            }*/
            // docsOrdenadosVector.insert(docsOrdenadosVector.end(), docsOrdenadosVectorAux.begin(), docsOrdenadosVectorAux.end());
            std::move(docsOrdenadosVectorAux.begin(), docsOrdenadosVectorAux.end(), std::back_inserter(docsOrdenadosVector));
        }
        return true;
    }
    catch (...)
    {
        cerr << "ERROR: Hubo un error al realizar la búsqueda" << endl;
        return false;
    }
}

// Función para calcular la similitud de coseno entre dos vectores
double Buscador::cosine_similarity(const vector<double>& vectorA, const vector<double>& vectorB) {
    // Verificar que los vectores tengan la misma dimensión
    if (vectorA.size() != vectorB.size()) {
        std::cerr << "Los vectores tienen dimensiones diferentes." << std::endl;
        return 0.0;
    }

    // Calcular el producto punto de los vectores
    double dotProduct = 0.0;
    for (std::size_t i = 0; i < vectorA.size(); ++i) {
        dotProduct += vectorA[i] * vectorB[i];
    }

    // Calcular las magnitudes de los vectores
    double magnitudeA = 0.0;
    double magnitudeB = 0.0;
    for (std::size_t i = 0; i < vectorA.size(); ++i) {
        magnitudeA += vectorA[i] * vectorA[i];
        magnitudeB += vectorB[i] * vectorB[i];
    }
    magnitudeA = std::sqrt(magnitudeA);
    magnitudeB = std::sqrt(magnitudeB);

    // Calcular la similitud de coseno
    if (magnitudeA == 0.0 || magnitudeB == 0.0) {
        std::cerr << "Uno de los vectores tiene magnitud cero." << std::endl;
        return 0.0;
    }
    return dotProduct / (magnitudeA * magnitudeB);
}

void Buscador::ImprimirResultadoBusqueda(const int &numDocumentos)
{
    try
    {
        // Imprimir los resultados de la búsqueda
        int numPreguntaAnterior = -1;
        int numDocs = 0;
        int posicion = 0;
        string nombrePregunta = "";
        string nombreDoc = "";
        unordered_map<string, InfDoc> indiceDocs;
        DevolverIndiceDocs(indiceDocs);
        // while (!docsOrdenados.empty())
        for (auto &resultado : docsOrdenadosVector)
        {
            // Una linea por cada documento con el siguiente formato:
            // NumPregunta FormulaSimilitud NomDocumento Posicion PuntuacionDoc PreguntaIndexada

            // ResultadoRI resultado = docsOrdenados.top();
            if (numPreguntaAnterior != resultado.NumPregunta())
            {
                numDocs = 0;
                posicion = 0;
            }
            else
            {
                numDocs++;
                posicion++;
            }
            if (numDocs < numDocumentos)
            {
                if (resultado.NumPregunta() == 0)
                    DevuelvePregunta(nombrePregunta);
                else
                    nombrePregunta = "ConjuntoDePreguntas";
                // Nombre del documento sin el directorio y sin la extension
                nombreDoc = resultado.NombreDoc().substr(resultado.NombreDoc().find_last_of("/\\") + 1);
                nombreDoc = nombreDoc.substr(0, nombreDoc.find_last_of("."));
                std::cout.setf(std::ios::fixed);
                std::cout.precision(6);
                std::cout << resultado.NumPregunta() << " " << (formSimilitud == 0 ? "DFR" : "BM25") << " " << nombreDoc << " " << posicion << " " << resultado.VSimilitud() << " " << nombrePregunta << "\n";
            }
            numPreguntaAnterior = resultado.NumPregunta();
            // docsOrdenados.pop();
        }
    }
    catch (...)
    {
        cerr << "ERROR: Hubo un error al imprimir el resultado de la búsqueda" << endl;
    }
}

bool Buscador::ImprimirResultadoBusqueda(const int &numDocumentos, const string &nombreFichero)
{
    try
    {
        // Imprimir los resultados de la búsqueda
        int numPreguntaAnterior = -1;
        int numDocs = 0;
        int posicion = 0;
        string nombrePregunta = "";
        string nombreDoc = "";
        ofstream f(nombreFichero);
        if (!f.is_open())
        {
            cerr << "ERROR: No se pudo abrir el fichero " << nombreFichero << endl;
            return false;
        }
        unordered_map<string, InfDoc> indiceDocs;
        DevolverIndiceDocs(indiceDocs);
        // while (!docsOrdenados.empty())
        for (auto &resultado : docsOrdenadosVector)
        {
            // Una linea por cada documento con el siguiente formato:
            // NumPregunta FormulaSimilitud NomDocumento Posicion PuntuacionDoc PreguntaIndexada

            // ResultadoRI resultado = docsOrdenados.top();
            if (numPreguntaAnterior != resultado.NumPregunta())
            {
                numDocs = 0;
                posicion = 0;
            }
            else
            {
                numDocs++;
                posicion++;
            }
            if (numDocs < numDocumentos)
            {
                if (resultado.NumPregunta() == 0)
                    DevuelvePregunta(nombrePregunta);
                else
                    nombrePregunta = "ConjuntoDePreguntas";
                // Nombre del documento sin el directorio y sin la extension
                nombreDoc = resultado.NombreDoc().substr(resultado.NombreDoc().find_last_of("/\\") + 1);
                nombreDoc = nombreDoc.substr(0, nombreDoc.find_last_of("."));
                std::cout.setf(std::ios::fixed);
                std::cout.precision(6);
                f << resultado.NumPregunta() << " " << (formSimilitud == 0 ? "DFR" : "BM25") << " " << nombreDoc << " " << posicion << " " << resultado.VSimilitud() << " " << nombrePregunta << "\n";
            }
            numPreguntaAnterior = resultado.NumPregunta();
            // docsOrdenados.pop();
        }
        f.close();
        return true;
    }
    catch (...)
    {
        cerr << "ERROR: Hubo un error al imprimir el resultado de la búsqueda" << endl;
        return false;
    }
}