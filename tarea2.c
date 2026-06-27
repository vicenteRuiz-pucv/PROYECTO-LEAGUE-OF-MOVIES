#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  char id[100];
  char title[300];
  List *genres;
  int year;
  char director[300];
  int similitud;
  float rating;
} pelicula;

// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     ¡Bienvenido a League Of Movies!");
  puts("========================================");

  puts("1) CINE MATCH");
  puts("2) CINE CLASH");
  puts("3) MARATHON MAKER");
  puts("4) CINE ORACLE");
  puts("5) Salir");
}

/**
 * Compara dos claves de tipo string para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo string.
 *
 * @param key1 Primer puntero a la clave string.
 * @param key2 Segundo puntero a la clave string.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

/**
 * Compara dos claves de tipo entero para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo entero.
 *
 * @param key1 Primer puntero a la clave entera.
 * @param key2 Segundo puntero a la clave entera.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_int(void *key1, void *key2) {
  return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}

/**
 * Carga películas desde un archivo CSV y las almacena en un mapa por ID.
 */
void cargar_peliculas(Map *pelis_byid, Map *pelis_bygenres, Map *pelis_decada,Map* pelis_titulo) {
  // Intenta abrir el archivo CSV que contiene datos de películas
  FILE *archivo = fopen("Top1500.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  char **campos;
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV

  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    // Crea una nueva estructura Film y almacena los datos de cada película
    pelicula *peli = (pelicula *)malloc(sizeof(pelicula));
    strcpy(peli->id, campos[1]);        // Asigna ID
    strcpy(peli->title, campos[5]);     // Asigna título
    peli->genres = split_string(campos[11], ",");       // Inicializa la lista de géneros
    peli->rating = atof(campos[8]);
    peli->year = atoi(campos[10]); // Asigna año, convirtiendo de cadena a entero
    strcpy(peli->director,campos[14]);
    
    // Inserta la película en el mapa usando el ID como clave
    map_insert(pelis_byid, peli->id, peli);

    
    //CREAMOS UN MAPA DE TITULOS!
    map_insert(pelis_titulo,peli->title,peli);
    
    //LLENAR MAPA DECADAS
    int *calculoDecada = (int*) malloc(sizeof(int));
    *calculoDecada = (peli->year / 10) * 10;
    MapPair *busquedaDecada= map_search(pelis_decada,calculoDecada);
    if(busquedaDecada == NULL)
    {
      List* lista_decada= list_create();
      list_pushBack(lista_decada,peli);
      map_insert(pelis_decada,calculoDecada,lista_decada);
    }
    else
    {
      list_pushBack((List*)busquedaDecada->value,peli);
      free(calculoDecada);
    }
    
    
    // Obtiene el primer género de la lista de géneros de la película
    char *genre = list_first(peli->genres);
    // Itera sobre cada género de la película
    while (genre != NULL) 
    {
        // Busca el género en el mapa pelis_bygenres
        MapPair *genre_pair = map_search(pelis_bygenres, genre);

        // Si el género no existe en el mapa, crea una nueva lista y agrégala al mapa
        if (genre_pair == NULL) {
            List *new_list = list_create();
            list_pushBack(new_list, peli);
            map_insert(pelis_bygenres, genre, new_list);
        } else {
            // Si el género ya existe en el mapa, obtén la lista y agrega la película
            List *genre_list = (List *)genre_pair->value;
            list_pushBack(genre_list, peli);
        }

        // Avanza al siguiente género en la lista
        genre = list_next(peli->genres);
    }
    
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas

  //printf("Peliculas cargadas Exitosamente!\n");
  // Itera sobre el mapa para mostrar las películas cargadas
  /*
  MapPair *pair = map_first(pelis_byid);
  while (pair != NULL) {
    Film *peli = pair->value;
    printf("ID: %s, Título: %s, Año: %d\n", peli->id, peli->title,
           peli->year);

    printf("Géneros: ");
    for(char *genre = list_first(peli->genres); genre != NULL; genre = list_next(peli->genres))
      printf("%s, ", genre);
    printf("\n");
    
    pair = map_next(pelis_byid); // Avanza al siguiente par en el mapa
  }
  */
}

/**
 * Busca y muestra la información de una película por su ID en un mapa.
 */
void buscar_por_id(Map *pelis_byid) {
  char id[10]; // Buffer para almacenar el ID de la película

  // Solicita al usuario el ID de la película
  printf("Ingrese el id de la película: ");
  scanf("%s", id); // Lee el ID del teclado

  // Busca el par clave-valor en el mapa usando el ID proporcionado
  MapPair *pair = map_search(pelis_byid, id);

  // Si se encontró el par clave-valor, se extrae y muestra la información de la
  // película
  if (pair != NULL) {
    pelicula *peli =
        pair->value; // Obtiene el puntero a la estructura de la película
    // Muestra el título y el año de la película
    printf("Título: %s, Año: %d\n", peli->title, peli->year);
  } else {
    // Si no se encuentra la película, informa al usuario
    printf("La película con id %s no existe\n", id);
  }
}
void mostrar_marathon(pelicula *p, int pos, int total){
  if (p == NULL)return;
  printf("\n");
  printf("  +--------------------------------------------------+\n");
  printf("  | Pelicula %d de %d\n", pos,total);
  printf("  | Titulo : %s\n", p->title);
  printf("  | Año    : %d\n", p->year);
  printf("  | Rating : %.1f\n", p->rating);
  printf("  | Generos: ");
  char *g = list_first(p->genres);
  while(g != NULL){
    printf("%s",g);
    g = list_next(p->genres);
    if (g != NULL)printf(", ");
  }
  printf("\n");
  printf("  +--------------------------------------------------+\n");
}

void marathon_maker(Map *pelis_bygenres, Map *pelis_bydecade){
  limpiarPantalla();
  printf("========================================\n");
  printf("         MARATHON MAKER\n");
  printf("========================================\n\n");
  
  // Paso 1: Guardar el genero ingresado 
  char genero[100];
  printf("Ingrese un genero : ");
  scanf("%99s",genero);
  
  // Paso 2: Se busca el genero ingreado en el mapa
  MapPair *par_genero = map_search(pelis_bygenres,genero);
  if (par_genero == NULL){
    printf("Genero no encontrado.\n");
    return;
  }
  // Paso 3 : Preguntamos por la decada
  int anio, decada_buscada;

  while(1){
    printf("Ingrese un Año : ");
    if (scanf("%d", &anio) != 1){
      printf("Intente nuevamente\n");
      while(getchar() != '\n');
      continue;
    }
    while (getchar() != '\n');

    decada_buscada = (anio / 10) * 10;

    if (decada_buscada >= 1880 && decada_buscada <= 2020)break;
    printf("Digite un año razonable.\n");
  }
  // Paso 4: Prengutamos por la cantidad de peliculas en la maraton 
  int cantidad;

  while (1){
    printf("Cuantas peliculas desea en su maraton?: ");
    if (scanf("%d", &cantidad) == 1 && cantidad > 0){
      while (getchar() != '\n');
      break;
    }
    printf("Intentelo nuevamente.\n");
    while(getchar() != '\n');
  }

  List *lista_genero = (List*)par_genero->value;
  
  /* Aca recorremos toda la lista de generos y seleccionamos aquellas
  peliculas que pertenezcan a la decada pedida, los almacenamos en un
  arreglo temporal para poder ordenarlas segun su rating luego*/
  
  pelicula *aux[5000];
  int n_aux = 0;

  pelicula *p = (pelicula*)list_first(lista_genero);
  while (p!= NULL){
    int decada_peli = (p->year / 10) * 10;
    if (decada_peli == decada_buscada){
      int duplicada = 0;
      for (int i = 0; i<= n_aux; i++){
        if (strcmp(aux[i]->id, p->id) == 0){
          duplicada = 1;
          break;
        }
      }
      if (!duplicada) aux[n_aux++] = p;
    }
    p = (pelicula *)list_next(lista_genero);
  }
  // No hay resultados
  if (n_aux == 0){
    printf("\nNo existen peliculas adecuadas\n");
    return;
  }
  // Usamos un bubble sort para ordenar
  for (int i = 0; i < n_aux-1; i++){
    for ( int j = 0; j < n_aux - i - 1; j++){
      if (aux[j]->rating < aux[j + 1]->rating){
        pelicula *auxiliar = aux[j];
        aux[j] = aux[j + 1];
        aux[j + 1] = auxiliar;
      }
    }
  }
  int total = (cantidad <= n_aux) ? cantidad : n_aux;
  if (total < cantidad){
    printf("Solo se encuentran %d peliculas de las %d\n",total,cantidad);
  }
 /* Construimos la lista enlazada doble con todas las peliculas     seleccionadas, y navegamos con list_prev, list_next */
  List *maraton = list_create();
  for(int i = 0; i < total; i++){
    list_pushBack(maraton, aux[i]);
  }
 // Navegacion interactiva
  printf("\n Lista generada con %d peliculas.\n",total);
  printf("Navegacion : [S] Siguiente | [A] Anterior | [C] Confirmar | [V]Volver\n");
  pelicula *actual = (pelicula*)list_first(maraton);
  int pos = 1;
  int confirmada = 0;
  char c;

  while (actual != NULL){
    mostrar_marathon(actual,pos,total);
    printf("\nOpcion [s]iguiente | [a]nterior | [c]onfirmar | [v]olver : ");

    scanf(" %c", &c);

    if (c == 's'){
      pelicula *sig = (pelicula*)list_next(maraton);
      if (sig != NULL){
        actual = sig;
        pos++;
      }
      else printf("Ya estas en la ultima pelicula\n");
    } else if (c == 'a'){
      if (pos == 1){
        printf("Ya estas en la primera pelicula\n");
      }else{
        pelicula *ant = (pelicula*)list_prev(maraton);
        if(ant != NULL){
          actual = ant;
          pos--;
        }
        else printf("Ya estas en la primera pelicula\n");
      }
    }else if ( c == 'c'){
      confirmada = 1;
      break;
    }else if (c == 'v'){
      printf("\nMaraton decartada. Volviendo al menu...\n");
      break;
    }else{
      printf("Opcion invalida");
    }
  }
// Resultado final
  if (confirmada){
    limpiarPantalla();
    printf("========================================\n");
    printf("        Maraton Confirmada\n");
    printf("========================================\n\n");
    printf("Genero: %s  |  Decada: %d  |  Peliculas: %d\n\n",genero,decada_buscada,total);

    pelicula *temporal = (pelicula*)list_first(maraton);
    int i = 1;
    while (temporal != NULL){
      printf("  %2d. %-40s (%d) \n",i,temporal->title,temporal->year);
      temporal = (pelicula*)list_next(maraton);
      i++;
    }
    printf("\nDisfruta de tu maraton\n");
  }
  //Liberamos memoria
  list_clean(maraton);
  return;
}

void cinematch(Map* principal, Map* generos, Map* decada,Map* titulos)
{
  printf("Ingrese el nombre de una Pelicula! : ");
  int c;
  while((c = getchar()) != '\n' && c != EOF);
  
  //PRIMERO RECIBIMOS EL NOMBRE DE UNA PELICULA.
  char nombrePelicula[100];
  MapPair* par = NULL;
  while(1)
  {  
    //LEER EL NOMBRE DE LA PELICULA CON ESPACIOS EN BLANCO
    fgets(nombrePelicula,sizeof(nombrePelicula),stdin);
    //ELIMINAR SALTO DE LINEA
    nombrePelicula[strcspn(nombrePelicula,"\r\n")] = '\0';
    //HACEMOS LA BUSQUEDA PARA QUE LEA ESPACIOS EN BLANCO!
    par = map_search(titulos,(void*)nombrePelicula);
    if(par == NULL)
    {
      printf("Actualmente la pelicula ingresada no se encuentra en nuestro catalogo..\n");
    }
    else break;
  }

  
  
  //accedemos a el valor del par(la pelicula) en el mapa de titulos
  pelicula* ingresada = (pelicula*)par->value;
  printf("procesando afinidad.....\n");
  //arreglo que tndra el top 5
  pelicula* top5[5] = {NULL, NULL, NULL, NULL, NULL};
  //CREAMOS UN MAPA PARA EVITAR REPETICION!
  Map* procesadas = map_create(is_equal_str);

  //BUSCAR COINCIDENCIAS EN LOS GENEROS

  char* genBase = (char*)list_first(ingresada->genres);
  while(genBase != NULL)
  {
      MapPair* parGenero = map_search(generos,genBase);

      if(parGenero != NULL)
      {
        List* listaMismoGenero = (List*) parGenero->value;
        pelicula* peliEvaluar=(pelicula*) list_first(listaMismoGenero);
      
        while(peliEvaluar != NULL)
          {
            if(strcmp(peliEvaluar->id,ingresada->id) != 0 && map_search(procesadas,peliEvaluar->id) == NULL)
            {
              map_insert(procesadas,peliEvaluar->id,peliEvaluar);

              //CALCULO DEL PUNTAJE!
              int similitud = 0;
              //DIRECTOR
              if(strcmp(ingresada->director,peliEvaluar->director) == 0) similitud += 3;
              //DECADA
              if((ingresada->year / 10) == (peliEvaluar->year / 10)) similitud += 2;
              //CADA GENERO!
              char* g1 = (char*) list_first(ingresada->genres);
              while(g1 != NULL)
                {
                  char* g2 = (char*)list_first(peliEvaluar->genres);
                  while(g2 != NULL)
                    {
                      if(strcmp(g1,g2) == 0)
                      {
                        similitud++;
                        break;
                      }
                      g2= (char *)list_next(peliEvaluar->genres);
                    }
                  g1 = (char*)list_next(ingresada->genres);
                }

              peliEvaluar->similitud = similitud;
              if(top5[4] == NULL || similitud > top5[4]->similitud)
              {
                for(int i = 0; i < 5; i++)
                  {
                    if(top5[i]==NULL || similitud > top5[i]->similitud)
                    {
                      for(int j = 4; j > i; j--)
                        {
                          top5[j] = top5[j-1];
                        }
                      top5[i]= peliEvaluar;
                      break;
                    }
                  }
              }
              
            }
            peliEvaluar= (pelicula*) list_next(listaMismoGenero);
          }
          
      }
      genBase= (char*)list_next(ingresada->genres);
  }
    printf("TOP 5 PELICULAS RECOMENDADAS\n");
    for(int i = 0; i < 5; i++)
      {
        if(top5[i]!=NULL)
        {
          printf("%d. Título: %s, Año: %d, Géneros: ",i+1,top5[i]->title,top5[i]->year);
          char* g= (char*) list_first(top5[i]->genres);
          while(g != NULL)
            {
              printf("%s",g);
              g= (char*)list_next(top5[i]->genres);
              if(g!= NULL) printf(", ");
            }
          printf(" Puntaje de Afinidad : %i \n",top5[i]->similitud);
        } 
      }
}



int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
                
  // Creación de un mapa para almacenar películas, utilizando una función de
  // comparación que trabaja con claves de tipo string e int.
  //MAPA PRINCIPAL
  Map *pelis_byid = map_create(is_equal_str);
  //M. GENEROS
  Map *pelis_bygenres = map_create(is_equal_str);
  //M. DECADA
  Map *pelis_decada = map_create(is_equal_int);
  //M. TITULO
  Map *pelis_titulo = map_create(is_equal_str);
  
  cargar_peliculas(pelis_byid, pelis_bygenres,pelis_decada,pelis_titulo);

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      cinematch(pelis_byid,pelis_bygenres,pelis_decada,pelis_titulo);
      break;
    case '2':
      //CINECLASH
      break;
    case '3':
      marathon_maker(pelis_bygenres,pelis_decada);
      break;
    case '4':
      //CINE ORACLE
      break;
    }
    presioneTeclaParaContinuar();

  } while (opcion != '5');

  return 0;
}
