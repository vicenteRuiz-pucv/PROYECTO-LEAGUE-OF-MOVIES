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

} pelicula;

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

void cargar_peliculas(Map *pelis_byid, Map *pelis_bygenres, Map *pelis_decada,Map* pelis_titulo) {


void normalizar_minusculas(char* str){
  for (int i = 0; str[i]; i++){
    str[i] = tolower((unsigned char)str[i]);
  }
}

char *limpiar_espacios(char *str){
  while(isspace((unsigned char)*str))str++;
  char *fin = str + strlen(str) - 1;
  while(fin > str && isspace((unsigned char)*fin)) *fin-- = '\0';
  return str;
}

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



//FUNCION CINEMATCH
void cinematch(Map* principal, Map* generos, Map* decada,Map* titulos)
{
  //PRINTEAMOS PARA AÑADIR FORMATO
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
  //ITERAMOS LA LISTA DE TODOS LOS GENEROS!
  char* genBase = (char*)list_first(ingresada->genres);
  while(genBase != NULL)
  {
    //obtenemos el par de el genero iesimo..  
    MapPair* parGenero = map_search(generos,genBase);
      
      if(parGenero != NULL)
      {
        
        List* listaMismoGenero = (List*) parGenero->value;
        pelicula* peliEvaluar=(pelicula*) list_first(listaMismoGenero);
        //recorremos todas las peliculas por genero..
        while(peliEvaluar != NULL)
          {
            //no evaluar peliculas procesadas y no evaluarla consigo misma
            if(strcmp(peliEvaluar->id,ingresada->id) != 0 && map_search(procesadas,peliEvaluar->id) == NULL)
            {
              //insertamos en el mapa de ya procesadas
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
                        break; //salir del ciclo interior si se encontro coincidencia
                      }
                      g2= (char *)list_next(peliEvaluar->genres);
                    }
                  g1 = (char*)list_next(ingresada->genres);
                }
              //registrar en el struct la similitud


              //algoritmo para ordenar el arreglo de el top 5!!
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
            //pasar a la siguiente pelicula
            peliEvaluar= (pelicula*) list_next(listaMismoGenero);
          }
          
      }
    //pasar al siguiente genero  
    genBase= (char*)list_next(ingresada->genres);
  }
  //MOSTRAMOS EL TOP 5 DE LAS PELICULAS RECOMENDADAS  
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



// Cargar pool de preguntas desde archivo PREGUNTASORACLE.CSV
// Se almacenan en la variable pool de tipo "preguntaOracle"
// Retorna la cantidad de preguntas cargadas correctamente
int cargar_pool_preguntas(preguntaOracle pool[]){
  // Aqui se intenta abrir el archivo CSV en modo lectura
  FILE *archivo = fopen("PREGUNTASORACLE.CSV", "r");

  // Si el archivo no existe o no se puede abrir, muetra error y retorna 0
  if (archivo == NULL){
    printf("ERROR: no se encontro el archivo PREGUNTASORACLE.CSV\n");
    return 0;
  }
  int count = 0; // Contador de preguntas cargadas
  char linea[300]; // Cadena temporal donde se leera cada linea del CSV

  // Lee linea a linea hasta el fin del archivo
  while (fgets(linea, sizeof(linea), archivo) != NULL){

    // Elimina el salto de linea (\r\n o \n) al final de la linea
    linea[strcspn(linea, "\r\n")] = '\0';

    // Ignora lineas vacias
    if (strlen(linea) == 0)continue;


    // Separa la linea por ";" como separador
    // Primer token sera el tipo de pregunta (1=género, 2=década, 3=director, 4=rating, 5=año)
    char *token = strtok(linea, ";");
    if (token != NULL) pool[count].tipo = atoi(token);

    // Segundo token sera el atributo de referencia (ej: action, 1990, The godfather)
    token = strtok(NULL, ";");
    if (token != NULL) strcpy(pool[count].atributo, token);

    // Tercer token sera el texto que se le muestre al usuario
    token = strtok(NULL, ";");
    if (token != NULL) strcpy(pool[count].texto, token);

    // Marca la pregunta como no usada aun
    pool[count].usada = 0;
    count++; // Avanza a la siguiente posicion del arreglo
  }
  fclose(archivo); // Se cierra el archivo tras terminar la lectura
  return count; // Retorna el total de preguntas cargadas
}


// Funcion principal del modo CineOracle
// Recibe el mapa de peliculas indexado por ID
void cineoracle(Map* pelis_byid){

  limpiarPantalla(); // Limpia la consola antes de mostrar el menu al usuario
  puts("====================================================");
  puts("            ¡ Bienvenidos a CineOracle !            ");
  puts("====================================================");
  puts("El Oraculo leera tu mente. Responde con sinceridad\n");

  preguntaOracle pool[71]; // Aqui se guardan las 70 preguntas de la pool de PREGUNTASORACLE.CSV
  int total_preguntas = cargar_pool_preguntas(pool); // Se cargan las preguntas desde el archivo CSV

  // Si no se cargan preguntas, no se puede continuar
  if (total_preguntas == 0)return;

  List* candidatas = list_create(); // Lista de las peliculas candidatas
  int cant_candidatas = 0; // Contador de candidatas actuales

  // Recorre el mapa e inserta todas las peliculas en la lista de candidatas
  MapPair* pair = map_first(pelis_byid);
  while(pair != NULL){
    list_pushBack(candidatas, pair->value); // Agrega la pelicula al final de la lista
    cant_candidatas++;
    pair = map_next(pelis_byid);
  }
  srand(time(NULL)); // Se inicializa la semilla aleatoria con el tiempo actual

  Queue* cola_preguntas = queue_create(NULL); // Cola auxiliar para las preguntas que se realizaran

  // Bucle principal mientras haya mas de una pelicula candidata
  while(cant_candidatas > 1){
    // Arreglos para candidatas a preguntas "ideales" (sweet spot -> 70% a 30% de descarte de peliculas) y las de respaldo
    int indices_sweet_spot[71];
    int cant_sweet_spot = 0;
    int indices_respaldo[71];
    int cant_respaldo = 0;
    int mejor_distancia_respaldo = 999999;

    // Limites del sweet spot: Preguntas que dividen entre el 30% y 70% de peliculas candidatas
    int limite_inferior = cant_candidatas * 0.3;
    int limite_superior = cant_candidatas * 0.7;

    // Aqui se evalua cada pregunta del pool que aun no ha sido usada
    for (int i = 0; i < total_preguntas; i++){
      if (pool[i].usada)continue; // Salta las preguntas ya utilizadas

      int coinciden = 0; // Cuantas peliculas candidatas cumplen el "atributo" de esa pregunta

      // Recorre todas las peliculas candidatas
      pelicula* p = (pelicula*)list_first(candidatas);
      while(p != NULL){

        // Tipo 1: genero - verifica si la pelicula tiene el genero indicado
        if(pool[i].tipo == 1){
          char* g = list_first(p->genres);
          while (g != NULL){
            if(g[0] == ' ')g++; // Ignora espacios iniciales en el genero (porciacaso)
            if (strcmp(g, pool[i].atributo) == 0){
              coinciden++; // La pelicula tiene el genero
              break;
            }
            g = list_next(p->genres);
          }
        }
        // Tipo 2: decada - verifica si el año de la pelicula corresponde a la decada indicada
        else if(pool[i].tipo == 2){
          if (((p->year / 10)*10) == atoi(pool[i].atributo))coinciden++;
        }
        // Tipo 3: director - verifica si el nombre del director contiene el atributo
        else if(pool[i].tipo == 3){
          if (strstr(p->director, pool[i].atributo) != NULL)coinciden++;
        }
        // Tipo 4: rating minimo - verifica si el rating de la pelicula supera el umbral
        else if(pool[i].tipo == 4){
          if (p->rating >= atof(pool[i].atributo))coinciden++;
        }
        // tipo 5: año anterior - verifica si la pelicula es anterior al año indicado
        else if(pool[i].tipo == 5){
          if (p->year < atoi(pool[i].atributo))coinciden++;
        }
        p = (pelicula*)list_next(candidatas);
      }

      // Descarta preguntas que no eliminan ninguna candidata (0 o 100%)
      if (coinciden <= 0 || coinciden >= cant_candidatas)continue;

      // Si la pregunta cae en el sweet spot (30%-70%) y se agrega a ese grupo
      if (coinciden >= limite_inferior && coinciden <= limite_superior){
        indices_sweet_spot[cant_sweet_spot] = i;
        cant_sweet_spot++;
      }

      // Calcula que tan cerca esta esa pregunta de dividir en mitades exactas (entropía)
      int distancia = ABS(coinciden - (cant_candidatas / 2));

      // Actualiza el mejor respaldo con la pregunta mas cercana al centro
      if (distancia < mejor_distancia_respaldo){
        mejor_distancia_respaldo = distancia;
        indices_respaldo[0] = i;
        cant_respaldo = 1;
      }

      // Si hay empate en distancia o es muy cercana, tambien se agrega al respaldo
      else if (distancia == mejor_distancia_respaldo || distancia == mejor_distancia_respaldo + 1){
        indices_respaldo[cant_respaldo] = i;
        cant_respaldo++;
      }
    }
    
    int indice_elegido = -1;

    // Prioriza el sweet spot, si no hay, usa el respaldo, si tampoco hay, termina la funcion
    if (cant_sweet_spot > 0){
      indice_elegido = indices_sweet_spot[rand() % cant_sweet_spot]; // Elige aleatoriamente pregunta del sweet spot
    }
    else if (cant_respaldo > 0){
      indice_elegido = indices_respaldo[rand() % cant_respaldo]; // Elige aleatoriamente pregunta del respaldo
    }
    else break; // No hay preguntas utiles

  // Marca la pregunta seleccionada como usada para no volver a repetir
  pool[indice_elegido].usada = 1;

  // Crea una copia dinamica de la pregunta para insertarla en la cola
  preguntaOracle* pregunta = (preguntaOracle*)malloc(sizeof(preguntaOracle));
  *pregunta = pool[indice_elegido];
  queue_insert(cola_preguntas, pregunta); // Inserta la pregunta en la cola


  // Extrae inmediatamente la pregunta de la cola para procesarla
  preguntaOracle* preguntaActual = (preguntaOracle*)queue_remove(cola_preguntas);

  int respuesta = 0; // Almacena la respuesta del usuario (si o no)

  // bucle de validacion de la entrada del usuario: se repite hasta recibir 1 o 2
  while(1){
    printf("\n-> %s\n", preguntaActual->texto); // Muestra la pregunta
    printf("[1] SI\n[2] NO\nSeleccion: ");
    if(scanf(" %d", &respuesta) == 1 && (respuesta == 1 || respuesta == 2))break;
    printf("Respuesta invalida. Por favor ingresa 1 0 2\n");
    while(getchar() != '\n'); // limpia buffer de entrada
  }

  // Recorre la lista de candidatas y elimina las que no coincidan con la respuesta
  pelicula* p = (pelicula*)list_first(candidatas);

  while(p != NULL){
    // esto es un verificado para saber si la pelicula tiene el atributo de la pregunta
    int tiene_atributo = 0; 

    // Repite la logica de verificacion del atributo para la pelicula actual
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

    // Aqui se decide si mantener la pelicula:
    // SI (1) -> se queda si tiene el atributo
    // NO (2) -> se queda si NO tiene el atributo
    int mantener = ((respuesta == 1 && tiene_atributo) || (respuesta == 2 && !tiene_atributo));

    if (!mantener){
      list_popCurrent(candidatas); // Elimina la pelicula actual de la lista de candidatas
      cant_candidatas -= 1;
      p = (pelicula*)list_first(candidatas); // Reinicia desde el inicio de la lista
    }
    else p = (pelicula*)list_next(candidatas); // Avanza a la siguiente pelicula
  }
    
  free(preguntaActual); // Libera la memoria de la pregunta procesada

  // Si se eliminan todas las candidatas, el oraculo no puede adivinar
  if (cant_candidatas == 0){
    printf("\n!Eres demasiado dificil de leer. Intentalo de nuevo¡\n");
    break;
  }
}
// Al terminar el bucle, siq uedo al menos 1 candidata se elige la mejor 
if (cant_candidatas >= 1){
  // Busca la pelicula con el mayor rating entre las candidatas restantes
  pelicula* mejorPelicula = (pelicula*)list_first(candidatas);
  pelicula* control = (pelicula*)list_next(candidatas);
  while(control != NULL){
    if (control->rating > mejorPelicula->rating)mejorPelicula = control;
    control = (pelicula*)list_next(candidatas);
  }

  // Imprime resultado final del oraculo
  printf("\n*****************************************************\n");
  printf("                ¡ El ORACULO HA HABLADO !                \n");
  if (cant_candidatas > 1){
    // Si quedan varias candidatas, se informa el empate y elige la de mayor rating
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
    // Elimina espacios inicial si lo hay
    if (g[0] == ' ')g++;
    printf("%s", g);
    g = (char*)list_next(mejorPelicula->genres);
    if (g != NULL) printf(", ");
  }
  printf("\n*****************************************************\n");
}

  // Libera la cola y la lista de candidatas al terminar
free(cola_preguntas);
free(candidatas);
}



int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
                
  
  Map *pelis_byid = map_create(is_equal_str);
  Map *pelis_bygenres = map_create(is_equal_str);
  Map *pelis_decada = map_create(is_equal_int);

  //M. TITULO
  Map *pelis_titulo = map_create(is_equal_str);
  
  cargar_peliculas(pelis_byid, pelis_bygenres,pelis_decada,pelis_titulo);

  Map *pelis_titulo = map_create(is_equal_str);

  cargar_peliculas(pelis_byid, pelis_bygenres,pelis_decada, pelis_titulo);


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
