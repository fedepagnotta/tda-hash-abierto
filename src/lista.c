#include "lista.h"
#include <stdlib.h>

typedef struct nodo {
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

struct lista {
	nodo_t *nodo_inicio;
	nodo_t *nodo_ultimo;
	size_t tamanio;
};

struct lista_iterador {
	nodo_t *corriente;
};

/**
 * Recibe un void pointer. 
 * Crea y devuelve un struct nodo que almacena el elemento al que apunta 
 * el puntero, y el siguiente del nodo es NULL.
*/
nodo_t *nodo_crear(void *elemento)
{
	nodo_t *nodo = malloc(sizeof(nodo_t));
	nodo->elemento = elemento;
	nodo->siguiente = NULL;
	return nodo;
}

/**
 * Crea la lista reservando la memoria necesaria.
 * Devuelve un puntero a la lista creada o NULL en caso de error.
 */
lista_t *lista_crear()
{
	return calloc(1, sizeof(lista_t));
}

/**
 * Inserta un elemento al final de la lista.
 *
 * Devuelve NULL si no pudo insertar el elemento a causa de un error, o la lista en caso de exito.
 */
lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (!lista)
		return NULL;
	nodo_t *nodo_nuevo = nodo_crear(elemento);
	if (!nodo_nuevo)
		return NULL;
	if (!lista->nodo_inicio) {
		lista->nodo_inicio = nodo_nuevo;
		lista->nodo_ultimo = nodo_nuevo;
	} else {
		lista->nodo_ultimo->siguiente = nodo_nuevo;
		lista->nodo_ultimo = nodo_nuevo;
	}
	lista->tamanio++;
	return lista;
}

/**
 * Recibe un puntero a lista_t y uno a nodo_t. 
 * Inserta el nodo al principio de la lista.
*/
void insertar_al_principio(lista_t *lista, nodo_t *nodo_nuevo)
{
	if (lista->tamanio == 0) {
		lista->nodo_ultimo = nodo_nuevo;
	}
	nodo_nuevo->siguiente = lista->nodo_inicio;
	lista->nodo_inicio = nodo_nuevo;
}

/**
 * Inserta un elemento en la posicion indicada, donde 0 es insertar
 * como primer elemento y 1 es insertar luego del primer elemento.  
 * En caso de no existir la posicion indicada, lo inserta al final.
 *
 * Devuelve NULL si no pudo insertar el elemento a causa de un error, 
 * o la lista en caso de exito.
 */
lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{
	if (!lista || posicion >= lista->tamanio)
		return lista_insertar(lista, elemento);
	nodo_t *nodo_nuevo = nodo_crear(elemento);
	if (!nodo_nuevo)
		return NULL;
	if (posicion == 0) {
		insertar_al_principio(lista, nodo_nuevo);
	} else {
		nodo_t *nodo_actual = lista->nodo_inicio;
		for (int i = 0; i < posicion - 1; i++)
			nodo_actual = nodo_actual->siguiente;
		nodo_nuevo->siguiente = nodo_actual->siguiente;
		nodo_actual->siguiente = nodo_nuevo;
	}
	lista->tamanio++;
	return lista;
}

/**
 * Recibe una lista con un nodo y lo quita, dejando la lista vacía.
*/
void *quitar_unico_elemento(lista_t *lista)
{
	lista->nodo_ultimo = NULL;
	void *elemento = lista->nodo_inicio->elemento;
	free(lista->nodo_inicio);
	lista->nodo_inicio = NULL;
	return elemento;
}

/**
 * Quita de la lista el elemento que se encuentra en la ultima posición.
 *
 * Devuelve el elemento removido de la lista o NULL en caso de error.
 */
void *lista_quitar(lista_t *lista)
{
	if (!lista || lista->tamanio == 0)
		return NULL;
	void *elemento;
	if (lista->tamanio == 1) {
		elemento = quitar_unico_elemento(lista);
	} else {
		nodo_t *nodo_actual = lista->nodo_inicio;
		while (nodo_actual->siguiente->siguiente)
			nodo_actual = nodo_actual->siguiente;
		nodo_t *nodo_quitar = lista->nodo_ultimo;
		elemento = nodo_quitar->elemento;
		lista->nodo_ultimo = nodo_actual;
		free(nodo_quitar);
	}
	lista->tamanio--;
	return elemento;
}

/**
 * Quita de la lista el elemento que se encuentra en la posición
 * indicada, donde 0 es el primer elemento de la lista.
 *
 * En caso de no existir esa posición se intentará borrar el último
 * elemento.
 *
 * Devuelve el elemento removido de la lista o NULL en caso de error.
 */
void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	void *elemento;
	nodo_t *nodo_a_quitar;
	if (!lista || lista->tamanio == 0 || posicion >= lista->tamanio)
		return lista_quitar(lista);
	if (posicion == 0) {
		nodo_a_quitar = lista->nodo_inicio;
		elemento = nodo_a_quitar->elemento;
		lista->nodo_inicio = nodo_a_quitar->siguiente;
	} else {
		nodo_t *nodo_actual = lista->nodo_inicio;
		for (int i = 0; i < posicion - 1; i++)
			nodo_actual = nodo_actual->siguiente;
		nodo_a_quitar = nodo_actual->siguiente;
		elemento = nodo_a_quitar->elemento;
		nodo_actual->siguiente = nodo_a_quitar->siguiente;
	}
	free(nodo_a_quitar);
	lista->tamanio--;
	return elemento;
}

/**
 * Devuelve el elemento en la posicion indicada, donde 0 es el primer
 * elemento.
 *
 * Si no existe dicha posicion devuelve NULL.
 */
void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || posicion >= lista->tamanio)
		return NULL;
	nodo_t *nodo_actual = lista->nodo_inicio;
	for (int i = 0; i < posicion; i++)
		nodo_actual = nodo_actual->siguiente;
	return nodo_actual->elemento;
}

/**
 * Devuelve el primer elemento de la lista que cumple la condición
 * comparador(elemento, contexto) == 0.
 *
 * Si no existe el elemento devuelve NULL.
 */
void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (!lista || lista->tamanio == 0 || !comparador)
		return NULL;
	nodo_t *nodo_actual = lista->nodo_inicio;
	while (nodo_actual) {
		if (comparador(nodo_actual->elemento, contexto) == 0) {
			return nodo_actual->elemento;
		}
		nodo_actual = nodo_actual->siguiente;
	}
	return NULL;
}

/**
 * Devuelve el primer elemento de la lista o NULL si la lista se
 * encuentra vacía o no existe.
 */
void *lista_primero(lista_t *lista)
{
	if (!lista || lista->tamanio == 0)
		return NULL;
	return lista->nodo_inicio->elemento;
}

/**
 * Devuelve el último elemento de la lista o NULL si la lista se
 * encuentra vacía o no existe.
 */
void *lista_ultimo(lista_t *lista)
{
	if (!lista || lista->tamanio == 0)
		return NULL;
	return lista->nodo_ultimo->elemento;
}

/**
 * Devuelve true si la lista está vacía (o no existe) 
 * o false en caso contrario.
 */
bool lista_vacia(lista_t *lista)
{
	return (lista == NULL || lista->tamanio == 0);
}

/**
 * Devuelve la cantidad de elementos almacenados en la lista.
 * Una lista que no existe no puede tener elementos.
 */
size_t lista_tamanio(lista_t *lista)
{
	if (lista == NULL)
		return 0;
	return (size_t)lista->tamanio;
}

/**
 * Libera la memoria reservada por la lista.
 */
void lista_destruir(lista_t *lista)
{
	lista_destruir_todo(lista, NULL);
}

/**
 * Libera la memoria reservada por la lista pero además aplica la función
 * destructora dada (si no es NULL) a cada uno de los elementos presentes en la
 * lista.
 */
void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	while (lista && lista->nodo_inicio) {
		if (funcion) {
			funcion(lista->nodo_inicio->elemento);
		}
		lista_quitar_de_posicion(lista, 0);
	}
	free(lista);
}

/**
 * Crea un iterador para una lista. El iterador creado es válido desde
 * el momento de su creación hasta que no haya mas elementos por
 * recorrer o se modifique la lista iterada (agregando o quitando
 * elementos de la lista). 
 *
 * Al momento de la creación, el iterador queda listo para devolver el
 * primer elemento utilizando lista_iterador_elemento_actual.
 *
 * Devuelve el puntero al iterador creado o NULL en caso de error.
 */
lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista)
		return NULL;
	lista_iterador_t *iterador = malloc(sizeof(lista_iterador_t));
	iterador->corriente = lista->nodo_inicio;
	return iterador;
}

/**
 * Devuelve true si hay mas elementos sobre los cuales iterar o false
 * si no hay mas.
 */
bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->corriente)
		return false;
	return true;
}

/**
 * Avanza el iterador al siguiente elemento.
 * Devuelve true si pudo avanzar el iterador o false en caso de
 * que no queden elementos o en caso de error.
 *
 * Una vez llegado al último elemento, si se invoca a
 * lista_iterador_elemento_actual, el resultado siempre será NULL.
 */
bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (lista_iterador_tiene_siguiente(iterador) == false)
		return false;
	iterador->corriente = iterador->corriente->siguiente;
	return lista_iterador_tiene_siguiente(iterador);
}

/**
 * Devuelve el elemento actual del iterador o NULL en caso de que no
 * exista dicho elemento o en caso de error.
 */
void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->corriente)
		return NULL;
	return iterador->corriente->elemento;
}

/**
 * Libera la memoria reservada por el iterador.
 */
void lista_iterador_destruir(lista_iterador_t *iterador)
{
	free(iterador);
}

/**
 * Iterador interno. Recorre la lista e invoca la funcion con cada elemento de
 * la misma como primer parámetro. Dicha función puede devolver true si se 
 * deben seguir recorriendo elementos o false si se debe dejar de 
 * iterar elementos.
 *
 * El puntero contexto se pasa como segundo argumento a la función del usuario.
 *
 * La función devuelve la cantidad de elementos iterados o 0 en caso de error
 * (errores de memoria, función o lista NULL, etc).
 *
 */
size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if (lista_vacia(lista) || !funcion)
		return 0;
	size_t contador = 0;
	nodo_t *nodo_iterador = lista->nodo_inicio;
	while (nodo_iterador && funcion(nodo_iterador->elemento, contexto)) {
		nodo_iterador = nodo_iterador->siguiente;
		contador++;
	}
	return contador;
}
