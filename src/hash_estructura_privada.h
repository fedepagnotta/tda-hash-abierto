#ifndef HASH_ESTRUCTURA_PRIVADA_H_
#define HASH_ESTRUCTURA_PRIVADA_H_

#include "hash.h"
#include "lista.h"

struct hash {
	lista_t **tabla;
	size_t capacidad;
	size_t cantidad;
};

typedef struct par_clave_valor {
	char *clave;
	void *valor;
} par_cv_t;

#endif // HASH_ESTRUCTURA_PRIVADA_H_