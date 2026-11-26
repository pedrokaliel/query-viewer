#ifndef ARVORE_H
#define ARVORE_H

/* Arvore binaria de busca usada no "order by nome". */

typedef struct NoArvore {
    const char *chave;               /* nome (usado para ordenar) */
    void *dado;                      /* registro completo */
    struct NoArvore *esq, *dir;
} NoArvore;

NoArvore *arvore_inserir(NoArvore *raiz, const char *chave, void *dado);
void arvore_percorrer(NoArvore *raiz, void (*mostrar)(void *));   /* em ordem */
void arvore_liberar(NoArvore *raiz);

#endif
