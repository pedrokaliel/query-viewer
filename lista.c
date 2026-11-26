#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

void lista_inserir(Lista *l, void *dado)
{
    No *no = malloc(sizeof(No));
    no->dado = malloc(l->tam);
    memcpy(no->dado, dado, l->tam);

    no->prox = NULL;
    no->ant = l->fim;
    if (l->fim) l->fim->prox = no;
    else        l->ini = no;
    l->fim = no;
}

void lista_remover(Lista *l, No *no)
{
    if (no->ant) no->ant->prox = no->prox;
    else         l->ini = no->prox;
    if (no->prox) no->prox->ant = no->ant;
    else          l->fim = no->ant;
    free(no->dado);
    free(no);
}

No *lista_buscar(Lista *l, int codigo)
{
    for (No *no = l->ini; no; no = no->prox)
        if (*(int *)no->dado == codigo) return no;
    return NULL;
}

void lista_carregar(Lista *l, const char *arquivo, int tam)
{
    FILE *f = fopen(arquivo, "rb");
    void *registro = malloc(tam);

    l->ini = l->fim = NULL;
    l->tam = tam;
    if (f != NULL) {                       /* na 1a execucao o arquivo nao existe */
        while (fread(registro, tam, 1, f) == 1)
            lista_inserir(l, registro);
        fclose(f);
    }
    free(registro);
}

void lista_salvar(Lista *l, const char *arquivo)
{
    FILE *f = fopen(arquivo, "wb");
    if (f == NULL) {
        printf("Erro ao salvar %s\n", arquivo);
        return;
    }
    for (No *no = l->ini; no; no = no->prox)
        fwrite(no->dado, l->tam, 1, f);
    fclose(f);
}
