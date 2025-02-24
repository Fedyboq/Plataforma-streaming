#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <mutex>
#include <thread>
#include <future>
#include <functional>
#include <set>
#include <memory>
#include <regex>
#include <chrono>

using namespace std;
using namespace chrono;

namespace Utilidades {
    string normalizar_texto(const string &texto) {
        string resultado;
        for (char c : texto) {
            if (isalnum(c) || isspace(c)) {
                resultado += tolower(c);
            }
        }
        return resultado;
    }

    vector<string> dividir(const string &cadena, char delimitador) {
        vector<string> tokens;
        stringstream ss(cadena);
        string token;
        while (getline(ss, token, delimitador)) {
            token = normalizar_texto(token);
            if (!token.empty()) {  // Evitar insertar tokens vacíos
                tokens.push_back(token);
            }
        }
        return tokens;
    }
}

class Pelicula {
public:
    string imdb_id;
    string titulo;
    string sinopsis;
    vector<string> etiquetas;
    string split;
    string fuente_sinopsis;
    int relevancia;

    Pelicula(string id, string t, string s, vector<string> e, string sp, string fs)
            : imdb_id(std::move(id)),
              titulo(Utilidades::normalizar_texto(std::move(t))),
              sinopsis(Utilidades::normalizar_texto(std::move(s))),
              etiquetas(std::move(e)),
              split(Utilidades::normalizar_texto(std::move(sp))),
              fuente_sinopsis(Utilidades::normalizar_texto(std::move(fs))),
              relevancia(0) {}

    void mostrar_detalles() const {
        cout << "\n========== Detalles de la Pelicula ==========\n";
        cout << endl;
        cout << "Titulo: " << titulo << endl;
        cout << "IMDB ID: " << imdb_id << endl;
        cout << "Sinopsis: " << sinopsis << endl;
        cout << "Genero: ";
        for (const auto& tag : etiquetas) {
            cout << tag << " ";
        }
        cout << "\nSplit: " << split << endl;
        cout << "Fuente de Sinopsis: " << fuente_sinopsis << endl;
        cout << "=============================================\n";
    }
};

unordered_map<string, vector<Pelicula> > indice_tags;

class ComponenteArbol {
public:
    virtual void insertar(const string &palabra, const string &id_pelicula, const string &titulo) = 0;
    virtual unordered_map<string, string> buscar(const string &termino) = 0;
    virtual ~ComponenteArbol() {}
};

class NodoHoja : public ComponenteArbol {
private:
    unordered_map<string, string> peliculas;

public:
    void insertar(const string &palabra, const string &id_pelicula, const string &titulo) override {
        peliculas[id_pelicula] = titulo;
    }

    unordered_map<string, string> buscar(const string &termino) override {
        return peliculas;
    }
};

class NodoCompuesto : public ComponenteArbol {
private:
    unordered_map<char, shared_ptr<ComponenteArbol>> hijos;

public:
    void insertar(const string &palabra, const string &id_pelicula, const string &titulo) override {
        if (palabra.empty()) return;
        char c = palabra[0];
        if (hijos.find(c) == hijos.end()) {
            // Aquí ahora reconoce NodoHoja porque ya fue declarado antes
            hijos[c] = make_shared<NodoHoja>();
        }
        hijos[c]->insertar(palabra.substr(1), id_pelicula, titulo);
    }

    unordered_map<string, string> buscar(const string &termino) override {
        unordered_map<string, string> resultados;
        if (termino.empty()) {
            for (auto &hijo : hijos) {
                auto subresultados = hijo.second->buscar(termino);
                resultados.insert(subresultados.begin(), subresultados.end());
            }
        } else {
            char c = termino[0];
            if (hijos.find(c) != hijos.end()) {
                resultados = hijos[c]->buscar(termino.substr(1));
            }
        }
        return resultados;
    }
};

class ArbolBusqueda {
private:
    unordered_map<string, vector<pair<string, string>>> indice;

public:
    void insertar(const string &palabra, const string &id_pelicula, const string &titulo) {
        if (!palabra.empty()) {
            indice[palabra].push_back({id_pelicula, titulo});
        }
    }

    unordered_map<string, string> buscar(const string &termino) {
        unordered_map<string, string> resultados;
        if (termino.empty()) {
            return resultados;
        }

        // Buscar coincidencias parciales
        for (const auto &entrada : indice) {
            if (entrada.first.find(termino) != string::npos) {  // Si el término está contenido en la palabra
                for (auto &pair : entrada.second) {
                    resultados[pair.first] = pair.second;
                }
            }
        }

        // Mensaje si no encuentra resultados
        if (resultados.empty()) {
            cout << "\nNo se encontraron resultados para el termino: '" << termino << "'." << endl;
        }

        return resultados;
    }
};


class Observador {
public:
    virtual void actualizar(const string& mensaje) = 0;
};

class PlataformaPeliculas : public Observador {
private:
    vector<string> ver_mas_tarde;
    unordered_set<string> likes;

public:
    virtual void agregar_ver_mas_tarde(const string &titulo) {
        ver_mas_tarde.push_back(titulo);
        actualizar("Pelicula agregada a Ver Mas Tarde");
    }

    virtual void agregar_like(const string &titulo) {
        likes.insert(titulo);
        actualizar("Pelicula agregada a Likes");
    }

    virtual void mostrar_ver_mas_tarde() {
        cout << "\nPeliculas en Ver Mas Tarde:" << endl;
        if (ver_mas_tarde.empty()) {
            cout << "No tienes peliculas guardadas en Ver Mas Tarde." << endl;
        } else {
            for (const string &titulo : ver_mas_tarde) {
                cout << "- " << titulo << endl;
            }
        }
        cout << endl;
    }

    virtual void mostrar_likes() {
        cout << "\nPeliculas con Like: " << endl;
        if (likes.empty()) {
            cout << "No has dado Like a ninguna pelicula." << endl;
        } else {
            for (const string &titulo : likes) {
                cout << "- " << titulo << endl;
            }
        }
        cout << endl;
    }

    void actualizar(const string& mensaje) override {
        cout << "\n[Notificacion]: " << mensaje << endl << endl;
    }
};

// Decorador base
class PlataformaPeliculasDecorator : public PlataformaPeliculas {
protected:
    shared_ptr<PlataformaPeliculas> plataforma;
public:
    PlataformaPeliculasDecorator(shared_ptr<PlataformaPeliculas> plataforma) : plataforma(std::move(plataforma)) {}

    void agregar_ver_mas_tarde(const string &titulo) override {
        plataforma->agregar_ver_mas_tarde(titulo);
    }

    void agregar_like(const string &titulo) override {
        plataforma->agregar_like(titulo);
    }

    void mostrar_ver_mas_tarde() override {
        plataforma->mostrar_ver_mas_tarde();
    }

    void mostrar_likes() override {
        plataforma->mostrar_likes();
    }

    void actualizar(const string& mensaje) override {
        plataforma->actualizar(mensaje);
    }
};

// Decorador concreto: agrega logs
class PlataformaPeliculasLoggerDecorator : public PlataformaPeliculasDecorator {
public:
    PlataformaPeliculasLoggerDecorator(shared_ptr<PlataformaPeliculas> plataforma)
            : PlataformaPeliculasDecorator(std::move(plataforma)) {}

    void agregar_ver_mas_tarde(const string &titulo) override {
        cout << "[LOG] Agregando pelicula a Ver Mas Tarde: " << titulo << endl;
        PlataformaPeliculasDecorator::agregar_ver_mas_tarde(titulo);
    }

    void agregar_like(const string &titulo) override {
        cout << "[LOG] Agregando Like a pelicula: " << titulo << endl;
        PlataformaPeliculasDecorator::agregar_like(titulo);
    }
};

class CargadorCSV {
public:
    static vector<Pelicula> cargar_csv(const string &nombre_archivo, ArbolBusqueda &arbol) {
        ifstream archivo(nombre_archivo);
        vector<Pelicula> peliculas;
        string linea;

        if (!archivo.is_open()) {
            cerr << "Error: No se pudo abrir el archivo." << endl;
            return peliculas;
        }

        getline(archivo, linea); // Leer encabezado y descartarlo
        while (getline(archivo, linea)) {
            stringstream ss(linea);
            string imdb_id, titulo, plot_synopsis, tags, split, fuente_sinopsis;

            getline(ss >> ws, imdb_id, '\t');
            getline(ss >> ws, titulo, '\t');
            getline(ss >> ws, plot_synopsis, '\t');
            getline(ss >> ws, tags, '\t');
            getline(ss >> ws, split, '\t');
            getline(ss >> ws, fuente_sinopsis, '\t');

            imdb_id = Utilidades::normalizar_texto(imdb_id);
            titulo = Utilidades::normalizar_texto(titulo);
            plot_synopsis = Utilidades::normalizar_texto(plot_synopsis);

            vector<string> etiquetas_procesadas = Utilidades::dividir(tags, ',');

            split = Utilidades::normalizar_texto(split);
            fuente_sinopsis = Utilidades::normalizar_texto(fuente_sinopsis);

            Pelicula pelicula(imdb_id, titulo, plot_synopsis, etiquetas_procesadas, split, fuente_sinopsis);
            peliculas.push_back(pelicula);

            for (const string &palabra : Utilidades::dividir(pelicula.titulo + " " + pelicula.sinopsis, ' ')) {
                arbol.insertar(palabra, pelicula.imdb_id, pelicula.titulo);
            }
            for (const auto &tag : etiquetas_procesadas) {
                string tag_normalizado = Utilidades::normalizar_texto(tag);
                tag_normalizado.erase(remove_if(tag_normalizado.begin(), tag_normalizado.end(), ::isspace), tag_normalizado.end()); // Eliminar espacios extra
                indice_tags[tag_normalizado].push_back(pelicula);
            }

        }

        archivo.close();
        return peliculas;
    }
};

class Busqueda {
public:
    virtual void realizar_busqueda() = 0; // Método abstracto
    virtual ~Busqueda() = default;
};

class BusquedaPorPalabra : public Busqueda {
private:
    ArbolBusqueda& arbol;
    vector<Pelicula>& peliculas;  // Referencia a la lista completa de películas
    shared_ptr<PlataformaPeliculas> plataforma; // Referencia a la plataforma

public:
    // Constructor actualizado para recibir la lista de películas y la plataforma
    BusquedaPorPalabra(ArbolBusqueda& arbol, vector<Pelicula>& peliculas, shared_ptr<PlataformaPeliculas> plataforma)
            : arbol(arbol), peliculas(peliculas), plataforma(std::move(plataforma)) {}

    void realizar_busqueda() override {
        cout << "\nIngrese una palabra para buscar peliculas: ";
        string busqueda;
        cin >> busqueda;

        if (busqueda.empty()) {
            cout << "\nNo ingreso ninguna palabra valida." << endl;
            return;
        }

        auto inicio_busqueda = high_resolution_clock::now(); // INICIO TIEMPO DE BUSQUEDA

        unordered_map<string, string> resultados = arbol.buscar(Utilidades::normalizar_texto(busqueda));

        auto fin_busqueda = high_resolution_clock::now(); // FIN TIEMPO DE BUSQUEDA
        auto duracion_busqueda = duration_cast<milliseconds>(fin_busqueda - inicio_busqueda);
        cout << "\nTiempo de ejecución de la busqueda: " << duracion_busqueda.count() << " ms" << endl;

        if (resultados.empty()) {
            cout << "\nNo se encontraron peliculas." << endl;
            return;
        }

        vector<pair<string, string>> lista_resultados(resultados.begin(), resultados.end());
        int pagina = 0;
        int total_paginas = (lista_resultados.size() + 4) / 5;

        while (true) {
            cout << "\n========== Pagina " << pagina + 1 << " de " << total_paginas << " ==========\n";
            for (int i = pagina * 5; i < min((pagina + 1) * 5, (int)lista_resultados.size()); ++i) {
                cout << (i % 5) + 1 << ". " << lista_resultados[i].second << " (IMDB ID: " << lista_resultados[i].first << ")\n";
            }

            if (total_paginas > 1) {
                cout << "6. Pagina siguiente\n7. Pagina anterior\n";
            }
            cout << "0. Volver al menu principal\nSeleccione una pelicula o una opcion: ";
            int seleccion;
            cin >> seleccion;

            if (seleccion == 0) break;
            if (seleccion == 6 && pagina + 1 < total_paginas) { pagina++; continue; }
            if (seleccion == 7 && pagina > 0) { pagina--; continue; }
            if (seleccion >= 1 && seleccion <= 5) {
                int index = pagina * 5 + (seleccion - 1);
                if (index >= lista_resultados.size()) {
                    cout << "\nOpcion invalida." << endl;
                    continue;
                }

                string imdb_id = lista_resultados[index].first;

                // Buscar la película por ID en la lista de películas
                auto it = find_if(peliculas.begin(), peliculas.end(), [&](const Pelicula& p) {
                    return p.imdb_id == imdb_id;
                });

                // Mostrar detalles si se encuentra la película
                if (it != peliculas.end()) {
                    it->mostrar_detalles();

                    // Submenú para dar Like o Agregar a Ver Más Tarde
                    while (true) {
                        cout << "\n1. Dar Like\n2. Agregar a Ver Mas Tarde\n3. Volver al menu\nSeleccione una opcion: ";
                        int subopcion;
                        cin >> subopcion;

                        if (subopcion == 1) {
                            plataforma->agregar_like(it->titulo);
                        } else if (subopcion == 2) {
                            plataforma->agregar_ver_mas_tarde(it->titulo);
                        } else if (subopcion == 3) {
                            break;
                        } else {
                            cout << "\nOpcion invalida." << endl;
                        }
                    }
                } else {
                    cout << "\nNo se encontraron detalles de la pelicula seleccionada.\n";
                }
            } else {
                cout << "\nOpcion invalida." << endl;
            }
        }
    }
};

class BusquedaPorTag : public Busqueda {
private:
    vector<Pelicula>& peliculas; // Referencia a la lista completa de películas
    shared_ptr<PlataformaPeliculas> plataforma; // Referencia a la plataforma

public:
    // Constructor actualizado para recibir la lista de películas y la plataforma
    BusquedaPorTag(vector<Pelicula>& peliculas, shared_ptr<PlataformaPeliculas> plataforma)
            : peliculas(peliculas), plataforma(plataforma) {}

    void realizar_busqueda() override {
        cout << "\nIngrese el tag por el que desea buscar: ";
        string tag;
        cin >> ws;
        getline(cin, tag);
        tag = Utilidades::normalizar_texto(tag);
        tag.erase(remove_if(tag.begin(), tag.end(), ::isspace), tag.end());

        auto inicio_busqueda = high_resolution_clock::now(); // INICIO TIEMPO BUSQUEDA POR TAG

        if (indice_tags.find(tag) == indice_tags.end()) {
            cout << "No se encontraron peliculas con el tag ingresado." << endl;
            return;
        }

        vector<Pelicula> peliculas_tag = indice_tags[tag];

        auto fin_busqueda = high_resolution_clock::now(); // FIN TIEMPO BUSQUEDA POR TAG
        auto duracion_busqueda = duration_cast<milliseconds>(fin_busqueda - inicio_busqueda);
        cout << "\nTiempo de ejecucion de la busqueda: " << duracion_busqueda.count() << " ms" << endl;

        int pagina = 0;
        int total_paginas = (peliculas_tag.size() + 4) / 5;

        while (true) {
            cout << "\n========== Pagina " << pagina + 1 << " de " << total_paginas << " ==========\n";
            for (int i = pagina * 5; i < min((pagina + 1) * 5, (int)peliculas_tag.size()); ++i) {
                cout << (i % 5) + 1 << ". " << peliculas_tag[i].titulo << " (ID: " << peliculas_tag[i].imdb_id << ")\n";
            }

            if (total_paginas > 1) {
                cout << "6. Página siguiente\n7. Página anterior\n";
            }
            cout << "0. Volver al menu principal\n";
            cout << "\nSeleccione una pelicula o una opcion: ";
            int seleccion;
            cin >> seleccion;

            if (seleccion == 0) break;
            if (seleccion == 6 && pagina + 1 < total_paginas) { pagina++; continue; }
            if (seleccion == 7 && pagina > 0) { pagina--; continue; }
            if (seleccion >= 1 && seleccion <= 5) {
                int index = pagina * 5 + (seleccion - 1);
                if (index >= peliculas_tag.size()) {
                    cout << "\nOpción invalida." << endl;
                    continue;
                }

                string imdb_id = peliculas_tag[index].imdb_id;
                string titulo = peliculas_tag[index].titulo;

                // Mostrar opciones adicionales para la película seleccionada
                cout << "\n1. Dar Like\n2. Agregar a Ver Mas Tarde\n3. Volver al menu\nSeleccione una opcion: ";
                int opcion_extra;
                cin >> opcion_extra;

                if (opcion_extra == 1) {
                    plataforma->agregar_like(titulo);
                } else if (opcion_extra == 2) {
                    plataforma->agregar_ver_mas_tarde(titulo);
                } else if (opcion_extra == 3) {
                    continue;
                } else {
                    cout << "\nOpcion invalida." << endl;
                }
            } else {
                cout << "\nOpcion invalida." << endl;
            }
        }
    }
};

class BusquedaFactory {
public:
    static unique_ptr<Busqueda> crear_busqueda(int tipo, ArbolBusqueda& arbol, vector<Pelicula>& peliculas, shared_ptr<PlataformaPeliculas> plataforma) {
        if (tipo == 3) {
            return make_unique<BusquedaPorPalabra>(arbol, peliculas, plataforma);
        } else if (tipo == 4) {
            return make_unique<BusquedaPorTag>(peliculas, plataforma);
        }
        return nullptr;
    }
};

int main() {
    string nombre_archivo = "output_ultimo.tsv";
    ArbolBusqueda arbol;
    shared_ptr<PlataformaPeliculas> plataforma = make_shared<PlataformaPeliculas>();
    shared_ptr<PlataformaPeliculas> plataformaDecorada = make_shared<PlataformaPeliculasLoggerDecorator>(plataforma);

    auto inicio_carga = high_resolution_clock::now(); //INICIO TIEMPO DE CARGA DEL ARCHIVO

    auto futuro_peliculas = async(launch::async, CargadorCSV::cargar_csv, nombre_archivo, ref(arbol));
    vector<Pelicula> peliculas = futuro_peliculas.get();

    auto fin_carga = high_resolution_clock::now(); //FIN TIEMPO DE CARGA DEL ARCHIVO
    auto duracion_carga = duration_cast<milliseconds>(fin_carga - inicio_carga);
    cout << "\nTiempo de carga del archivo CSV: " << duracion_carga.count() << " ms" << endl;

    // Verificación después de la carga del archivo
    if (peliculas.empty()) {
        cerr << "No se cargaron películas. Verifica el archivo de entrada." << endl;
        return 1;
    }

    cout << "\n-----------------------------------------" << endl;
    cout << "|       Plataforma de Streaming         |" << endl;
    cout << "-----------------------------------------" << endl;

    while (true) {
        cout << "\n1. Ver peliculas en Ver Mas Tarde" << endl;
        cout << "2. Ver peliculas Likeadas" << endl;
        cout << "3. Buscar peliculas por palabra" << endl;
        cout << "4. Buscar peliculas por tag" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        int opcion;
        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrada invalida. Intente de nuevo.\n";
            continue;
        }

        if (opcion == 0) break;
        if (opcion == 1) {
            plataformaDecorada->mostrar_ver_mas_tarde();
            continue;
        }
        if (opcion == 2) {
            plataformaDecorada->mostrar_likes();
            continue;
        }

        // Usar la fábrica para generar el tipo de búsqueda
        if (opcion == 3 || opcion == 4) {
            auto busqueda = BusquedaFactory::crear_busqueda(opcion, arbol, peliculas, plataformaDecorada);
            if (busqueda) {
                busqueda->realizar_busqueda();
            } else {
                cout << "Opcion de busqueda invalida." << endl;
            }
        }
    }

    return 0;
}
