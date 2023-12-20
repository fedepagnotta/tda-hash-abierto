#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lista.h"
#include "hash.h"
#include "hash_estructura_privada.h"

#define FACTOR_CARGA_MAXIMO 0.7
#define TAMANIO_HASH_MINIMO 3

/**
 * Recibe un puntero a hash con tabla NULL, y la inicializa creando una lista
 * enlazada vacía en cada espacio de la tabla. 
 * 
 * Devuelve el hash con la tabla inicializada.
*/
hash_t *inicializar_tabla(hash_t *hash)
{
	hash->tabla = calloc(hash->capacidad, sizeof(lista_t *));
	if (!hash->tabla)
		return NULL;
	for (int i = 0; i < hash->capacidad; i++) {
		hash->tabla[i] = lista_crear();
	}
	return hash;
}

/*
 * Crea el hash reservando la memoria necesaria para el.
 *
 * Capacidad indica la capacidad inicial con la que se crea el hash. La
 * capacidad inicial no puede ser menor a 3. Si se solicita una capacidad menor,
 * el hash se creará con una capacidad de 3.
 *
 * Devuelve un puntero al hash creado o NULL en caso de no poder crearlo.
 */
hash_t *hash_crear(size_t capacidad)
{
	if (capacidad < TAMANIO_HASH_MINIMO)
		capacidad = 3;
	hash_t *hash = malloc(sizeof(hash_t));
	if (!hash)
		return NULL;
	hash->cantidad = 0;
	hash->capacidad = capacidad;
	return inicializar_tabla(hash);
}

/**
 * Función hash.
 * 
 * Recibe una clave que es un string, y suma sus valores ascii.
 * 
 * Devuelve la suma.
*/
int funcion_hash(const char *clave)
{
	int i = 0, suma = 0;
	while (i < strlen(clave)) {
		suma = suma + clave[i];
		i++;
	}
	return suma;
}

/**
 * Recibe un par_cv_t pointer y una clave.
 * 
 * Compara la clave con la clave del par y devuelve un número mayor que cero
 * si la clave del par es mayor, o menor que cero si es menor, o cero si son
 * iguales.
*/
int comparador_claves(void *par, void *clave)
{
	const char *clave1 = ((par_cv_t *)par)->clave;
	return strcmp(clave1, clave);
}

/**
 * Recibe dos par_cv_t pointers y compara sus claves. 
 * 
 * Si son iguales se reemplaza el valor del primero por el del segundo 
 * y devuelve false. Si no son iguales devuelve true.
*/
bool cambiar_valor_de_clave_repetida(void *par1, void *par2)
{
	const char *clave = ((par_cv_t *)par2)->clave;
	void *valor = ((par_cv_t *)par2)->valor;
	if (comparador_claves(par1, (void *)clave) == 0) {
		((par_cv_t *)par1)->valor = valor;
		return false;
	}
	return true;
}

/**
 * Recibe un puntero a hash, un doble puntero a void (anterior), una clave
 * y una posición de la tabla hash. 
 * 
 * Si el anterior no es NULL, busca si hay algún elemento en la lista que
 * se encuentra en la posición pasada por parámetro de la tabla que tenga
 * la misma clave que la pasada por parámetro.
 * 
 * Si la hay, reemplaza lo que había en *anterior por el valor de ese par
 * encontrado. Caso contrario, *anterior pasa a ser NULL. 
*/
void actualizar_anterior(hash_t *hash, void **anterior, const char *clave,
			 int posicion)
{
	if (anterior) {
		par_cv_t *par_anterior =
			lista_buscar_elemento(hash->tabla[posicion],
					      comparador_claves, (void *)clave);
		if (par_anterior)
			*anterior = par_anterior->valor;
		else
			*anterior = NULL;
	}
}

/*
 * Inserta o actualiza un elemento en el hash asociado a la clave dada.
 *
 * Si la clave ya existía y se reemplaza el elemento, se almacena un puntero al
 * elemento reemplazado en *anterior, si anterior no es NULL.
 *
 * Si la clave no existía y anterior no es NULL, se almacena NULL en *anterior.
 *
 * La función almacena una copia de la clave provista por el usuario.
 *
 * Devuelve el hash si pudo guardar el elemento o NULL si no pudo.
 */
hash_t *insertar_sin_rehash(hash_t *hash, const char *clave, void *elemento,
			    void **anterior, bool hash_buscar_duplicado)
{
	par_cv_t *par = malloc(sizeof(par_cv_t));
	if (!par)
		return NULL;
	char *clave_copia = malloc((strlen(clave) + 1));
	if (!clave_copia) {
		free(par);
		return NULL;
	}
	strcpy(clave_copia, clave);
	par->clave = clave_copia;
	par->valor = elemento;
	int posicion = funcion_hash(clave) % (int)((hash_t *)hash)->capacidad;
	actualizar_anterior(hash, anterior, clave, posicion);
	if (hash_buscar_duplicado) {
		size_t pares_iterados = lista_con_cada_elemento(
			hash->tabla[posicion], cambiar_valor_de_clave_repetida,
			par);
		if (pares_iterados < lista_tamanio(hash->tabla[posicion])) {
			free(clave_copia);
			free(par);
			return hash;
		}
	}
	if (!lista_insertar(((hash_t *)hash)->tabla[posicion], par)) {
		free(clave_copia);
		free(par);
		return NULL;
	}
	hash->cantidad++;
	return hash;
}

/**
 * Recibe dos punteros a hash, donde el primero tiene el doble de capacidad
 * que el primero (y obviamente su tabla es el doble de grande).
 * 
 * Intercambia las tablas de cada hash, duplica la capacidad del primer
 * hash y demedia la del segundo, y por último destruye el segundo hash.
 * 
 * Es decir que los datos de ambos hash se intercambian, y el segundo se
 * libera.
*/
void hash_swap_y_liberar_uno(hash_t *hash, hash_t *hash_a_quitar)
{
	lista_t **tabla_aux = hash->tabla;
	hash->tabla = hash_a_quitar->tabla;
	hash_a_quitar->tabla = tabla_aux;
	hash->capacidad = hash->capacidad * 2;
	hash_a_quitar->capacidad = hash_a_quitar->capacidad / 2;
	hash_destruir(hash_a_quitar);
}

/**
 * Recibe una clave, un valor y un hash.
 * 
 * Inserta un par clave-valor con los pasados por parámetro en el hash.
 * 
 * Devuelve true si se pudo insertar, o false si no se pudo.
*/
bool hash_insertar_par(const char *clave, void *valor, void *hash)
{
	if (!insertar_sin_rehash(hash, clave, valor, NULL, false))
		return false;
	return true;
}

/**
 * Recibe un puntero a hash, duplica su capacidad y re-inserta sus elementos
 * con la nueva capacidad.
 * 
 * Devuelve cero si se pudo agrandar el hash, o -1 en caso de error (el hash
 * queda como se recibió).
*/
int rehash(hash_t *hash)
{
	hash_t *nuevo_hash = hash_crear(hash->capacidad * 2);
	if (!nuevo_hash)
		return -1;
	size_t pares_insertados =
		hash_con_cada_clave(hash, hash_insertar_par, nuevo_hash);
	if (pares_insertados < hash->cantidad) {
		hash_destruir(nuevo_hash);
		return -1;
	}
	hash_swap_y_liberar_uno(hash, nuevo_hash);
	return 0;
}

/*
 * Inserta o actualiza un elemento en el hash asociado a la clave dada.
 *
 * Si el factor de carga del hash (cantidad / capacidad) es mayor a 0.7,
 * duplica la capacidad del hash para evitar futuras colisiones.
 * 
 * Si la clave ya existía y se reemplaza el elemento, se almacena un puntero al
 * elemento reemplazado en *anterior, si anterior no es NULL.
 *
 * Si la clave no existía y anterior no es NULL, se almacena NULL en *anterior.
 *
 * La función almacena una copia de la clave provista por el usuario.
 *
 * Devuelve el hash si pudo guardar el elemento o NULL si no pudo.
 */
hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave)
		return NULL;
	float factor_de_carga = (float)hash->cantidad / (float)hash->capacidad;
	if (factor_de_carga > FACTOR_CARGA_MAXIMO)
		if (rehash(hash) == -1)
			return NULL;
	return insertar_sin_rehash(hash, clave, elemento, anterior, true);
}

/**
 * Recibe un par_cv_t pointer y una clave.
 * 
 * Compara la clave con la clave del par y devuelve true si son distintas, o
 * false si son iguales.
*/
bool encontrar_elemento_con_clave(void *par, void *clave)
{
	return (comparador_claves(par, clave) != 0);
}

/**
 * Recibe un hash, una posición de la tabla del hash, y una posición de la
 * lista a la que apunta la tabla en posicion_hash.
 * 
 * Quita el elemento que se encuentra en posicion_lista, de la lista que se
 * encuentra en posición_hash. Libera el par y la clave quitados, y devuelve
 * el valor del par.
*/
void *quitar_elemento(hash_t *hash, size_t posicion_lista, int posicion_hash)
{
	par_cv_t *par_quitado = lista_quitar_de_posicion(
		hash->tabla[posicion_hash], posicion_lista);
	void *elemento_quitado = par_quitado->valor;
	free(par_quitado->clave);
	free(par_quitado);
	hash->cantidad--;
	return elemento_quitado;
}

/*
 * Quita un elemento del hash y lo devuelve.
 *
 * Si no encuentra el elemento o en caso de error devuelve NULL
 */
void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;
	int posicion = funcion_hash(clave) % (int)((hash_t *)hash)->capacidad;
	size_t posicion_a_quitar = lista_con_cada_elemento(
		hash->tabla[posicion], encontrar_elemento_con_clave,
		(void *)clave);
	if (posicion_a_quitar < lista_tamanio(hash->tabla[posicion])) {
		return quitar_elemento(hash, posicion_a_quitar, posicion);
	}
	return NULL;
}

/*
 * Devuelve un elemento del hash con la clave dada o NULL si dicho
 * elemento no existe (o en caso de error).
 */
void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;
	int posicion = funcion_hash(clave) % (int)((hash_t *)hash)->capacidad;
	par_cv_t *par_encontrado = lista_buscar_elemento(
		hash->tabla[posicion], comparador_claves, (void *)clave);
	if (par_encontrado)
		return par_encontrado->valor;
	return NULL;
}

/*
 * Devuelve true si el hash contiene un elemento almacenado con la
 * clave dada o false en caso contrario (o en caso de error).
 */
bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return false;
	int posicion = funcion_hash(clave) % (int)((hash_t *)hash)->capacidad;
	par_cv_t *par_encontrado = lista_buscar_elemento(
		hash->tabla[posicion], comparador_claves, (void *)clave);
	return par_encontrado != NULL;
}

/*
 * Devuelve la cantidad de elementos almacenados en el hash o 0 en
 * caso de error.
 */
size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;
	return hash->cantidad;
}

typedef struct estructura_auxiliar_para_destructor {
	void (*destructor)(void *);
} destructor_t;

/**
 * Recibe una clave, un valor y un puntero a destructor_t que contiene una
 * función destructora. Si la función no es NULL la invoca pasándole el valor
 * por parámetro, y luego libera la clave.
 *
 * Devuelve true.
*/
bool destruir_todo(const char *clave, void *valor, void *destructor_aux)
{
	void (*destructor)(void *) =
		((destructor_t *)destructor_aux)->destructor;
	if (destructor) {
		destructor(valor);
	}
	free((void *)clave);
	return true;
}

/*
 * Destruye el hash liberando la memoria reservada.
 */
void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

/*
 * Destruye el hash liberando la memoria reservada y asegurandose de
 * invocar la funcion destructora con cada elemento almacenado en el
 * hash.
 */
void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;
	destructor_t destructor_aux = { .destructor = destructor };
	hash_con_cada_clave(hash, destruir_todo, (void *)&destructor_aux);
	for (int i = 0; i < hash->capacidad; i++) {
		lista_destruir_todo(hash->tabla[i], free);
	}
	free(hash->tabla);
	free(hash);
}

typedef struct estructura_auxiliar_para_iterador {
	bool (*f)(const char *, void *, void *);
	void *aux;
} aux_iterador_t;

/**
 * Recibe un par_cv_t pointer y un puntero a aux_iterador_t, que contiene
 * una función f que devuelve un booleano y un void pointer auxiliar.
 * 
 * Se invoca a la función f pasándole como parámetro la clave y el valor
 * del par, y el puntero auxiliar. Se devuelve el valor de retorno de f.
*/
bool llamar_funcion_con_clave_y_valor(void *par, void *f_y_aux)
{
	void *aux = ((aux_iterador_t *)f_y_aux)->aux;
	bool (*f)(const char *, void *, void *) =
		((aux_iterador_t *)f_y_aux)->f;
	const char *clave = ((par_cv_t *)par)->clave;
	void *valor = ((par_cv_t *)par)->valor;
	return f(clave, valor, aux);
}

/*
 * Recorre cada una de las claves almacenadas en la tabla de hash e invoca a la
 * función f, pasandole como parámetros la clave, el valor asociado a la clave
 * y el puntero auxiliar.
 *
 * Mientras que queden mas claves o la funcion retorne true, la
 * iteración continúa. Cuando no quedan mas claves o la función
 * devuelve false, la iteración se corta y la función principal
 * retorna.
 *
 * Devuelve la cantidad de claves totales iteradas (la cantidad de
 * veces que fue invocada la función) o 0 en caso de error.
 */
size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
	size_t resultado = 0, pares_iterados = 0;
	if (!hash || !f)
		return resultado;
	aux_iterador_t f_y_aux = { .f = f, .aux = aux };
	for (int i = 0; i < hash->capacidad; i++) {
		pares_iterados = lista_con_cada_elemento(
			hash->tabla[i], llamar_funcion_con_clave_y_valor,
			(void *)&f_y_aux);
		resultado = resultado + pares_iterados;
		if (pares_iterados < lista_tamanio(hash->tabla[i])) {
			resultado++;
			return resultado;
		}
	}
	return resultado;
}
