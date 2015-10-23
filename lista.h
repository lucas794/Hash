#ifndef LISTA_H
#define LISTA_H
#include <stdbool.h>
#include <stdlib.h>

struct lista;
typedef struct lista lista_t;
struct lista_iter;
typedef struct lista_iter lista_iter_t;

/*Primitivas básicas*/

//Crea una lista vacía o devuelve NULL si ocurrió un error
lista_t* lista_crear(void);
//pre:la lista esta creada
//post:Devuelve true si esta vacia o false si no
//tiempo de ejecucion: constante
bool lista_esta_vacia(const lista_t *lista);
//pre: la lista está vacia
//post:Agrega un elemento al comienzo de la lista y devuelve true
//o devuelve false si ocurrio un error
//Tiempo de ejecucion: constante
bool lista_insertar_primero(lista_t *lista, void *dato);
//pre:lista creada
//post: Agrega un elemento al final de la lista
//Tiempo de ejecucion: constante
bool lista_insertar_ultimo(lista_t *lista, void *dato);
//pre:lista creada
//post: se elimina el primer elemento de la lista y lo devuelve,
//o devuelve NULL si esta vacia
//Tiempo de ejecucion: constante
void* lista_borrar_primero(lista_t *lista);
//pre: lista creada
//post: devuelve el prumer elemento de la lista sin modificarla
//Tiempo de ejecucion: constante
void* lista_ver_primero(const lista_t *lista);
//pre: la lista fue creada
//post: devuelve el largo de la lista
//Tiempo de ejecucion: constante
size_t lista_largo(const lista_t *lista);
//pre:la lista fue creadal. Se deve pasar una funcion que sea capaz de destruir
//todos los datos de la lista
//post:La lista fue destruida y los datos eliminados
//Tiempo de ejecucion: O(n)
void lista_destruir(lista_t *lista, void destruir_dato(void*));

/*Primitivas de iteración*/

//Devuelve un iterador
//Tiempo de ejecucion: constante
lista_iter_t* lista_iter_crear(const lista_t *lista);
//pre: el iterador fue creado
//post: avanza el iterador una posicion y devuelve true, o false en caso de error
//Tiempo de ejecucion: constante
bool lista_iter_avanzar(lista_iter_t *iter);
//pre: el iterador fue creado
//post: devuelve el elemento actual de la lista donde se encuentra el iterador
//y lo devuelve sin modificar la lista, o devuelve NULL si la lista esta
//vacia o el iterador esta al final
//Tiempo de ejecucion: constante
void *lista_iter_ver_actual(const lista_iter_t *iter);
//pre: el iterador fue creado
//post: devuelve true si el iterador esta al final de la lista, o false si no
//Tiempo de ejecucion: constante
bool lista_iter_al_final(const lista_iter_t *iter);
//pre: el iterador fue creado
//post: destruye el iterador
//Tiempo de ejecucion: O(n)
void lista_iter_destruir(lista_iter_t *iter);

/*Primitivas de listas junto con iterador*/

//pre:la lista y el iterador fueron creados
//post: inserta un elemento en la posicion de la lista donde se encuentra el
//iterador
//Tiempo de ejecucion: constante
bool lista_insertar(lista_t *lista, lista_iter_t *iter, void *dato);

//pre:lista e iterador fueron creados
//post: se elimina el elemento de la lista donde se encuentra el iterador
//Tiempo de ejecucion: constante
void *lista_borrar(lista_t *lista, lista_iter_t *iter);

//Primitivas de iterador interno
//(la función de callback "visitar" recibe el dato y un puntero extra,
//y devuelve true si se debe seguir iterando, false en caso contrario):

void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra),void* extra);
//Función de pruebas:

void pruebas_lista_alumno(void);
#endif
