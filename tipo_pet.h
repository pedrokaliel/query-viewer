#ifndef TIPO_PET_H
#define TIPO_PET_H

#include "lista.h"
#include "arvore.h"
#include "comando.h"

typedef struct {
    int  codigo;
    char descricao[50];
} TipoPet;

void tipo_pet_executar(Comando *c, Lista *tipos, Lista *pets);

#endif
