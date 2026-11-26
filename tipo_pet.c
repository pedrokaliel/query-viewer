#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipo_pet.h"
#include "pet.h"

/* Coloca o valor no campo. Devolve 0 se o campo nao existe. */
static int preencher(TipoPet *t, const char *campo, const char *valor)
{
    if      (strcmp(campo, "codigo") == 0)    t->codigo = atoi(valor);
    else if (strcmp(campo, "descricao") == 0 ||
             strcmp(campo, "nome") == 0)      strncpy(t->descricao, valor, sizeof(t->descricao) - 1);
    else return 0;
    return 1;
}

static const char *validar(TipoPet *t)
{
    if (t->codigo <= 0 || t->descricao[0] == '\0')
        return "campos obrigatorios: codigo, descricao";
    return NULL;
}

static void inserir(Comando *c, Lista *tipos)
{
    TipoPet t = {0};
    const char *erro;

    for (int i = 0; i < c->qtd; i++) {
        if (!preencher(&t, c->campos[i], c->valores[i])) {
            printf("  Erro: campo '%s' nao existe.\n", c->campos[i]);
            return;
        }
    }
    erro = validar(&t);
    if (erro == NULL && lista_buscar(tipos, t.codigo) != NULL)
        erro = "ja existe tipo com esse codigo";
    if (erro != NULL) {
        printf("  Erro: %s.\n", erro);
        return;
    }
    lista_inserir(tipos, &t);
    printf("  Tipo de pet inserido.\n");
}

static void alterar(Comando *c, Lista *tipos)
{
    No *no = lista_buscar(tipos, c->where_codigo);
    TipoPet novo;
    const char *erro;

    if (no == NULL) {
        printf("  Erro: tipo nao encontrado.\n");
        return;
    }
    novo = *(TipoPet *)no->dado;
    for (int i = 0; i < c->qtd; i++) {
        if (strcmp(c->campos[i], "codigo") == 0 || !preencher(&novo, c->campos[i], c->valores[i])) {
            printf("  Erro: campo '%s' invalido ou nao pode ser alterado.\n", c->campos[i]);
            return;
        }
    }
    erro = validar(&novo);
    if (erro != NULL) {
        printf("  Erro: %s.\n", erro);
        return;
    }
    *(TipoPet *)no->dado = novo;
    printf("  Tipo de pet alterado.\n");
}

/* Nao remove tipo que ainda e usado por algum pet. */
static void remover(Comando *c, Lista *tipos, Lista *pets)
{
    No *no = lista_buscar(tipos, c->where_codigo);
    if (no == NULL) {
        printf("  Erro: tipo nao encontrado.\n");
        return;
    }
    for (No *x = pets->ini; x; x = x->prox) {
        if (((Pet *)x->dado)->codigo_tipo == c->where_codigo) {
            printf("  Erro: existem pets deste tipo, nao foi removido.\n");
            return;
        }
    }
    lista_remover(tipos, no);
    printf("  Tipo de pet removido.\n");
}

static void mostrar(void *dado)
{
    TipoPet *t = dado;
    printf("  %d | %s\n", t->codigo, t->descricao);
}

static void selecionar(Comando *c, Lista *tipos)
{
    NoArvore *raiz = NULL;

    for (No *no = tipos->ini; no; no = no->prox) {
        TipoPet *t = no->dado;
        if (c->tem_where && t->codigo != c->where_codigo) continue;
        if (c->ordenar) raiz = arvore_inserir(raiz, t->descricao, t);   /* a arvore ordena */
        else            mostrar(t);
    }
    arvore_percorrer(raiz, mostrar);
    arvore_liberar(raiz);
}

void tipo_pet_executar(Comando *c, Lista *tipos, Lista *pets)
{
    if      (c->operacao == 'I') inserir(c, tipos);
    else if (c->operacao == 'U') alterar(c, tipos);
    else if (c->operacao == 'D') remover(c, tipos, pets);
    else                         selecionar(c, tipos);
}
