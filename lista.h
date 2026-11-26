#ifndef LISTA_H
#define LISTA_H

/* Lista duplamente encadeada generica: guarda uma copia de qualquer struct.
   Convencao: todas as structs (Pessoa, TipoPet, Pet) tem "int codigo" como primeiro campo. */

typedef struct No {
    void *dado;
    struct No *ant, *prox;
} No;

typedef struct {
    No *ini, *fim;
    int tam;                 /* tamanho em bytes de cada registro */
} Lista;

void lista_carregar(Lista *l, const char *arquivo, int tam);   /* monta a lista a partir do arquivo */
void lista_salvar(Lista *l, const char *arquivo);
void lista_inserir(Lista *l, void *dado);                      /* insere no fim */
void lista_remover(Lista *l, No *no);
No  *lista_buscar(Lista *l, int codigo);                       /* NULL se nao achar */

#endif
