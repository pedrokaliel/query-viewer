#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "comando.h"

/* ================= FILA ================= */

void fila_enfileirar(Fila *f, Comando *c)
{
    NoFila *no = malloc(sizeof(NoFila));
    no->cmd = *c;
    no->prox = NULL;
    if (f->fim) f->fim->prox = no;
    else        f->ini = no;
    f->fim = no;
}

int fila_desenfileirar(Fila *f, Comando *saida)
{
    NoFila *no = f->ini;
    if (no == NULL) return 0;
    *saida = no->cmd;
    f->ini = no->prox;
    if (f->ini == NULL) f->fim = NULL;
    free(no);
    return 1;
}

/* ================= AUXILIARES ================= */

/* tira espacos do comeco e do fim */
static void aparar(char *s)
{
    int fim = strlen(s);
    while (fim > 0 && isspace((unsigned char)s[fim - 1])) fim--;
    s[fim] = '\0';

    int ini = 0;
    while (isspace((unsigned char)s[ini])) ini++;
    memmove(s, s + ini, strlen(s + ini) + 1);
}

/* 'joao' -> joao */
static void tirar_aspas(char *s)
{
    int n = strlen(s);
    if (n >= 2 && s[0] == '\'' && s[n - 1] == '\'') {
        s[n - 1] = '\0';
        memmove(s, s + 1, n - 1);
    }
}

/* Divide "1, 'rua b, nr 2', 3" nas virgulas (ignorando as que estao entre aspas).
   Devolve a quantidade de itens (ou -1 se passar do maximo). */
static int separar(const char *texto, char itens[][TAM_TEXTO])
{
    int qtd = 0, j = 0, aspas = 0;

    for (const char *p = texto; ; p++) {
        if ((*p == ',' && !aspas) || *p == '\0') {
            itens[qtd][j] = '\0';
            aparar(itens[qtd]);
            qtd++;
            j = 0;
            if (*p == '\0') break;
            if (qtd == MAX_CAMPOS) return -1;
        } else {
            if (*p == '\'') aspas = !aspas;
            if (j < TAM_TEXTO - 1) itens[qtd][j++] = *p;
        }
    }
    return qtd;
}

/* ================= VALIDACAO ================= */
/* Em cada sscanf, o %n guarda quantos caracteres foram lidos; so e > 0 se o
   comando casou com o formato inteiro. Depois conferimos se nao sobrou nada. */

/* insert into tabela(campo1, campo2) values(valor1, valor2) */
static int validar_insert(Comando *c, const char *t)
{
    char campos[200], valores[300];
    int n = 0;

    sscanf(t, "insert into %99[a-z_] (%199[^)]) values (%299[^)])%n", c->tabela, campos, valores, &n);
    if (n == 0 || t[n] != '\0') return 0;

    c->qtd = separar(campos, c->campos);
    if (c->qtd <= 0 || c->qtd != separar(valores, c->valores)) return 0;
    for (int i = 0; i < c->qtd; i++)
        tirar_aspas(c->valores[i]);
    return 1;
}

/* update tabela set campo1 = valor1, campo2 = valor2 where codigo = N */
static int validar_update(Comando *c, char *t)
{
    char itens[MAX_CAMPOS][TAM_TEXTO];
    int n = 0, inicio = 0;

    char *onde = strstr(t, " where ");
    if (onde == NULL) return 0;
    sscanf(onde, " where codigo = %d%n", &c->where_codigo, &n);
    if (n == 0 || onde[n] != '\0') return 0;
    *onde = '\0';                                   /* t fica so com "update tabela set ..." */
    c->tem_where = 1;

    sscanf(t, "update %99s set %n", c->tabela, &inicio);
    if (inicio == 0) return 0;

    c->qtd = separar(t + inicio, itens);
    if (c->qtd <= 0) return 0;
    for (int i = 0; i < c->qtd; i++) {              /* "campo = valor" */
        char *igual = strchr(itens[i], '=');
        if (igual == NULL) return 0;
        *igual = '\0';
        strcpy(c->campos[i], itens[i]);
        strcpy(c->valores[i], igual + 1);
        aparar(c->campos[i]);
        aparar(c->valores[i]);
        tirar_aspas(c->valores[i]);
    }
    return 1;
}

/* delete from tabela where codigo = N */
static int validar_delete(Comando *c, const char *t)
{
    int n = 0;

    sscanf(t, "delete from %99s where codigo = %d%n", c->tabela, &c->where_codigo, &n);
    c->tem_where = 1;
    return n > 0 && t[n] == '\0';
}

/* select * from tabela [where codigo = N] [order by nome] */
static int validar_select(Comando *c, const char *t)
{
    int n = 0;

    sscanf(t, "select * from %99s where codigo = %d order by nome%n", c->tabela, &c->where_codigo, &n);
    if (n > 0 && t[n] == '\0') { c->tem_where = 1; c->ordenar = 1; return 1; }

    n = 0;
    sscanf(t, "select * from %99s where codigo = %d%n", c->tabela, &c->where_codigo, &n);
    if (n > 0 && t[n] == '\0') { c->tem_where = 1; return 1; }

    n = 0;
    sscanf(t, "select * from %99s order by nome%n", c->tabela, &n);
    if (n > 0 && t[n] == '\0') { c->ordenar = 1; return 1; }

    n = 0;
    sscanf(t, "select * from %99s%n", c->tabela, &n);
    return n > 0 && t[n] == '\0';
}

int validar_comando(Comando *c)
{
    char t[TAM_DESC];
    int ok;

    strcpy(t, c->descricao);
    aparar(t);
    int n = strlen(t);
    if (n == 0 || t[n - 1] != ';') return 0;       /* precisa terminar com ; */
    t[n - 1] = '\0';
    aparar(t);

    c->qtd = 0;
    c->tem_where = 0;
    c->ordenar = 0;

    if      (strncmp(t, "insert into ", 12) == 0) { c->operacao = 'I'; ok = validar_insert(c, t); }
    else if (strncmp(t, "update ", 7) == 0)       { c->operacao = 'U'; ok = validar_update(c, t); }
    else if (strncmp(t, "delete from ", 12) == 0) { c->operacao = 'D'; ok = validar_delete(c, t); }
    else if (strncmp(t, "select ", 7) == 0)       { c->operacao = 'S'; ok = validar_select(c, t); }
    else return 0;

    return ok && (strcmp(c->tabela, "pessoa") == 0 ||
                  strcmp(c->tabela, "tipo_pet") == 0 ||
                  strcmp(c->tabela, "pet") == 0);
}
