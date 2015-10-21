#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "hash.h"

typedef struct nodo nodo_t;

struct nodo {
	char *llave;
	void *valor;
	struct nodo *siguiente;
};

struct hash {
	size_t tam;
	size_t cant_elementos;
	nodo_t** elementos_hash;
	hash_destruir_dato_t f_destruir;
};

const size_t TAMANIO_INICIAL = 1024;

#define FACTOR_UTILIZACION_MEMORIA 4
#define FACTOR_MULTIPLICADOR 2
#define TOPE_FACTOR_CARGA 0.7

struct hash_iter{
	const hash_t* tabla_hash;
	size_t index;
	size_t iterados;
	nodo_t* elemento;
};

/* http://www.cse.yorku.ca/~oz/hash.html complete hash functions */
unsigned long hash_string(const char *str)
{
	unsigned int hash = 0;
	unsigned int c = (unsigned int) *str;
	while( *str != '\0' )
	{
	    hash += c;
		str++;
		c = (unsigned int) *str;
	}

	return hash;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato)
{
	hash_t *crear_hash;

	if( !(crear_hash = malloc( sizeof(*crear_hash) )) )
	{
		return NULL;
	}

	crear_hash->tam = TAMANIO_INICIAL;
	crear_hash->cant_elementos = 0;

	if( !(crear_hash->elementos_hash = malloc( crear_hash->tam * sizeof(nodo_t*) ) ) )
	{
		free(crear_hash);
		return NULL;
	}

	crear_hash->f_destruir = destruir_dato;

	for( size_t i = 0; i < crear_hash->tam; i++ )
		crear_hash->elementos_hash[i] = NULL;

	return crear_hash;
}


bool _funcion_guardar(hash_t *hash, const char* clave, void *dato){
   unsigned long pos = hash_string(clave);
   nodo_t* prox = hash->elementos_hash[pos];
   nodo_t* ultimo = NULL;

   /* El bucle aquí intenta buscar el dato aver si existe colision*/
   while( prox != NULL && prox->llave != NULL && strcmp(clave, prox->llave) )
   {
	   ultimo = prox;
	   prox = prox->siguiente;
   }

   /*Se encontro un par!!*/
   if( prox != NULL && prox->llave != NULL && !strcmp(clave, prox->llave) )
   {
	   if( hash->f_destruir )
		   hash->f_destruir(prox->valor);

	   prox->valor = dato; /*actualizamos su dato*/
   }
   else /* No se encontró ninguno, armemos una nuevo dato! */
   {
	   nodo_t* nuevo_dato;
	   if( !(nuevo_dato = malloc(sizeof(nodo_t))) )
		   return false;

	   if( (nuevo_dato->llave = strdup(clave)) == NULL )
	   {
		   free(nuevo_dato);
		   return false;
	   }
	   nuevo_dato->valor = dato;

	   nuevo_dato->siguiente = NULL;

	   /*Ahora, tenemos que verificar DONDE estamos parados
		* Para insertarlo */
	   if( prox == hash->elementos_hash[pos] ) /* ¿Al principio? */
	   {
		   nuevo_dato->siguiente = prox;
		   hash->elementos_hash[pos] = nuevo_dato;
	   }
	   else if( prox == NULL ) /* Estamos en el final Ó no existe tal posicion */
	   {
		   ultimo->siguiente = nuevo_dato;
	   }
	   else /* estamos en el medio de la 'lista' */
	   {
		   nuevo_dato->siguiente = prox;
		   ultimo->siguiente = nuevo_dato;
	   }
	   hash->cant_elementos++;
   }
   return true;
}

bool _hash_redimensionar(hash_t* hash, size_t tamanio_a_reasignar ){

   nodo_t** auxiliar = malloc(sizeof(nodo_t*) * tamanio_a_reasignar);
   if( !auxiliar ){
	   return false;
   }
   nodo_t** tabla_a_reemplazar= hash->elementos_hash;
   for( size_t i = 0; i < tamanio_a_reasignar; i++ ){
	   auxiliar[i] = NULL;
   }
   nodo_t* actual;
   hash->elementos_hash=auxiliar;
   hash->cant_elementos=0;
   for( size_t i = 0; i < hash->tam-1; i++ ){
  		if(tabla_a_reemplazar[i]){
			actual=tabla_a_reemplazar[i];
			while(actual){
				_funcion_guardar(hash,actual->llave,actual->valor);
				free(actual->llave);
				nodo_t* auxiliar = actual;
				actual=actual->siguiente;
				free(auxiliar);
			}
		}

   }
   free(tabla_a_reemplazar);
   hash->tam=tamanio_a_reasignar;
   return true;
}


bool hash_guardar(hash_t *hash, const char* clave, void *dato)
{
   unsigned long pos = hash_string(clave);
   float factor_de_carga = (float)hash->cant_elementos/(float)hash->tam;
   if(factor_de_carga>=TOPE_FACTOR_CARGA || pos>hash->tam){
	   size_t nuevo_tamanio = hash->tam*FACTOR_MULTIPLICADOR;
	   if(!_hash_redimensionar(hash,nuevo_tamanio)){
		   return false;
	   }
   }
   return _funcion_guardar(hash, clave, dato);
}


void *hash_borrar(hash_t *hash, const char *clave)
{
	unsigned long pos = hash_string(clave);

	nodo_t* cabeza_lista = hash->elementos_hash[pos];
	nodo_t* anterior = NULL;

	while( cabeza_lista != NULL ) /* debemos iterar para encontarlo*/
	{
		if( !strcmp(cabeza_lista->llave, clave) )
		{
			void* dato_eliminado = cabeza_lista->valor;

			if( anterior != NULL ) /* significa que iteró al menos 1 vez*/
			{
				anterior->siguiente = cabeza_lista->siguiente;
			}
			else
			{
				hash->elementos_hash[pos] = cabeza_lista->siguiente;
			}
			if( hash->f_destruir )
			{
				hash->f_destruir(cabeza_lista->valor);
			}
			free(cabeza_lista->llave);
			free(cabeza_lista);
			hash->cant_elementos--;
			return dato_eliminado;
		}
		anterior = cabeza_lista;
		cabeza_lista = cabeza_lista->siguiente;
	}

	if( (hash->tam - 1) < (hash->cant_elementos / FACTOR_UTILIZACION_MEMORIA) )
			if( !_hash_redimensionar(hash, hash->tam / FACTOR_MULTIPLICADOR) )
					return NULL;

	/* No se encontró , devolvemos NULL */
	return NULL;
}

void *hash_obtener(const hash_t *hash, const char* clave)
{
	unsigned long pos = hash_string(clave);
	nodo_t* inicio = hash->elementos_hash[pos];

	while( inicio != NULL )
	{
		if( !strcmp(inicio->llave, clave) )
		{
			return inicio->valor;
		}

		inicio = inicio->siguiente;
	}
	return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave)
{
	if(!hash_cantidad(hash)){
		return false;
	}

	/* acá hay que recorrer no solo la tabla hash
		 * sinó sus sub-nodos que pueden salir de cada index. */
	for(size_t posicion=0;posicion<hash->tam;posicion++)
		for( nodo_t* e = hash->elementos_hash[posicion]; e; e = e->siguiente )
			if( !strcmp(e->llave, clave) )
				return true;

	return false;
}

size_t hash_cantidad(const hash_t *hash)
{
	return hash->cant_elementos;
}

void hash_destruir(hash_t *hash)
{
	if(hash->cant_elementos==0){
		free(hash->elementos_hash);
		free(hash);
		return;
	}
	nodo_t* tmp_vagon = NULL;
	for( size_t i = 0; i < hash->tam; i++ )
	{
		for( nodo_t* e = hash->elementos_hash[i]; e ;  )
		{
			tmp_vagon = e;

			if(hash->f_destruir)
			{
				hash->f_destruir(tmp_vagon->valor);
			}
			e = e->siguiente;
			free(tmp_vagon->llave);
			free(tmp_vagon);
		}
	}
	free(hash->elementos_hash);
	free(hash);
}

/*Primitivas iterador*/

hash_iter_t* hash_iter_crear(const hash_t* hash){
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if(!iter){
		return NULL;
	}

	iter->index = 0;
	iter->tabla_hash = hash;
	iter->elemento = hash->elementos_hash[0];
	iter->iterados = 0;
	while( iter->elemento == NULL )
	{
		if( iter->index < iter->tabla_hash->tam-1)
		{
			iter->index++;
			iter->elemento = iter->tabla_hash->elementos_hash[iter->index];
		}
		else
			break;
	}
	return iter;
}

bool hash_iter_al_final(const hash_iter_t* iter){
	return (iter->iterados == iter->tabla_hash->cant_elementos);
}

bool hash_iter_avanzar(hash_iter_t* iter){
	if(hash_iter_al_final(iter)){
		return false;
	}
	if( iter->elemento->siguiente)
	{
		iter->elemento = iter->elemento->siguiente;
	}
	else
	{
		iter->index++;
		iter->elemento=iter->tabla_hash->elementos_hash[iter->index];
		while( iter->elemento == NULL && iter->index<iter->tabla_hash->tam-1)
		{
			iter->index++;
			iter->elemento = iter->tabla_hash->elementos_hash[iter->index];
		}

	}
	iter->iterados++;
	return true;
}

const char* hash_iter_ver_actual(const hash_iter_t* iter){
	if(hash_iter_al_final(iter)){
		return NULL;
	}
	return iter->elemento->llave;
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}
