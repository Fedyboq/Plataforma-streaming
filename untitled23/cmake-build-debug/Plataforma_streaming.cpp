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

using namespace std;

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
            tokens.push_back(normalizar_texto(token));
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
        : imdb_id(move(id)), titulo(move(t)), sinopsis(move(s)), etiquetas(move(e)), split(move(sp)), fuente_sinopsis(move(fs)), relevancia(0) {}
};

class NodoArbol {
public:
    unordered_map<char, NodoArbol*> hijos;
    unordered_map<string, string> peliculas;

    ~NodoArbol() {
        for (auto &par : hijos) {
            delete par.second;
        }
    }
};

class ArbolBusqueda {
private:
    NodoArbol* raiz;
public:
    ArbolBusqueda() { raiz = new NodoArbol(); }
    ~ArbolBusqueda() { delete raiz; }

    void insertar(const string &palabra, const string &id_pelicula, const string &titulo) {
        NodoArbol* nodo = raiz;
        for (char c : palabra) {
            if (!nodo->hijos[c]) {
                nodo->hijos[c] = new NodoArbol();
            }
            nodo = nodo->hijos[c];
        }
        nodo->peliculas[id_pelicula] = titulo;
    }

    unordered_map<string, string> buscar(const string &termino) {
        NodoArbol* nodo = raiz;
        for (char c : termino) {
            if (!nodo->hijos[c]) return {};
            nodo = nodo->hijos[c];
        }
        return nodo->peliculas;
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
    PlataformaPeliculasDecorator(shared_ptr<PlataformaPeliculas> plataforma) : plataforma(move(plataforma)) {}

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
        : PlataformaPeliculasDecorator(move(plataforma)) {}

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
        }

        archivo.close();
        return peliculas;
    }
};


int main() {
    string nombre_archivo = "output_limpio_minusculas.tsv";
    ArbolBusqueda arbol;
    shared_ptr<PlataformaPeliculas> plataforma = make_shared<PlataformaPeliculas>();
    shared_ptr<PlataformaPeliculas> plataformaDecorada = make_shared<PlataformaPeliculasLoggerDecorator>(plataforma);

    auto futuro_peliculas = async(launch::async, CargadorCSV::cargar_csv, nombre_archivo, ref(arbol));
    vector<Pelicula> peliculas = futuro_peliculas.get();
    cout << "\n-----------------------------------------" << endl;
    cout << "|       Plataforma de Streaming         |"<<endl;
    cout << "-----------------------------------------" << endl;

    while (true) {
        cout << "1. Ver peliculas en Ver Mas Tarde" << endl;
        cout << "2. Ver peliculas Likeadas" << endl;
        cout << "3. Buscar peliculas" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        int opcion;
        cin >> opcion;

        if (opcion == 0) break;
        if (opcion == 1) { plataformaDecorada->mostrar_ver_mas_tarde(); continue; }
        if (opcion == 2) { plataformaDecorada->mostrar_likes(); continue; }

        if (opcion == 3) {
            cout << "\nIngrese una palabra para buscar peliculas: ";
            string busqueda;
            cin >> busqueda;
            unordered_map<string, string> resultados = arbol.buscar(Utilidades::normalizar_texto(busqueda));
            if (resultados.empty()) {
                cout << "\nNo se encontraron peliculas."<<endl;
                continue;
            }
            vector<pair<string, string>> lista_resultados(resultados.begin(), resultados.end());
            int pagina = 0;
            int total_paginas = (lista_resultados.size() + 4) / 5;

            while (true) {
                cout << "\n========== Pagina " << pagina + 1 << " de " << total_paginas << " ==========" << endl;
                for (int i = pagina * 5; i < min((pagina + 1) * 5, (int)lista_resultados.size()); ++i) {
                    cout << (i % 5) + 1 << ". " << lista_resultados[i].second << " (IMDB ID: " << lista_resultados[i].first << ")\n";
                }

                if (total_paginas > 1) {
                    cout << "6. Pagina siguiente" << endl;
                    cout << "7. Pagina anterior" << endl;
                }
                cout << "0. Volver al menu principal" << endl;
                cout << "Seleccione una pelicula (1-5) o una opcion: ";
                int seleccion;
                cin >> seleccion;

                if (seleccion == 0) break;
                if (seleccion == 6 && pagina + 1 < total_paginas) { pagina++; continue; }
                if (seleccion == 7 && pagina > 0) { pagina--; continue; }
                if (seleccion < 1 || seleccion > 5) {
                    cout << "\nOpcion invalida." << endl;
                    continue;
                }

                int index = pagina * 5 + (seleccion - 1);
                if (index >= lista_resultados.size()) {
                    cout << "\nOpcion invalida." << endl;
                    continue;
                }

                string pelicula_titulo = lista_resultados[index].second;
                cout << "\n1. Dar Like\n2. Agregar a Ver Mas Tarde\n3. Volver al menu\nSeleccione una opcion: ";
                int accion;
                cin >> accion;
                if (accion == 1) plataformaDecorada->agregar_like(pelicula_titulo);
                else if (accion == 2) plataformaDecorada->agregar_ver_mas_tarde(pelicula_titulo);
            }
        }
    }
    return 0;
}
