#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#define ABS(x) ((x) < 0 ? -(x) : (x))
#include "tdas/queue.h"

typedef struct {
  char id[100];
  char title[300];
  List *genres;
  int year;
  char director[300];
  int similitud;
  float rating;
} pelicula;

typedef struct{
  int tipo;
  char atributo[50];
  char texto[200];
  int usada;
}preguntaOracle;


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
void cargar_peliculas(Map *pelis_byid, Map *pelis_bygenres, Map *pelis_decada, Map *pelis_titulo) {
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
    peli->year = atoi(campos[10]); // Asigna año, convirtiendo de cadena a entero
    strcpy(peli->director, campos[14]);
    peli->rating = atof(campos[8]);
    
    
    // Inserta la película en el mapa usando el ID como clave
    map_insert(pelis_byid, peli->id, peli);
    map_insert(pelis_titulo, peli->title, peli);
  
    // Obtiene el primer género de la lista de géneros de la película
    char *genre = list_first(peli->genres);
    // Itera sobre cada género de la película
    while (genre != NULL) {

        if (genre[0] == ' ')genre++;
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


int cargar_pool_preguntas(preguntaOracle pool[]){
  FILE *archivo = fopen("PREGUNTASORACLE.CSV", "r");
  if (archivo == NULL){
    printf("ERROR: no se encontro el archivo PREGUNTASORACLE.CSV\n");
    return 0;
  }
  int count = 0;
  char linea[300];
  while (fgets(linea, sizeof(linea), archivo) != NULL){
    linea[strcspn(linea, "\r\n")] = '\0';
    if (strlen(linea) == 0)continue;

    char *token = strtok(linea, ";");
    if (token != NULL) pool[count].tipo = atoi(token);

    token = strtok(NULL, ";");
    if (token != NULL) strcpy(pool[count].atributo, token);

    token = strtok(NULL, ";");
    if (token != NULL) strcpy(pool[count].texto, token);

    pool[count].usada = 0;
    count++;
  }
  fclose(archivo);
  return count;
}

void cineoracle(Map* pelis_byid){
  limpiarPantalla();
  puts("====================================================");
  puts("            ¡ Bienvenidos a CineOracle !            ");
  puts("====================================================");
  puts("El Oraculo leera tu mente. Responde con sinceridad\n");

  preguntaOracle pool[71];
  int total_preguntas = cargar_pool_preguntas(pool);

  if (total_preguntas == 0)return;

  List* candidatas = list_create();
  int cant_candidatas = 0;

  MapPair* pair = map_first(pelis_byid);
  while(pair != NULL){
    list_pushBack(candidatas, pair->value);
    cant_candidatas++;
    pair = map_next(pelis_byid);
  }
  srand(time(NULL));

  Queue* cola_preguntas = queue_create(NULL);

  while(cant_candidatas > 1){
    int indices_sweet_spot[71];
    int cant_sweet_spot = 0;
    int indices_respaldo[71];
    int cant_respaldo = 0;
    int mejor_distancia_respaldo = 999999;
    
    int limite_inferior = cant_candidatas * 0.3;
    int limite_superior = cant_candidatas * 0.7;

    for (int i = 0; i < total_preguntas; i++){
      if (pool[i].usada)continue;

      int coinciden = 0;

      pelicula* p = (pelicula*)list_first(candidatas);
      while(p != NULL){
        if(pool[i].tipo == 1){
          char* g = list_first(p->genres);
          while (g != NULL){
            if(g[0] == ' ')g++;
            if (strcmp(g, pool[i].atributo) == 0){
              coinciden++;
              break;
            }
            g = list_next(p->genres);
          }
        }
        else if(pool[i].tipo == 2){
          if (((p->year / 10)*10) == atoi(pool[i].atributo))coinciden++;
        }
        else if(pool[i].tipo == 3){
          if (strstr(p->director, pool[i].atributo) != NULL)coinciden++;
        }
        else if(pool[i].tipo == 4){
          if (p->rating >= atof(pool[i].atributo))coinciden++;
        }
        else if(pool[i].tipo == 5){
          if (p->year < atoi(pool[i].atributo))coinciden++;
        }
        p = (pelicula*)list_next(candidatas);
      }

      if (coinciden <= 0 || coinciden >= cant_candidatas)continue;

      if (coinciden >= limite_inferior && coinciden <= limite_superior){
        indices_sweet_spot[cant_sweet_spot] = i;
        cant_sweet_spot++;
      }
      int distancia = ABS(coinciden - (cant_candidatas / 2));
      if (distancia < mejor_distancia_respaldo){
        mejor_distancia_respaldo = distancia;
        indices_respaldo[0] = i;
        cant_respaldo = 1;
      }
      else if (distancia == mejor_distancia_respaldo || distancia == mejor_distancia_respaldo + 1){
        indices_respaldo[cant_respaldo] = i;
        cant_respaldo++;
      }
    }
    int indice_elegido = -1;
    if (cant_sweet_spot > 0){
      indice_elegido = indices_sweet_spot[rand() % cant_sweet_spot];
    }
    else if (cant_respaldo > 0){
      indice_elegido = indices_respaldo[rand() % cant_respaldo];
    }
    else break;
   
  pool[indice_elegido].usada = 1;

  preguntaOracle* pregunta = (preguntaOracle*)malloc(sizeof(preguntaOracle));
  *pregunta = pool[indice_elegido];
  queue_insert(cola_preguntas, pregunta);

  preguntaOracle* preguntaActual = (preguntaOracle*)queue_remove(cola_preguntas);

  int respuesta = 0;

  while(1){
    printf("\n-> %s\n", preguntaActual->texto);
    printf("[1] SI\n[2] NO\nSeleccion: ");
    if(scanf(" %d", &respuesta) == 1 && (respuesta == 1 || respuesta == 2))break;
    printf("Respuesta invalida. Por favor ingresa 1 0 2\n");
    while(getchar() != '\n');
  }

  pelicula* p = (pelicula*)list_first(candidatas);

  while(p != NULL){
    int tiene_atributo = 0;

    if (preguntaActual->tipo == 1){
      char* g = list_first(p->genres);
      while (g != NULL){
        if (g[0] == ' ')g++;
        if (strcmp(g, preguntaActual->atributo) == 0){
          tiene_atributo = 1;
          break;
        }
        g = list_next(p->genres);
      }
    }
    else if (preguntaActual->tipo == 2){
      if (((p->year / 10) * 10) == atoi(preguntaActual->atributo))tiene_atributo = 1;
    }
    else if (preguntaActual->tipo == 3){
      if (strstr(p->director, preguntaActual->atributo) != NULL)tiene_atributo = 1;
    }
    else if (preguntaActual->tipo == 4){
      if (p->rating >= atof(preguntaActual->atributo))tiene_atributo = 1;
    }
    else if (preguntaActual->tipo == 5){
      if (p->year < atoi(preguntaActual->atributo))tiene_atributo = 1;
    }
    int mantener = ((respuesta == 1 && tiene_atributo) || (respuesta == 2 && !tiene_atributo));

    if (!mantener){
      list_popCurrent(candidatas);
      cant_candidatas -= 1;
      p = (pelicula*)list_first(candidatas);
    }
    else p = (pelicula*)list_next(candidatas);
  }
  free(preguntaActual);
  if (cant_candidatas == 0){
    printf("\n!Eres demasiado dificil de leer. Intentalo de nuevo¡\n");
    break;
  }
}

if (cant_candidatas >= 1){
  pelicula* mejorPelicula = (pelicula*)list_first(candidatas);
  pelicula* control = (pelicula*)list_next(candidatas);
  while(control != NULL){
    if (control->rating > mejorPelicula->rating)mejorPelicula = control;
    control = (pelicula*)list_next(candidatas);
  }

  printf("\n*****************************************************\n");
  printf("                ¡ El ORACULO HA HABLADO !                \n");
  if (cant_candidatas > 1){
    printf("  Hubo un empate de %d peliculas, pero he seleccionado  \n", cant_candidatas);
    printf("  la mejor valorada por la critica para ti:        \n\n");
  }
  else printf("  La pelicula PERFECTA para ti el dia de hoy es:      \n\n");
  
  printf("    -> %s (%d) \n", mejorPelicula->title, mejorPelicula->year);
  printf("    Director: %s\n", mejorPelicula->director);
  printf("    Rating IMDb: %.1f / 10.0\n", mejorPelicula->rating);

  printf("    Generos: ");
  char* g = (char*)list_first(mejorPelicula->genres);
  while(g != NULL){
    if (g[0] == ' ')g++;
    printf("%s", g);
    g = (char*)list_next(mejorPelicula->genres);
    if (g != NULL) printf(", ");
  }
  printf("\n*****************************************************\n");
}

free(cola_preguntas);
free(candidatas);
}


int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
                
  
  Map *pelis_byid = map_create(is_equal_str);
  Map *pelis_bygenres = map_create(is_equal_str);
  Map *pelis_decada = map_create(is_equal_int);
  Map *pelis_titulo = map_create(is_equal_str);

  cargar_peliculas(pelis_byid, pelis_bygenres,pelis_decada, pelis_titulo);

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      //CINEMATCH
      break;
    case '2':
      //CINECLASH
      break;
    case '3':
      //MARATHON MAKER
      break;
    case '4':
      cineoracle(pelis_byid);
      break;
    }
    presioneTeclaParaContinuar();

  } while (opcion != '5');

  return 0;
}
