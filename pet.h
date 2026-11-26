#ifndef PET_H
#define PET_H

#include "lista.h"
#include "arvore.h"
#include "comando.h"

typedef struct {
    int  codigo;
    int  codigo_pes;      /* dono */
    char nome[50];
    int  codigo_tipo;     /* tipo do pet */
} Pet;

void pet_executar(Comando *c, Lista *pets, Lista *pessoas, Lista *tipos);

#endif
