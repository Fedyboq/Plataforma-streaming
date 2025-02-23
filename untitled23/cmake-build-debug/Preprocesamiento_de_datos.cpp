#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cctype>

using namespace std;

// Mapa de caracteres especiales a ASCII básico
unordered_map<char, string> caracteresEspeciales = {
        {'á', "a"}, {'é', "e"}, {'í', "i"}, {'ó', "o"}, {'ú', "u"},
        {'Á', "A"}, {'É', "E"}, {'Í', "I"}, {'Ó', "O"}, {'Ú', "U"},
        {'ñ', "n"}, {'Ñ', "N"}, {'å', "a"}, {'ý', "y"},
        {'ö', "o"}, {'Ö', "O"}, {'ü', "u"}, {'Ü', "U"},
        {'ß', "ss"},{'Å', "A"},
        {'â', "a"}, {'ê', "e"}, {'î', "i"}, {'ô', "o"}, {'û', "u"},
        {'Â', "A"}, {'Ê', "E"}, {'Î', "I"}, {'Ô', "O"}, {'Û', "U"},
        {'à', "a"}, {'è', "e"}, {'ù', "u"},
        {'À', "A"}, {'È', "E"}, {'Ù', "U"},
        {'ë', "e"}, {'Ë', "E"},
        {'ç', "c"}, {'Ç', "C"},
        {'ø', "o"}, {'Ø', "O"},
        {'œ', "oe"}, {'Œ', "OE"},
        {'š', "s"}, {'Š', "S"},
        {'ž', "z"}, {'Ž', "Z"}
};

// Función para convertir caracteres especiales a ASCII básico
string convertirASCII(const string &texto) {
    string resultado;
    for (char c : texto) {
        if (caracteresEspeciales.count(c)) {
            resultado += caracteresEspeciales[c];
        } else {
            resultado += c;
        }
    }
    return resultado;
}

// Función para limpiar texto y convertir caracteres especiales
string limpiar_texto(const string &texto) {
    string resultado;
    for (char c : texto) {
        if (isalnum(c) || isspace(c) || c == ',' || c == '"') {
            if (caracteresEspeciales.count(c)) {
                resultado += caracteresEspeciales[c];
            } else {
                resultado += c;
            }
        }
    }
    return resultado;
}

// Función para convertir un texto a minúsculas
string texto_a_minuscula(const string &texto) {
    string resultado;
    for (char c : texto) {
        resultado += tolower(c);
    }
    return resultado;
}

// Función para remover "tt" de la columna imdb_id
string remove_tt_imdb_id(const string &texto) {
    if (texto.rfind("tt", 0) == 0) {
        return texto.substr(2);
    }
    return texto;
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
    getline(infile, linea);
    outfile << linea << "\n";

    // Determinar índice de la columna imdb_id
    stringstream cabecera_ss(linea);
    vector<string> nombre_columna;
    string columna;
    int imdb_id_index = -1, col_index = 0;

    while (getline(cabecera_ss, columna, ',')) {
        nombre_columna.push_back(columna);
        if (columna == "imdb_id") {
            imdb_id_index = col_index;
        }
        col_index++;
    }

    while (getline(infile, linea)) {
        stringstream ss(linea);
        vector<string> campos;
        string campo;
        int current_index = 0;

        while (getline(ss, campo, ',')) {
            campo = limpiar_texto(convertirASCII(campo));

            if (current_index == imdb_id_index) {
                campo = remove_tt_imdb_id(campo);
            }

            campos.push_back(campo);
            current_index++;
        }

        for (size_t i = 0; i < campos.size(); ++i) {
            outfile << campos[i];
            if (i < campos.size() - 1) outfile << ",";
        }
        outfile << "\n";
    }

    infile.close();
    outfile.close();

    cout << "Preprocesamiento completado! Se guardo en output_limpio_final.csv." << endl;

    // Convertir el archivo limpio a minúsculas
    ifstream archivoLimpio("output_limpio_final.csv");
    ofstream archivoMinuscula("output_limpio_minusculas.csv");

    if (!archivoLimpio.is_open()) {
        cerr << "No se pudo abrir el archivo limpio para convertir a minúsculas." << endl;
        return 1;
    }
    if (!archivoMinuscula.is_open()) {
        cerr << "No se pudo crear el archivo de salida para minúsculas." << endl;
        return 1;
    }

    while (getline(archivoLimpio, linea)) {
        archivoMinuscula << texto_a_minuscula(linea) << "\n";
    }

    archivoLimpio.close();
    archivoMinuscula.close();

    cout << "Archivo final completado para trabajar! Se guardo en output_limpio_minusculas.csv." << endl;

    return 0;
}