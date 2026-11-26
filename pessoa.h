#ifndef PESSOA_H
#define PESSOA_H

#include "lista.h"
#include "arvore.h"
#include "comando.h"

typedef struct {
    int  codigo;
    char nome[50];
    char fone[20];
    char data_nascimento[11];     /* dd/mm/aaaa */
    char endereco[100];
} Pessoa;

void pessoa_executar(Comando *c, Lista *pessoas, Lista *pets);

#endif
