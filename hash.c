#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hash.h"

typedef struct nodo nodo_t;

struct nodo {
	char *llave;
	char *valor;
	struct nodo *siguiente;
};

struct hash {
	size_t tam;
	size_t cant_elementos;
	nodo_t **elementos_hash;
	hash_destruir_dato_t f_destruir;
};

const size_t TAMANIO_INICIAL = 1024;

#define USO_MAXIMO 70 /*Al usar 70% del hash, re-pedimos memoria*/

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

bool hash_guardar(hash_t *hash, const char *clave, void *dato)
{
	unsigned long pos = hash_string(clave);
	
	if( ((100*pos) / 1024) >= USO_MAXIMO )
	{
		// REDIMENSIONAR!!
	}
	
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
		hash->f_destruir(prox->valor);
		prox->valor = dato; /*actualizamos su dato*/
	}
	else /* No se encontró ninguno, armemos una nuevo dato! */
	{
		nodo_t* nuevo_dato;
		if( !(nuevo_dato = malloc(sizeof(nodo_t))) )
		{
			return false;
		}
		
		strcpy(nuevo_dato->llave, clave);
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
	}
	hash->cant_elementos++;
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave)
{
	unsigned long pos = hash_string(clave);
	
	nodo_t* cabeza_lista = hash->elementos_hash[pos];
	nodo_t* anterior = NULL;
	
	if( !cabeza_lista )
	{
		return NULL;
	}
		
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
			
			hash->f_destruir(cabeza_lista->valor);
			
			free(cabeza_lista);
			
			hash->cant_elementos--;
			
			return dato_eliminado;
		}
		anterior = cabeza_lista;
		cabeza_lista = cabeza_lista->siguiente;
	}
	/* No se encontró , devolvemos NULL */ 
	return NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave)
{
	unsigned long pos = hash_string(clave);
	nodo_t* inicio = hash->elementos_hash[pos];
	
	while( inicio != NULL )
	{
		if( !strcmp(inicio->llave, clave) )
		{
			return inicio->llave;
		}
		
		inicio = inicio->siguiente;
	}
	return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave)
{
	printf("To do it\n");
	return true;
}

size_t hash_cantidad(const hash_t *hash)
{
	return hash->cant_elementos;
}

void hash_destruir(hash_t *hash)
{
	nodo_t* tmp_vagon = NULL;
	
	for( size_t i = 0; i < hash->tam; i++ )
	{
		for( nodo_t* e = hash->elementos_hash[i]; e ; e = e->siguiente )
		{
			tmp_vagon = e;
			
			if(hash->f_destruir)
				hash->f_destruir(e->valor);
			else
				free(e->valor);
			
			free(e->llave);
			free(tmp_vagon);
		}
	}
	free(hash->elementos_hash);
	free(hash);
}
