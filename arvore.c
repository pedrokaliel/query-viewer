#include <stdlib.h>
#include <string.h>
#include "arvore.h"

NoArvore *arvore_inserir(NoArvore *raiz, const char *chave, void *dado)
{
    if (raiz == NULL) {
        raiz = malloc(sizeof(NoArvore));
        raiz->chave = chave;
        raiz->dado = dado;
        raiz->esq = raiz->dir = NULL;
    } else if (strcmp(chave, raiz->chave) < 0) {
        raiz->esq = arvore_inserir(raiz->esq, chave, dado);
    } else {
        raiz->dir = arvore_inserir(raiz->dir, chave, dado);
    }
    return raiz;
}

void arvore_percorrer(NoArvore *raiz, void (*mostrar)(void *))
{
    if (raiz == NULL) return;
    arvore_percorrer(raiz->esq, mostrar);
    mostrar(raiz->dado);
    arvore_percorrer(raiz->dir, mostrar);
}

void arvore_liberar(NoArvore *raiz)
{
    if (raiz == NULL) return;
    arvore_liberar(raiz->esq);
    arvore_liberar(raiz->dir);
    free(raiz);
}
