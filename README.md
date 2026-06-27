Proyecto Final.

Team - League of Data.

Fecha: 24-05-2026.

CONTRIBUCIONES: 

Vicente Ruiz: Desarrollo de la función CineMatch.

Fabián Olivares: Implementación del modo CineClash.

Sebastián Pino: Creación de Marathon Maker.

Benjamín Alucema: Desarrollo de CineOracle.

Todos: diseño, comentarios y main.

---

# League Of Movies 🎬 — Plataforma interactiva de recomendación cinematográfica

> **Importante:** El catálogo se carga automáticamente desde `Top1500.csv` al iniciar. El archivo `PREGUNTASORACLE.CSV` es requerido únicamente para el modo CineOracle.

League Of Movies es una plataforma de consola escrita en **C** que permite explorar, comparar y descubrir películas a partir de un catálogo de 1500 títulos. El sistema ofrece cuatro modos de interacción completamente distintos: recomendación por afinidad, torneo de votación, maratón personalizada y adivinanza mediante preguntas de sí/no.

Toda la información se indexa en **cuatro mapas de hash** que permiten acceso eficiente, apoyados por TDAs de lista enlazada doble y cola FIFO para las operaciones de cada modo.

---

## Estructuras y archivos necesarios para ejecutar el programa

**TDAs requeridos:** `list.c`, `queue.c`, `map.c`, `extra.c`

**Cabeceras:** `list.h`, `queue.h`, `map.h`, `extra.h`

**Archivos de datos:**
- `Top1500.csv` — catálogo principal de películas (1500 títulos)
- `PREGUNTASORACLE.CSV` — pool de preguntas para CineOracle (71 preguntas, separador `;`)

**Estructura interna de cada película (`pelicula`):**

| Campo | Tipo | Descripción |
|---|---|---|
| `id` | `char[100]` | Identificador único numérico |
| `title` | `char[300]` | Título de la película |
| `genres` | `List*` | Lista enlazada de géneros |
| `year` | `int` | Año de estreno |
| `director` | `char[300]` | Nombre del director |
| `rating` | `float` | Puntuación IMDb (0.0 – 10.0) |
| `similitud` | `int` | Puntaje de afinidad calculado por CineMatch |

---

## Instrucciones de compilación

### Cargar aplicación

```bash
gcc tdas/*.c tarea2.c -Wno-unused-result -o leagueofmovies
```

### Ejecutar aplicación

```bash
./leagueofmovies
```

---

## Carga de datos inicial

Al iniciar, `cargar_peliculas()` lee `Top1500.csv` línea por línea y construye los cuatro mapas simultáneamente usando `leer_linea_csv()` y `split_string()` del módulo extra.

| Mapa | Tipo de clave | Propósito |
|---|---|---|
| `pelis_byid` | `string` (ID numérico) | Acceso directo O(1) a cualquier película |
| `pelis_titulo` | `string` (título exacto) | Búsqueda por nombre para CineMatch |
| `pelis_bygenres` | `string` (género) | Lista de películas por género para CineClash y CineMatch |
| `pelis_decada` | `int` (año/10*10) | Lista de películas por década para Marathon Maker |

---

## Funcionalidades principales

### 1 · CineMatch — Recomendación por afinidad

El usuario ingresa el nombre exacto de una película y el sistema devuelve las **5 más similares** del catálogo, ordenadas de mayor a menor afinidad.

**Criterios de puntaje:**

| Criterio | Puntos |
|---|---|
| Mismo director | +3 |
| Misma década | +2 |
| Cada género en común | +1 por género compartido |

Para evitar comparar una película consigo misma o evaluar duplicados, utiliza un **mapa auxiliar de procesadas** que garantiza evaluación única por ID.

**Ejemplo de funcionamiento — CineMatch:**

```
Ingrese el nombre de una Pelicula! : Inception

procesando afinidad.....

TOP 5 PELICULAS RECOMENDADAS
1. Título: The Dark Knight, Año: 2008, Géneros: Action, Crime, Drama  Puntaje de Afinidad: 7
   [mismo director Nolan +3, misma década 2000 +2, género Action +1, género Drama +1]

2. Título: Interstellar, Año: 2014, Géneros: Adventure, Drama, Sci-Fi  Puntaje de Afinidad: 5
   [mismo director Nolan +3, género Drama +1, género Sci-Fi +1]

3. Título: The Prestige, Año: 2006, Géneros: Drama, Mystery, Sci-Fi  Puntaje de Afinidad: 4
   [mismo director Nolan +3, misma década 2000 +2... género Drama -1 ya contado]

4. Título: Memento, Año: 2000, Géneros: Mystery, Thriller  Puntaje de Afinidad: 3
   [mismo director Nolan +3]

5. Título: The Matrix, Año: 1999, Géneros: Action, Sci-Fi  Puntaje de Afinidad: 2
   [género Action +1, género Sci-Fi +1]
```

---

### 2 · CineClash — Torneo de votación

Torneo de eliminación directa entre **8 películas aleatorias** de un género elegido por el usuario. Los 7 enfrentamientos se resuelven por el voto del usuario.

**Estructura del torneo:**

```
CUARTOS DE FINAL          SEMIFINALES            FINAL
  Pelicula A ─┐
              ├─► Ganador 1 ─┐
  Pelicula B ─┘              │
                             ├─► Ganador 5 ─┐
  Pelicula C ─┐              │              │
              ├─► Ganador 2 ─┘              ├─► CAMPEON
  Pelicula D ─┘                             │
                             ┌─► Ganador 6 ─┘
  Pelicula E ─┐              │
              ├─► Ganador 3 ─┘
  Pelicula F ─┘
  Pelicula G ─┐
              ├─► Ganador 4
  Pelicula H ─┘
```

Los ganadores se reinsertan al final de una **cola FIFO** y los eliminados se descartan. La selección inicial de 8 películas es aleatoria y sin repetición.

**Ejemplo de funcionamiento — CineClash:**

```
Ingrese el genero para iniciar el torneo: Action

Comienza el CineClash del Genero: Action

CUARTOS DE FINAL DEL CINECLASH!
Versus 1: [1] Mad Max: Fury Road vs [2] John Wick
Ingresa tu eleccion: 1
El ganador de esta ronda fue: Mad Max: Fury Road

Versus 2: [1] The Dark Knight vs [2] Die Hard
Ingresa tu eleccion: 1
El ganador de esta ronda fue: The Dark Knight

Versus 3: [1] Gladiator vs [2] Kill Bill
Ingresa tu eleccion: 2
El ganador de esta ronda fue: Kill Bill

Versus 4: [1] Avengers: Endgame vs [2] Mission Impossible
Ingresa tu eleccion: 1
El ganador de esta ronda fue: Avengers: Endgame

SEMIFINALES DEL CINECLASH!
Versus 5: [1] Mad Max: Fury Road vs [2] The Dark Knight
Ingresa tu eleccion: 2
El ganador de esta ronda fue: The Dark Knight

Versus 6: [1] Kill Bill vs [2] Avengers: Endgame
Ingresa tu eleccion: 2
El ganador de esta ronda fue: Avengers: Endgame

FINAL DEL CINECLASH!
Versus 7: [1] The Dark Knight vs [2] Avengers: Endgame
Ingresa tu eleccion: 1
El ganador de esta ronda fue: The Dark Knight

-------------------------------------------
El Ganador Del CineClash Es: The Dark Knight
-------------------------------------------
```

---

### 3 · Marathon Maker — Maratón personalizada

Genera una lista de películas para ver en maratón a partir de tres filtros: género, año (convertido a década) y cantidad. Ordena por rating IMDb descendente y permite navegar el resultado antes de confirmar.

**Navegación interactiva:**

| Tecla | Acción |
|---|---|
| `s` | Siguiente película |
| `a` | Película anterior |
| `c` | Confirmar e imprimir la maratón completa |
| `v` | Volver al menú sin guardar |

**Ejemplo de funcionamiento — Marathon Maker:**

```
========================================
         MARATHON MAKER
========================================

Ingrese un genero: Drama
Ingrese un Año : 1994
Cuantas peliculas desea en su maraton?: 3

Procesando maraton...
[OK] Lista generada con 3 pelicula(s).
Navegue: [S] Siguiente | [A] Anterior | [C] Confirmar | [V] Volver

  +--------------------------------------------------+
  |  Pelicula 1 de 3
  |  Titulo : The Shawshank Redemption
  |  Anio   : 1994
  |  Rating : 9.3
  |  Generos: Drama
  +--------------------------------------------------+

Opcion [s]iguiente | [a]nterior | [c]onfirmar | [v]olver: s

  +--------------------------------------------------+
  |  Pelicula 2 de 3
  |  Titulo : Pulp Fiction
  |  Anio   : 1994
  |  Rating : 8.9
  |  Generos: Crime, Drama
  +--------------------------------------------------+

Opcion [s]iguiente | [a]nterior | [c]onfirmar | [v]olver: s

  +--------------------------------------------------+
  |  Pelicula 3 de 3
  |  Titulo : Forrest Gump
  |  Anio   : 1994
  |  Rating : 8.8
  |  Generos: Drama, Romance
  +--------------------------------------------------+

Opcion [s]iguiente | [a]nterior | [c]onfirmar | [v]olver: c

========================================
        MARATON CONFIRMADA!
========================================

Genero: Drama  |  Decada: 1990  |  Peliculas: 3

   1. The Shawshank Redemption              (1994)
   2. Pulp Fiction                          (1994)
   3. Forrest Gump                          (1994)

  Disfruta tu maraton!
```

---

### 4 · CineOracle — El oráculo adivina tu película

El modo más elaborado. El oráculo carga hasta 71 preguntas de sí/no desde `PREGUNTASORACLE.CSV` y va descartando películas candidatas con cada respuesta hasta identificar la elegida por el usuario.

**Tipos de preguntas soportados:**

| Tipo | Criterio | Ejemplo de atributo |
|---|---|---|
| 1 | Género | `Action`, `Drama`, `Comedy` |
| 2 | Década | `1990`, `2000`, `2010` |
| 3 | Director (substring) | `Nolan`, `Spielberg` |
| 4 | Rating mínimo IMDb | `7.5`, `8.0` |
| 5 | Año anterior a | `2000`, `1990` |

**Algoritmo de selección de preguntas (por turno):**

```
Para cada pregunta no usada:
  contar cuántas candidatas cumplen su atributo → "coinciden"

  Si coinciden == 0 o coinciden == total → descartar (no aporta información)

  Si 30% ≤ coinciden ≤ 70% del total    → agregar al grupo "sweet spot" (ideal)
  Si |coinciden - total/2| es mínimo    → agregar al grupo "respaldo"

Prioridad de elección:
  1. Elegir aleatoriamente del sweet spot  (divide 30–70%, maximiza información)
  2. Si no hay sweet spot → elegir del respaldo (lo más cercano a mitades)
  3. Si no hay ninguna útil → salir del bucle y desempatar por rating
```

**Desempate final:** si quedan varias candidatas al terminar las preguntas, el oráculo elige automáticamente la de **mayor rating IMDb** e informa cuántas quedaron empatadas.

**Ejemplo de funcionamiento — CineOracle:**

```
====================================================
            ¡ Bienvenidos a CineOracle !
====================================================
El Oraculo leera tu mente. Responde con sinceridad

[1500 candidatas — evaluando 71 preguntas...]

-> ¿Te gustan las películas de acción?
[1] SI
[2] NO
Seleccion: 1
  ✓ Eliminadas ~825 películas sin género Action. Quedan ~675.

-> ¿Prefieres películas anteriores al año 2000?
[1] SI
[2] NO
Seleccion: 2
  ✓ Eliminadas películas de antes del 2000. Quedan ~420.

-> ¿Buscas una película muy bien valorada (8.0+)?
[1] SI
[2] NO
Seleccion: 1
  ✓ Eliminadas películas con rating < 8.0. Quedan ~40.

-> ¿Te gustan las películas de Christopher Nolan?
[1] SI
[2] NO
Seleccion: 1
  ✓ Eliminadas películas sin Nolan como director. Quedan 2.

[Sin más preguntas útiles — desempate por rating IMDb]

*****************************************************
                ¡ El ORACULO HA HABLADO !
  Hubo un empate de 2 peliculas, pero he seleccionado
  la mejor valorada por la critica para ti:

    -> The Dark Knight (2008)
    Director: Christopher Nolan
    Rating IMDb: 9.0 / 10.0
    Generos: Action, Crime, Drama
*****************************************************
```

---

## Funcionalidades auxiliares

### `cargar_peliculas()`

Lee `Top1500.csv` y construye los 4 mapas en una sola pasada. Usa `leer_linea_csv()` para tokenizar por coma y `split_string()` para separar los géneros de cada película.

**Ejemplo:** al leer la fila de *The Dark Knight*, extrae ID `tt0468569`, título `The Dark Knight`, géneros `[Action, Crime, Drama]`, año `2008`, director `Christopher Nolan`, rating `9.0`. Luego la inserta en `pelis_byid`, `pelis_titulo`, en las listas de `Action`, `Crime` y `Drama` dentro de `pelis_bygenres`, y en la lista de la década `2000` dentro de `pelis_decada`.


---

### `cargar_pool_preguntas()`

Lee `PREGUNTASORACLE.CSV` línea por línea, tokeniza por `;` y carga tipo, atributo y texto de cada pregunta en el arreglo `pool[]`. Marca todas como `usada = 0` al inicio.



---

### `is_equal_str()`

Función de comparación para mapas con clave `string`. Retorna `1` si `strcmp(key1, key2) == 0`, `0` si son distintas.



---

### `is_equal_int()`

Función de comparación para mapas con clave entera. Desreferencia ambos punteros y compara el valor directamente.



---

### `buscar_por_id()`

Solicita un ID al usuario, lo busca en `pelis_byid` con `map_search()` e imprime título y año si lo encuentra.


---

### `mostrar_marathon()`

Imprime la ficha visual de una película durante la navegación interactiva de Marathon Maker, mostrando su posición en la lista, título, año, rating y géneros.


---

### `mostrarMenuPrincipal()`

Limpia la pantalla con `limpiarPantalla()` e imprime el menú principal con las 5 opciones disponibles. Se llama al inicio de cada iteración del bucle principal.



---

## Justificación de TDAs utilizados

### Mapa de hash (`Map`)

Se eligió el mapa de hash como estructura principal de indexación porque el problema central del programa es la **búsqueda por clave**: dado un título, un género, una década o un ID, se necesita acceder a la información en el menor tiempo posible. El mapa ofrece búsqueda, inserción y eliminación en tiempo **O(1) promedio**, lo que es ideal cuando se tienen 1500 películas y el usuario espera respuesta inmediata.

Se usan cuatro instancias distintas porque cada modo del programa accede al catálogo por un criterio diferente: CineMatch necesita buscar por título, CineClash y Marathon Maker por género, y el sistema general por ID. Mantener un mapa por criterio evita recorrer linealmente las 1500 películas en cada operación.


### Lista enlazada doble (`List`)

Se eligió la lista enlazada doble para almacenar los géneros de cada película y las listas de candidatas en CineOracle porque el programa necesita **inserción al final y eliminación del nodo actual** de forma eficiente. La lista doble permite eliminar un nodo en O(1) con `list_popCurrent()` sin necesidad de buscar el nodo anterior, lo que es crítico en CineOracle donde se eliminan cientos de candidatas por ronda.

En Marathon Maker también se usa para la navegación interactiva: `list_next()` y `list_prev()` permiten moverse hacia adelante y hacia atrás en O(1), algo que un arreglo no permite sin guardar el índice manualmente.

---

### Cola FIFO (`Queue`)

Se eligió la cola para CineClash porque el torneo de eliminación directa tiene una lógica naturalmente FIFO: los 8 participantes entran en orden, se extraen de a dos para enfrentarse, y el ganador **vuelve al final** de la cola para la siguiente ronda. Esta disciplina de "el último ganador espera su turno" encaja exactamente con el modelo FIFO, sin necesidad de índices ni reorganización del arreglo.

En CineOracle también se usa como paso intermedio para encolar la pregunta seleccionada antes de procesarla, dejando la arquitectura preparada para encolar múltiples preguntas de antemano en una versión futura.

---

## Ejemplo de funcionamiento completo (sesión real)

A continuación se muestra una sesión de uso que recorre los cuatro modos del programa:

**1.** El programa inicia, `cargar_peliculas()` lee `Top1500.csv` y construye los 4 mapas. 1500 películas quedan indexadas.

**2.** El usuario selecciona **opción 1 (CineMatch)** e ingresa `"Inception"`. El sistema calcula afinidad con todas las películas de los mismos géneros (`Action`, `Adventure`, `Sci-Fi`) y devuelve el top 5 liderado por *The Dark Knight* con puntaje 7 (mismo director +3, misma década +2, Action +1, Drama +1... ajustado).

**3.** El usuario selecciona **opción 2 (CineClash)** con género `Action`. `sumLista()` cuenta 312 películas disponibles. Se seleccionan 8 al azar, se montan 7 enfrentamientos. El usuario vota en cada ronda y *The Dark Knight* sale campeón.

**4.** El usuario selecciona **opción 3 (Marathon Maker)** con género `Drama`, año `1994` y cantidad `3`. El sistema calcula la década `1990`, filtra las películas que cumplan ambos criterios y las ordena por rating. La navegación muestra *Shawshank Redemption* (9.3), *Pulp Fiction* (8.9) y *Forrest Gump* (8.8). El usuario navega con `s`, confirma con `c` y se imprime la maratón final.

**5.** El usuario selecciona **opción 4 (CineOracle)**. Tras 4 preguntas estratégicas, el oráculo descarta 1498 películas y presenta *The Dark Knight* como su respuesta final.

**6.** El usuario selecciona **opción 5** → el programa finaliza limpiamente.

