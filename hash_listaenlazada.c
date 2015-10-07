/* hash usando lista enlazada previamente subido acá */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hash.h"
#include "lista.h"

typedef struct nodo nodo_t;

struct nodo {
	char *clave;
	void *valor;
};

struct hash {
	size_t tam;
	size_t cant_elementos;
	lista_t **tabla_hash;
	hash_destruir_dato_t f_destruir;
};

const size_t TAMANIO_INICIAL = 1024;

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
	
	if( !(crear_hash->tabla_hash = malloc( crear_hash->tam * sizeof(nodo_t*) ) ) )
	{
		free(crear_hash);
		return NULL;
	}

	crear_hash->f_destruir = destruir_dato;

	for( size_t i = 0; i < crear_hash->tam; i++ )
		crear_hash->tabla_hash[i] = NULL;

	return crear_hash;
}

/* f aux */
nodo_t* crear_nodo(const char *clave, void *dato)
{
	nodo_t* vagon = malloc( sizeof(nodo_t) );
		
	if( !vagon )
		return NULL;
	
	strcpy(vagon->clave, clave);
	vagon->valor = dato;

	return vagon;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato)
{
	unsigned long pos = hash_string(clave);

	// algoritmo redirección.

	nodo_t* vagon = crear_nodo( clave, dato );
		
	if( !vagon )
			return false;	

	if( !hash->tabla_hash[pos] ) /* no hay ningun mismatch */
	{
		hash->tabla_hash[pos] = lista_crear( );

		if( !hash->tabla_hash[pos] )
			return false;
			
		lista_insertar_ultimo(hash->tabla_hash[pos], vagon);
		return true;
	}
	else
	{
		/* hay un mismatch, hay que iniciar a iterar */
		/* caso especial, el primero es el que reemplazamos ? */

		nodo_t* primero = lista_ver_primero(hash->tabla_hash[pos]);

		if( !strcmp(primero->clave, clave) )
		{
			lista_borrar_primero(hash->tabla_hash[pos]);
			lista_insertar_primero(hash->tabla_hash[pos], vagon);
			return true;
		}
		else
		{
			/* a iterar hasta encontrar algun mismatch? */
			lista_iter_t *iterador = lista_iter_crear(hash->tabla_hash[pos]);
			if( !iterador) return false;
			lista_iter_avanzar(iterador); /* el primero no es, asique avanzamos */	

			while( !lista_iter_al_final(iterador) )
			{
				nodo_t* act = lista_iter_ver_actual(iterador);

				if( !strcmp(act->clave, clave) )
				{
					/* AQUÍ LO TENEMOS! */					
					lista_borrar(hash->tabla_hash[pos], iterador);
					lista_insertar(hash->tabla_hash[pos], iterador, vagon);
					lista_iter_destruir(iterador);
					return true;
				}
				lista_iter_avanzar(iterador);
			}
			/* llegados al final, no hay coincidencias, lo agregamos */
			lista_insertar(hash->tabla_hash[pos], iterador, vagon);
			lista_iter_destruir(iterador);
		}
	}
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave)
{
	unsigned long pos = hash_string(clave);

	if( !hash->tabla_hash[pos] )
		return NULL;

	// Algoritmo redireccionamiento (achicar)
	
	void *dato_return;

	if( lista_largo(hash->tabla_hash[pos]) == 1 )
	{
		/* solo 1 elemento, borramos la lista */
		dato_return = lista_ver_primero(hash->tabla_hash[pos]);
		lista_destruir(hash->tabla_hash[pos], NULL);
		return dato_return;
	}

	lista_iter_t *iterador = lista_iter_crear(hash->tabla_hash[pos]);
	if( !iterador ) return NULL;

	while( !lista_iter_al_final(iterador) )
	{
		nodo_t* act = lista_iter_ver_actual(iterador);
		dato_return = act->valor;
	
		if( !strcmp(act->clave, clave) )
		{
			/* AQUÍ LO TENEMOS! */					
			lista_borrar(hash->tabla_hash[pos], iterador);
			lista_iter_destruir(iterador);
			return dato_return;
		}
		lista_iter_avanzar(iterador);
	}
	/* llegó al final y no encontró nada */
	lista_iter_destruir(iterador);
	return NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave)
{
	unsigned long pos = hash_string(clave);

	if( !hash->tabla_hash[pos] )
		return NULL;

	lista_iter_t *iterador = lista_iter_crear(hash->tabla_hash[pos]);
	if( !iterador ) return NULL;

	while( !lista_iter_al_final(iterador) )
	{
		nodo_t* act = lista_iter_ver_actual(iterador);
	
		if( !strcmp(act->clave, clave) )
		{
			/* AQUÍ LO TENEMOS! */
			lista_iter_destruir(iterador);			
			return act->valor;
		}
		lista_iter_avanzar(iterador);
	}
	lista_iter_destruir(iterador);
	return NULL;
}

size_t hash_cantidad(const hash_t *hash)
{
	return hash->cant_elementos;
}

void hash_destruir(hash_t *hash)
{
	for( size_t i = 0; i < hash->tam; i++ )
	{
		if( !hash->tabla_hash[i] )
			continue;

		lista_iter_t *iterador = lista_iter_crear(hash->tabla_hash[i]);

		if( !iterador ) 
		{
			i = i - 1; // Necesitamos que lo haga de nuevo si o si!!
			continue;
		}

		while( !lista_iter_al_final(iterador) )
		{
			nodo_t* tmp = lista_iter_ver_actual(iterador);
			
			hash->f_destruir ? hash->f_destruir(tmp->clave) : free(tmp->clave);
			hash->f_destruir ? hash->f_destruir(tmp->valor) : free(tmp->valor);
			/* algo destruido */

			lista_iter_avanzar(iterador);
		}
		lista_iter_destruir(iterador);
		lista_destruir(hash->tabla_hash[i], NULL);
	}
	free(hash->tabla_hash);
	free(hash);
}
