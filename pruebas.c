#include "pa2m.h"
#include "src/hash.h"
#include "src/hash_estructura_privada.h"
#include "src/lista.h"
#include <string.h>
#include <stdlib.h>

void crear_hash_con_capacidad_mayor_a_3()
{
	hash_t *hash = hash_crear(4);
	pa2m_afirmar(hash->tabla[0] && hash->tabla[1] && hash->tabla[2] &&
			     hash->tabla[3],
		     "Se puede crear un hash con capacidad mayor a 3.");
	hash_destruir(hash);
}

void crear_hash_con_capacidad_menor_a_3_se_crea_con_capacidad_3()
{
	hash_t *hash = hash_crear(1);
	pa2m_afirmar(hash->capacidad == 3 && hash->tabla[2],
		     "La capacidad mínima para crear un hash es 3.");
	hash_destruir(hash);
}

int posicion_correspondiente_a_clave(const char *clave, int capacidad)
{
	int i = 0, suma = 0;
	while (i < strlen(clave)) {
		suma = suma + clave[i];
		i++;
	}
	return suma % capacidad;
}

void insertar_sin_llegar_al_rehash_sin_colision_sin_clave_repetida()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a", *clave2 = "sc13";
	int valor1 = 1, valor2 = 2;
	int posicion1 = posicion_correspondiente_a_clave(clave1, 3);
	int posicion2 = posicion_correspondiente_a_clave(clave2, 3);
	hash_insertar(hash, clave1, &valor1, NULL);
	hash_insertar(hash, clave2, &valor2, NULL);
	par_cv_t *par1 = lista_primero(hash->tabla[posicion1]);
	par_cv_t *par2 = lista_primero(hash->tabla[posicion2]);
	char *clave_par1 = par1->clave, *clave_par2 = par2->clave;
	void *valor_par1 = par1->valor, *valor_par2 = par2->valor;
	pa2m_afirmar(
		strcmp((const char *)clave_par1, clave1) == 0 &&
			*(int *)valor_par1 == valor1 &&
			strcmp((const char *)clave_par2, clave2) == 0 &&
			*(int *)valor_par2 == valor2,
		"Puedo insertar sin rehash, colisiones ni claves repetidas.");
	hash_destruir(hash);
}

void insertar_sin_llegar_al_rehash_con_colision_sin_clave_repetida()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a", *clave2 = "fca3";
	int valor1 = 1, valor2 = 2;
	int posicion = posicion_correspondiente_a_clave(clave1, 3);
	hash_insertar(hash, clave1, &valor1, NULL);
	hash_insertar(hash, clave2, &valor2, NULL);
	par_cv_t *par1 = lista_primero(hash->tabla[posicion]);
	par_cv_t *par2 = lista_ultimo(hash->tabla[posicion]);
	char *clave_par1 = par1->clave, *clave_par2 = par2->clave;
	void *valor_par1 = par1->valor, *valor_par2 = par2->valor;
	pa2m_afirmar(
		strcmp((const char *)clave_par1, clave1) == 0 &&
			*(int *)valor_par1 == valor1 &&
			strcmp((const char *)clave_par2, clave2) == 0 &&
			*(int *)valor_par2 == valor2,
		"Puedo insertar con colisión, sin rehash y claves repetidas.");
	hash_destruir(hash);
}

void insertar_sin_llegar_al_rehash_con_clave_repetida_anterior_no_nulo()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a";
	int valor1 = 1, valor2 = 2;
	int posicion = posicion_correspondiente_a_clave(clave1, 3);
	void *anterior = &valor1;
	hash_insertar(hash, clave1, &valor1, NULL);
	hash_insertar(hash, clave1, &valor2, &anterior);
	par_cv_t *par1 = lista_primero(hash->tabla[posicion]);
	char *clave_par1 = par1->clave;
	void *valor_par1 = par1->valor;
	pa2m_afirmar(
		strcmp((const char *)clave_par1, clave1) == 0 &&
			*(int *)valor_par1 == valor2 &&
			*(int *)anterior == valor1,
		"Puedo insertar sin rehash, con clave repetida y anterior no nulo.");
	hash_destruir(hash);
}

void insertar_sin_llegar_al_rehash_con_clave_repetida_anterior_nulo()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a";
	int valor1 = 1, valor2 = 2;
	int posicion = posicion_correspondiente_a_clave(clave1, 3);
	void **anterior = NULL;
	hash_insertar(hash, clave1, &valor1, NULL);
	hash_insertar(hash, clave1, &valor2, anterior);
	par_cv_t *par1 = lista_primero(hash->tabla[posicion]);
	char *clave_par1 = par1->clave;
	void *valor_par1 = par1->valor;
	pa2m_afirmar(
		strcmp((const char *)clave_par1, clave1) == 0 &&
			*(int *)valor_par1 == valor2 && !anterior,
		"Puedo insertar sin rehash, con clave repetida y anterior nulo.");
	hash_destruir(hash);
}

void insertar_con_rehash()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "sc12", *clave2 = "sc13", *clave3 = "sc14",
		   *clave4 = "sc15";
	int valor1 = 1, valor2 = 2, valor3 = 3, valor4 = 4;

	hash_insertar(hash, clave1, &valor1, NULL);
	hash_insertar(hash, clave2, &valor2, NULL);
	hash_insertar(hash, clave3, &valor3, NULL);
	hash_insertar(hash, clave4, &valor4, NULL);

	int posicion1 = posicion_correspondiente_a_clave(clave1, 6);
	int posicion2 = posicion_correspondiente_a_clave(clave2, 6);
	int posicion3 = posicion_correspondiente_a_clave(clave3, 6);
	int posicion4 = posicion_correspondiente_a_clave(clave4, 6);

	par_cv_t *par1 = lista_primero(hash->tabla[posicion1]);
	par_cv_t *par2 = lista_primero(hash->tabla[posicion2]);
	par_cv_t *par3 = lista_primero(hash->tabla[posicion3]);
	par_cv_t *par4 = lista_primero(hash->tabla[posicion4]);

	pa2m_afirmar(hash->capacidad == 6 && hash->cantidad == 4 &&
			     strcmp((const char *)par1->clave, clave1) == 0 &&
			     strcmp((const char *)par2->clave, clave2) == 0 &&
			     strcmp((const char *)par3->clave, clave3) == 0 &&
			     strcmp((const char *)par4->clave, clave4) == 0 &&
			     *(int *)par1->valor == valor1 &&
			     *(int *)par2->valor == valor2 &&
			     *(int *)par3->valor == valor3 &&
			     *(int *)par4->valor == valor4,
		     "El rehash funciona correctamente.");

	hash_destruir(hash);
}

void insertar_pasando_hash_nulo()
{
	int valor = 1;
	pa2m_afirmar(!hash_insertar(NULL, "dwc3", &valor, NULL),
		     "No pasa nada si intento insertar en un hash nulo.");
}

void insertar_pasando_clave_nula()
{
	hash_t *hash = hash_crear(3);
	int valor = 1;
	pa2m_afirmar(!hash_insertar(hash, NULL, &valor, NULL),
		     "No pasa nada si intento insertar con clave nula.");
	hash_destruir(hash);
}

void quitar_elemento_que_se_encuentra_en_el_hash()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a", *clave2 = "sc13";
	int valor1 = 1, valor2 = 2;
	int posicion1 = posicion_correspondiente_a_clave(clave1, 3);
	hash_insertar(hash, clave1, &valor1, NULL);
	hash_insertar(hash, clave2, &valor2, NULL);
	void *elemento_quitado = hash_quitar(hash, clave1);
	pa2m_afirmar(*(int *)elemento_quitado == valor1 &&
			     lista_vacia(hash->tabla[posicion1]) &&
			     hash->cantidad == 1,
		     "Puedo quitar un elemento que se encuentra en el hash.");
	hash_destruir(hash);
}

void quitar_elemento_que_no_se_encuentra_en_el_hash()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a";
	int valor1 = 1;
	hash_insertar(hash, clave1, &valor1, NULL);
	pa2m_afirmar(
		!hash_quitar(hash, "fd1d") && hash->cantidad == 1,
		"No pasa nada si intento quitar un elemento que no está en el hash.");
	hash_destruir(hash);
}

void quitar_elemento_de_hash_nulo()
{
	pa2m_afirmar(
		!hash_quitar(NULL, "sdc2"),
		"No pasa nada si intento quitar un elemento de un hash nulo.");
}

void quitar_elemento_con_clave_nula()
{
	hash_t *hash = hash_crear(4);
	pa2m_afirmar(
		!hash_quitar(hash, NULL),
		"No pasa nada si intento quitar un elemento con clave nula.");
	hash_destruir(hash);
}

void obtener_elemento_que_esta_en_el_hash()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a";
	int valor1 = 1;
	hash_insertar(hash, clave1, &valor1, NULL);
	void *elemento_encontrado = hash_obtener(hash, clave1);
	pa2m_afirmar(*(int *)elemento_encontrado == valor1,
		     "Puedo buscar un elemento que se encuentra en el hash.");
	hash_destruir(hash);
}

void obtener_elemento_que_no_esta_en_el_hash()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a";
	int valor1 = 1;
	hash_insertar(hash, clave1, &valor1, NULL);
	pa2m_afirmar(!hash_obtener(hash, "sc1d"),
		     "No pasa nada si la clave no está en el hash.");
	hash_destruir(hash);
}

void obtener_elemento_en_hash_nulo()
{
	pa2m_afirmar(!hash_obtener(NULL, "sc1d"),
		     "No pasa nada si quiero buscar algo en un hash nulo.");
}

void obtener_elemento_con_clave_nula()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a";
	int valor1 = 1;
	hash_insertar(hash, clave1, &valor1, NULL);
	pa2m_afirmar(!hash_obtener(hash, NULL),
		     "No pasa nada si quiero buscar con una clave nula.");
	hash_destruir(hash);
}

void ver_si_contiene_elemento_que_esta_en_el_hash()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a";
	int valor1 = 1;
	hash_insertar(hash, clave1, &valor1, NULL);
	pa2m_afirmar(
		hash_contiene(hash, clave1),
		"Hash_contiene devuelve true si el elemento está en el hash.");
	hash_destruir(hash);
}

void ver_si_contiene_elemento_que_no_esta_en_el_hash()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a";
	int valor1 = 1;
	hash_insertar(hash, clave1, &valor1, NULL);
	pa2m_afirmar(
		!hash_contiene(hash, "sdca"),
		"Hash_contiene devuelve false si el elemento no está en el hash.");
	hash_destruir(hash);
}

void hash_contiene_con_hash_nulo()
{
	pa2m_afirmar(!hash_contiene(NULL, "sdca"),
		     "Hash_contiene devuelve false si el hash es nulo.");
}

void hash_contiene_con_clave_nula()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "fc3a";
	int valor1 = 1;
	hash_insertar(hash, clave1, &valor1, NULL);
	pa2m_afirmar(
		!hash_contiene(hash, NULL),
		"Hash_contiene devuelve false si el elemento no está en el hash.");
	hash_destruir(hash);
}

/**
 * Recibe una clave, un entero y una lista. Agrega el número a la lista si no es 0.
 * Si el elemento se agregó, se devuelve true. Caso contrario se devuelve false.
*/
bool agregar_numero_distinto_de_cero(const char *clave, void *numero,
				     void *lista)
{
	if (*(int *)numero != 0) {
		lista_insertar((lista_t *)lista, numero);
		return true;
	}
	return false;
}

void iterador_interno_pasando_hash_sin_colisiones()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "sc12", *clave2 = "sc13", *clave3 = "sc14",
		   *clave4 = "sc15";
	int valor1 = 1, valor2 = 2, valor3 = 3, valor4 = 0;

	hash_insertar(hash, clave1, &valor1, NULL);
	hash_insertar(hash, clave2, &valor2, NULL);
	hash_insertar(hash, clave3, &valor3, NULL);
	hash_insertar(hash, clave4, &valor4, NULL);

	lista_t *lista_numeros = lista_crear();

	size_t i = hash_con_cada_clave(hash, agregar_numero_distinto_de_cero,
				       lista_numeros);

	pa2m_afirmar(
		i == 4 && lista_tamanio(lista_numeros) == 3 &&
			*(int *)lista_elemento_en_posicion(lista_numeros, 0) ==
				valor1 &&
			*(int *)lista_elemento_en_posicion(lista_numeros, 1) ==
				valor2 &&
			*(int *)lista_elemento_en_posicion(lista_numeros, 2) ==
				valor3,
		"El iterador interno funciona correctamente en un hash sin colisiones.");

	lista_destruir(lista_numeros);

	hash_destruir(hash);
}

void iterador_interno_pasando_hash_con_colisiones()
{
	hash_t *hash = hash_crear(3);
	const char *clave1 = "sc12", *clave2 = "sc21", *clave3 = "sc14",
		   *clave4 = "sc15";
	int valor1 = 1, valor2 = 2, valor3 = 3, valor4 = 0;

	hash_insertar(hash, clave1, &valor1, NULL);
	hash_insertar(hash, clave2, &valor2, NULL);
	hash_insertar(hash, clave3, &valor3, NULL);
	hash_insertar(hash, clave4, &valor4, NULL);

	lista_t *lista_numeros = lista_crear();

	size_t i = hash_con_cada_clave(hash, agregar_numero_distinto_de_cero,
				       lista_numeros);

	pa2m_afirmar(
		i == 4 && lista_tamanio(lista_numeros) == 3 &&
			*(int *)lista_elemento_en_posicion(lista_numeros, 0) ==
				valor1 &&
			*(int *)lista_elemento_en_posicion(lista_numeros, 1) ==
				valor2 &&
			*(int *)lista_elemento_en_posicion(lista_numeros, 2) ==
				valor3,
		"El iterador interno funciona correctamente en un hash con colisiones.");

	lista_destruir(lista_numeros);

	hash_destruir(hash);
}

void iterador_interno_pasando_hash_nulo()
{
	lista_t *lista_numeros = lista_crear();
	size_t i = hash_con_cada_clave(NULL, agregar_numero_distinto_de_cero,
				       lista_numeros);
	pa2m_afirmar(
		i == 0 && lista_vacia(lista_numeros),
		"El iterador interno funciona correctamente en un hash nulo.");
	lista_destruir(lista_numeros);
}

void iterador_interno_pasando_funcion_nula()
{
	hash_t *hash = hash_crear(3);
	lista_t *lista_numeros = lista_crear();
	size_t i = hash_con_cada_clave(hash, NULL, lista_numeros);
	pa2m_afirmar(
		i == 0 && lista_vacia(lista_numeros),
		"El iterador interno funciona correctamente con una función nula.");
	lista_destruir(lista_numeros);
	hash_destruir(hash);
}

int main()
{
	pa2m_nuevo_grupo(
		"\n======================== CREAR ========================");
	crear_hash_con_capacidad_mayor_a_3();
	crear_hash_con_capacidad_menor_a_3_se_crea_con_capacidad_3();

	pa2m_nuevo_grupo(
		"\n======================== INSERTAR ========================");
	insertar_sin_llegar_al_rehash_sin_colision_sin_clave_repetida();
	insertar_sin_llegar_al_rehash_con_colision_sin_clave_repetida();
	insertar_sin_llegar_al_rehash_con_clave_repetida_anterior_no_nulo();
	insertar_sin_llegar_al_rehash_con_clave_repetida_anterior_nulo();
	insertar_con_rehash();
	insertar_pasando_hash_nulo();
	insertar_pasando_clave_nula();

	pa2m_nuevo_grupo(
		"\n======================== QUITAR ========================");
	quitar_elemento_que_se_encuentra_en_el_hash();
	quitar_elemento_que_no_se_encuentra_en_el_hash();
	quitar_elemento_de_hash_nulo();
	quitar_elemento_con_clave_nula();

	pa2m_nuevo_grupo(
		"\n======================== BUSCAR ========================");
	obtener_elemento_que_esta_en_el_hash();
	obtener_elemento_que_no_esta_en_el_hash();
	obtener_elemento_en_hash_nulo();
	obtener_elemento_con_clave_nula();

	pa2m_nuevo_grupo(
		"\n======================= CONTIENE =======================");
	ver_si_contiene_elemento_que_esta_en_el_hash();
	ver_si_contiene_elemento_que_no_esta_en_el_hash();
	hash_contiene_con_hash_nulo();
	hash_contiene_con_clave_nula();

	pa2m_nuevo_grupo(
		"\n======================= ITERADOR =======================");
	iterador_interno_pasando_hash_sin_colisiones();
	iterador_interno_pasando_hash_con_colisiones();
	iterador_interno_pasando_hash_nulo();
	iterador_interno_pasando_funcion_nula();

	return pa2m_mostrar_reporte();
}
