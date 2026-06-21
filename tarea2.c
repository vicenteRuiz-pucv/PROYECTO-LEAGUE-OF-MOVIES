#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
  char id[100];
  char title[300];
  List *genres;
  int year;
  char director[300];
  int similitud;
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

int sumLista(List *lista, MapPair *par){
  lista = (List*)par->value;
  int sum;
  pelicula *peli = list_first(lista);
  while(peli != NULL){
    sum++;
    peli = list_next(lista);
  }
  return sum;
}

void cineclash(Map *mapGeneros){
  char generoElegido[100];
  MapPair *par = NULL;
  List *lista_peliculas = NULL;
  printf("Ingrese el genero para iniciar el torneo: \n"); //pedimos el genero para hacer el torneo
  fgets(generoElegido, sizeof(generoElegido), stdin);
  par = map_search(mapGeneros, generoElegido); // se busca en el mapa de generos si este existe
  if(par == NULL){
    printf("Genero no encontrado.");
    return;
  }
  int peliculasDelGenero = sumLista(lista_peliculas, par); //contamos cuantas peliculas de este genero hay

  //creamos un arreglo temporal para guardar la lista de peliculas de el genero elegido
  pelicula **tempPelis = (pelicula**)malloc(peliculasDelGenero * sizeof(pelicula *));
  int i = 0;
  pelicula *pelis = list_first(lista_peliculas);
  while(pelis != NULL){
    tempPelis[i] = pelis;
    i++;
    pelis = list_next(lista_peliculas);
  }
  Queue *torneo = queue_create(NULL);
  int pelisYaEscogidas = 0;
  int posUsadas[8];
  while(pelisYaEscogidas < 8){
    int posRandom = rand() % peliculasDelGenero;
    bool seRepite = false;
    for(int j = 0; j < pelisYaEscogidas; j++){
      if(posUsadas[j] == posRandom){
        seRepite = true;
        break;
      }
    }
    if(seRepite == false){
      posUsada[pelisYaEscogidas] = posRandom;
      queue_insert(torneo, tempPelis[posRandom]);
      pelisYaEscogidas++;
    }
  }
  free(tempPelis);

  printf("Comienza el CineClash: %s\n", generoElegido);
  for(int vs = 1; vs <= 7; vs++){
    pelicula *peliA = (pelicula*)queue_remove(torneo);
    pelicula *peliB = (pelicula*)queue_remove(torneo);

    int voto = 0;
    if(vs == 1 || vs == 2 || vs == 3 || vs == 4)
      printf("CUARTOS DE FINAL DEL CINECLASH!\n");
    else if(vs == 5 || vs == 6)
      printf("SEMIFINALES DEL CINECLASH!\n");
    else
      printf("FINAL DEL CINECLASH!\n");
    
    while(voto != 1 && voto != 2){
      printf("Versus %d: [1] %s vs [2] %s\n", vs, peliA->title, peliB->title);
      printf("Ingresa tu eleccion: ");
      scanf("%d", &voto);
      if(voto !=  1 && voto != 2){
        printf("Voto invalido");
      }
    }
    if(voto == 1){
      queue_insert(torneo, peliA);
      printf("El ganador de esta ronda fue: %s\n", peliA->title);
    }
    else{
      queue_insert(torneo, peliB);
      printf("El ganador de esta ronda fue: %s\n", peliB->title");
    }
  }
  pelicula *ganador = (pelicula*)queue_remove(torneo);
  printf("-------------------------------------------\n");
  printf("El Ganador Del CineClash Es: %s\n", ganador->title);
  printf("-------------------------------------------");
  queue_clean(torneo);
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
  srand(time(NULL)); //para no generar la misma serie de numeros aleatorios cada vez que se inicie el programa

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      cinematch(pelis_byid,pelis_bygenres,pelis_decada,pelis_titulo);
      break;
    case '2':
      //cineclash(pelis_byid, pelis_bygenres)
      break;
    case '3':
      //MARATHON MAKER
      break;
    case '4':
      //CINE ORACLE
      break;
    }
    presioneTeclaParaContinuar();

  } while (opcion != '5');

  return 0;
}
