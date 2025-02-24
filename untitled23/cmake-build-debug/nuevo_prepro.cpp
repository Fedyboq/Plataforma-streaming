#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cctype>
#include <algorithm>

using namespace std;

// Mapa de caracteres especiales a ASCII básico
unordered_map<string, string> caracteresEspeciales = {
        {"á", "a"}, {"é", "e"}, {"í", "i"}, {"ó", "o"}, {"ú", "u"},
        {"Á", "A"}, {"É", "E"}, {"Í", "I"}, {"Ó", "O"}, {"Ú", "U"},
        {"ñ", "n"}, {"Ñ", "N"}, {"å", "a"}, {"ý", "y"},
        {"ö", "o"}, {"Ö", "O"}, {"ü", "u"}, {"Ü", "U"},
        {"ß", "ss"},{"Å", "A"},
        {"â", "a"}, {"ê", "e"}, {"î", "i"}, {"ô", "o"}, {"û", "u"},
        {"Â", "A"}, {"Ê", "E"}, {"Î", "I"}, {"Ô", "O"}, {"Û", "U"},
        {"à", "a"}, {"è", "e"}, {"ù", "u"},
        {"À", "A"}, {"È", "E"}, {"Ù", "U"},
        {"ë", "e"}, {"Ë", "E"},
        {"ç", "c"}, {"Ç", "C"},
        {"ø", "o"}, {"Ø", "O"},
        {"œ", "oe"}, {"Œ", "OE"},
        {"š", "s"}, {"Š", "S"},
        {"ž", "z"}, {"Ž", "Z"}
};

// Eliminar caracteres especiales
string convertirASCII(const string &texto) {
    string resultado;
    for (char c : texto) {
        string cStr(1, c);
        if (caracteresEspeciales.count(cStr)) {
            resultado += caracteresEspeciales[cStr];
        } else {
            resultado += c;
        }
    }
    return resultado;
}

// Limpiar texto, quitar saltos de línea dentro de comillas
string limpiar_texto(const string &texto, bool quitar_comas = false) {
    string resultado;
    for (char c : texto) {
        string cStr(1, c);
        if (isalnum(c) || isspace(c) || (!quitar_comas && c == ',') || c == '"') {
            if (caracteresEspeciales.count(cStr)) {
                resultado += caracteresEspeciales[cStr];
            } else {
                resultado += c;
            }
        }
    }
    return resultado;
}

// Convertir texto a minúsculas
string texto_a_minuscula(const string &texto) {
    string resultado;
    for (char c : texto) {
        resultado += tolower(c);
    }
    return resultado;
}

// Eliminar "tt" del imdb_id
string remove_tt_imdb_id(const string &texto) {
    if (texto.rfind("tt", 0) == 0) {
        return texto.substr(2);
    }
    return texto;
}

// Leer el archivo con manejo de líneas interrumpidas por comillas
string leer_linea_completa(ifstream &archivo) {
    string linea, total_linea;
    bool comillas_abiertas = false;

    while (getline(archivo, linea)) {
        total_linea += linea;

        // Contar comillas para verificar si están balanceadas
        int comillas = count(linea.begin(), linea.end(), '"');
        if (comillas % 2 != 0) {
            comillas_abiertas = !comillas_abiertas;
        }

        if (!comillas_abiertas) {
            break; // Termina cuando las comillas están balanceadas
        } else {
            total_linea += " "; // Agregar un espacio por cada salto de línea removido
        }
    }

    return total_linea;
}

int main() {
    ifstream infile("mpst_full_data.csv");
    ofstream outfile("output_limpio_final.csv");

    if (!infile.is_open()) {
        cerr << "No se pudo abrir el archivo de entrada." << endl;
        return 1;
    }
    if (!outfile.is_open()) {
        cerr << "No se pudo crear el archivo de salida." << endl;
        return 1;
    }

    string linea;
    getline(infile, linea);  // Leer la cabecera
    outfile << linea << "\n";

    // Leer y limpiar datos
    while (!infile.eof()) {
        string linea_completa = leer_linea_completa(infile);
        if (linea_completa.empty()) continue;

        // Procesar cada línea
        bool dentro_de_comillas = false;
        for (char &c : linea_completa) {
            if (c == '"') dentro_de_comillas = !dentro_de_comillas;
            else if (dentro_de_comillas && (c == '\n' || c == '\r')) c = ' '; // Reemplaza saltos de línea dentro de comillas por espacio
        }

        // Separar las columnas
        stringstream ss(linea_completa);
        string campo;
        vector<string> campos;
        int columna_index = 0;

        while (getline(ss, campo, ',')) {
            if (columna_index == 0) { // Eliminar "tt" del imdb_id
                campo = remove_tt_imdb_id(campo);
            }
            campos.push_back(campo);
            columna_index++;
        }

        // Escribir datos limpios al archivo
        for (size_t i = 0; i < campos.size(); ++i) {
            outfile << campos[i];
            if (i < campos.size() - 1) outfile << ",";
        }
        outfile << "\n";
    }

    infile.close();
    outfile.close();

    cout << "Preprocesamiento completado sin saltos de linea en sinopsis y sin 'tt' en imdb_id. Archivo guardado en output_limpio_final.csv." << endl;

    return 0;
}