## Plataforma-streaming
proyecto sobre la plataforma de streaming
# Descripción
Este es un proyecto que adapta una pequeña interfaz para poder buscar peliculas, en la cual usa un algoritmo de busqueda de trie para hacerlo, en base a si contienen el patron insertado tanto en su titulo como en su synopsis y te entrega el titulo con su id correspondiente. Ademas de esta funciona cuenta con una funcion para poder visualizar que peliculas fueron likeadas y una funcion de ver mas tarde.
# Caracteristicas 
Se utilizan distintos patrones de diseño:
- el observer para el mecanismo de likes y ver mas tarde
- el composatie o trie para hacer el algoritmo de busqueda de las peliculas
- el decorator para hacer una interfaz mas vistoza
- ...
# Manejo del CSV
para manejar correctamente los datos creamos un un cpp adicional llamado preprocesamiento de datos en el cual limpiamos todo el csv de caracteres especiales, pasamos a minusculas e incluso limpiamos la columna de los id. A pesar de todas estas incorporaciones seguia presentando errores a la hora de usar el codigo debido a problemas con las comas que se encontraban dentro de las columnas de sinopsis y en los tags a pesar de que esten dentro de las comillas. Debido a esto nos parecio mas sencillo pasar el archivo a un tsv delimitado por saltos de barra en vez de comas para poder trabajar mas facilmente y tuvimos exito con este metodo.
# Uso 
para usar el codigo solo se debe colocar una tabla en formato tsv en la variable nombre_archivo para que el codigo haga el arbol y corra con la interfaz.
la interfaz de el codigo hecha en la terminal se basa en que el usuario coloque numeros para moverse por la misma y cuenta con paginacion para no abrumar al usuario.
![Interfaz de texto](Imagenes/foto_interfaz.png)
![Interfaz de texto](Imagenes/foto_interfaz2.png)
