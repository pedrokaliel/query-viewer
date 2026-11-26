#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pet.h"

/* Coloca o valor no campo. Devolve 0 se o campo nao existe. */
static int preencher(Pet *p, const char *campo, const char *valor)
{
    if      (strcmp(campo, "codigo") == 0)      p->codigo = atoi(valor);
    else if (strcmp(campo, "codigo_pes") == 0 ||
             strcmp(campo, "codigo_cli") == 0)  p->codigo_pes = atoi(valor);
    else if (strcmp(campo, "codigo_tipo") == 0) p->codigo_tipo = atoi(valor);
    else if (strcmp(campo, "nome") == 0)        strncpy(p->nome, valor, sizeof(p->nome) - 1);
    else return 0;
    return 1;
}

/* Devolve a mensagem de erro, ou NULL se o pet esta ok. */
static const char *validar(Pet *p, Lista *pessoas, Lista *tipos)
{
    if (p->codigo <= 0 || p->codigo_pes <= 0 || p->codigo_tipo <= 0 || p->nome[0] == '\0')
        return "campos obrigatorios: codigo, codigo_pes, nome, codigo_tipo";
    if (lista_buscar(pessoas, p->codigo_pes) == NULL)
        return "codigo_pes nao e de uma pessoa cadastrada";
    if (lista_buscar(tipos, p->codigo_tipo) == NULL)
        return "codigo_tipo nao e de um tipo de pet cadastrado";
    return NULL;
}

static void inserir(Comando *c, Lista *pets, Lista *pessoas, Lista *tipos)
{
    Pet p = {0};
    const char *erro;

    for (int i = 0; i < c->qtd; i++) {
        if (!preencher(&p, c->campos[i], c->valores[i])) {
            printf("  Erro: campo '%s' nao existe.\n", c->campos[i]);
            return;
        }
    }
    erro = validar(&p, pessoas, tipos);
    if (erro == NULL && lista_buscar(pets, p.codigo) != NULL)
        erro = "ja existe pet com esse codigo";
    if (erro != NULL) {
        printf("  Erro: %s.\n", erro);
        return;
    }
    lista_inserir(pets, &p);
    printf("  Pet inserido.\n");
}

static void alterar(Comando *c, Lista *pets, Lista *pessoas, Lista *tipos)
{
    No *no = lista_buscar(pets, c->where_codigo);
    Pet novo;
    const char *erro;

    if (no == NULL) {
        printf("  Erro: pet nao encontrado.\n");
        return;
    }
    novo = *(Pet *)no->dado;                 /* altera uma copia e so grava se estiver tudo ok */
    for (int i = 0; i < c->qtd; i++) {
        if (strcmp(c->campos[i], "codigo") == 0 || !preencher(&novo, c->campos[i], c->valores[i])) {
            printf("  Erro: campo '%s' invalido ou nao pode ser alterado.\n", c->campos[i]);
            return;
        }
    }
    erro = validar(&novo, pessoas, tipos);   /* dono e tipo continuam validos? */
    if (erro != NULL) {
        printf("  Erro: %s.\n", erro);
        return;
    }
    *(Pet *)no->dado = novo;
    printf("  Pet alterado.\n");
}

static void remover(Comando *c, Lista *pets)
{
    No *no = lista_buscar(pets, c->where_codigo);
    if (no == NULL) {
        printf("  Erro: pet nao encontrado.\n");
        return;
    }
    lista_remover(pets, no);
    printf("  Pet removido.\n");
}

static void mostrar(void *dado)
{
    Pet *p = dado;
    printf("  %d | dono: %d | %s | tipo: %d\n", p->codigo, p->codigo_pes, p->nome, p->codigo_tipo);
}

static void selecionar(Comando *c, Lista *pets)
{
    NoArvore *raiz = NULL;

    for (No *no = pets->ini; no; no = no->prox) {
        Pet *p = no->dado;
        if (c->tem_where && p->codigo != c->where_codigo) continue;
        if (c->ordenar) raiz = arvore_inserir(raiz, p->nome, p);   /* a arvore ordena */
        else            mostrar(p);
    }
    arvore_percorrer(raiz, mostrar);
    arvore_liberar(raiz);
}

void pet_executar(Comando *c, Lista *pets, Lista *pessoas, Lista *tipos)
{
    if      (c->operacao == 'I') inserir(c, pets, pessoas, tipos);
    else if (c->operacao == 'U') alterar(c, pets, pessoas, tipos);
    else if (c->operacao == 'D') remover(c, pets);
    else                         selecionar(c, pets);
}
