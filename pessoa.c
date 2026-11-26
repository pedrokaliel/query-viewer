#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pessoa.h"
#include "pet.h"

/* Coloca o valor no campo. Devolve 0 se o campo nao existe. */
static int preencher(Pessoa *p, const char *campo, const char *valor)
{
    if      (strcmp(campo, "codigo") == 0)          p->codigo = atoi(valor);
    else if (strcmp(campo, "nome") == 0)            strncpy(p->nome, valor, sizeof(p->nome) - 1);
    else if (strcmp(campo, "fone") == 0)            strncpy(p->fone, valor, sizeof(p->fone) - 1);
    else if (strcmp(campo, "data_nascimento") == 0) strncpy(p->data_nascimento, valor, sizeof(p->data_nascimento) - 1);
    else if (strcmp(campo, "endereco") == 0)        strncpy(p->endereco, valor, sizeof(p->endereco) - 1);
    else return 0;
    return 1;
}

static int data_valida(const char *d)
{
    int dia, mes, ano, n = 0;
    sscanf(d, "%d/%d/%d%n", &dia, &mes, &ano, &n);
    return n == 10 && dia >= 1 && dia <= 31 && mes >= 1 && mes <= 12 && ano >= 1900;
}

/* Devolve a mensagem de erro, ou NULL se a pessoa esta ok. */
static const char *validar(Pessoa *p)
{
    if (p->codigo <= 0 || p->nome[0] == '\0' || p->data_nascimento[0] == '\0')
        return "campos obrigatorios: codigo, nome, data_nascimento";
    if (!data_valida(p->data_nascimento))
        return "data_nascimento deve ser dd/mm/aaaa";
    return NULL;
}

static void inserir(Comando *c, Lista *pessoas)
{
    Pessoa p = {0};
    const char *erro;

    for (int i = 0; i < c->qtd; i++) {
        if (!preencher(&p, c->campos[i], c->valores[i])) {
            printf("  Erro: campo '%s' nao existe.\n", c->campos[i]);
            return;
        }
    }
    erro = validar(&p);
    if (erro == NULL && lista_buscar(pessoas, p.codigo) != NULL)
        erro = "ja existe pessoa com esse codigo";
    if (erro != NULL) {
        printf("  Erro: %s.\n", erro);
        return;
    }
    lista_inserir(pessoas, &p);
    printf("  Pessoa inserida.\n");
}

static void alterar(Comando *c, Lista *pessoas)
{
    No *no = lista_buscar(pessoas, c->where_codigo);
    Pessoa novo;
    const char *erro;

    if (no == NULL) {
        printf("  Erro: pessoa nao encontrada.\n");
        return;
    }
    novo = *(Pessoa *)no->dado;              /* altera uma copia e so grava se estiver tudo ok */
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
    *(Pessoa *)no->dado = novo;
    printf("  Pessoa alterada.\n");
}

/* Nao remove pessoa que ainda tem pets. */
static void remover(Comando *c, Lista *pessoas, Lista *pets)
{
    No *no = lista_buscar(pessoas, c->where_codigo);
    if (no == NULL) {
        printf("  Erro: pessoa nao encontrada.\n");
        return;
    }
    for (No *x = pets->ini; x; x = x->prox) {
        if (((Pet *)x->dado)->codigo_pes == c->where_codigo) {
            printf("  Erro: a pessoa ainda possui pets, nao foi removida.\n");
            return;
        }
    }
    lista_remover(pessoas, no);
    printf("  Pessoa removida.\n");
}

static void mostrar(void *dado)
{
    Pessoa *p = dado;
    printf("  %d | %s | %s | %s | %s\n", p->codigo, p->nome, p->fone, p->data_nascimento, p->endereco);
}

static void selecionar(Comando *c, Lista *pessoas)
{
    NoArvore *raiz = NULL;

    for (No *no = pessoas->ini; no; no = no->prox) {
        Pessoa *p = no->dado;
        if (c->tem_where && p->codigo != c->where_codigo) continue;
        if (c->ordenar) raiz = arvore_inserir(raiz, p->nome, p);   /* a arvore ordena */
        else            mostrar(p);
    }
    arvore_percorrer(raiz, mostrar);
    arvore_liberar(raiz);
}

void pessoa_executar(Comando *c, Lista *pessoas, Lista *pets)
{
    if      (c->operacao == 'I') inserir(c, pessoas);
    else if (c->operacao == 'U') alterar(c, pessoas);
    else if (c->operacao == 'D') remover(c, pessoas, pets);
    else                         selecionar(c, pessoas);
}
