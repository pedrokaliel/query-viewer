#ifndef COMANDO_H
#define COMANDO_H

#define TAM_DESC   300
#define TAM_TEXTO  100
#define MAX_CAMPOS 6

typedef struct {
    int  ordem;                          /* posicao do comando no script */
    char descricao[TAM_DESC];            /* texto do comando */

    /* preenchidos por validar_comando: */
    char operacao;                       /* 'I' insert, 'U' update, 'D' delete, 'S' select */
    char tabela[TAM_TEXTO];              /* pessoa, tipo_pet ou pet */
    int  qtd;                            /* quantidade de campos (insert/update) */
    char campos[MAX_CAMPOS][TAM_TEXTO];
    char valores[MAX_CAMPOS][TAM_TEXTO];
    int  tem_where;                      /* where codigo = ... */
    int  where_codigo;
    int  ordenar;                        /* order by nome */
} Comando;

/* Fila de comandos */
typedef struct NoFila {
    Comando cmd;
    struct NoFila *prox;
} NoFila;

typedef struct {
    NoFila *ini, *fim;
} Fila;

void fila_enfileirar(Fila *f, Comando *c);
int  fila_desenfileirar(Fila *f, Comando *saida);   /* 0 se a fila esta vazia */

int  validar_comando(Comando *c);                   /* 1 = valido */

#endif
