#include <stdio.h>
#include <string.h>
#include "lista.h"
#include "comando.h"
#include "pessoa.h"
#include "tipo_pet.h"
#include "pet.h"

#define ARQ_PESSOAS   "pessoas.bin"
#define ARQ_TIPOS_PET "tipos_pet.bin"
#define ARQ_PETS      "pets.bin"

/* Le o script (arquivo ou teclado) e poe cada linha na fila de comandos. */
static void ler_script(Fila *fila)
{
    char linha[TAM_DESC], nome_arquivo[100];
    int opcao = 0, ordem = 0;
    FILE *entrada = stdin;

    printf("1 - ler script de arquivo\n2 - digitar script\nEscolha: ");
    scanf("%d", &opcao);
    while (getchar() != '\n');

    if (opcao == 1) {
        printf("Nome do arquivo: ");
        fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
        nome_arquivo[strcspn(nome_arquivo, "\r\n")] = '\0';
        entrada = fopen(nome_arquivo, "r");
        if (entrada == NULL) {
            printf("Nao foi possivel abrir '%s'.\n", nome_arquivo);
            return;
        }
    } else {
        printf("Digite um comando por linha (linha vazia termina):\n");
    }

    while (fgets(linha, sizeof(linha), entrada)) {
        Comando c = {0};
        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0') {
            if (entrada == stdin) break;
            continue;
        }
        c.ordem = ordem++;
        strcpy(c.descricao, linha);
        fila_enfileirar(fila, &c);
    }
    if (entrada != stdin) fclose(entrada);
}

/* Esvazia a fila de comandos: joga fora os invalidos e separa os validos por tabela. */
static void separar_comandos(Fila *fila, Fila *f_pessoa, Fila *f_tipo, Fila *f_pet)
{
    Comando c;

    while (fila_desenfileirar(fila, &c)) {
        if (!validar_comando(&c))
            printf("Comando invalido (descartado): %s\n", c.descricao);
        else if (strcmp(c.tabela, "pessoa") == 0)
            fila_enfileirar(f_pessoa, &c);
        else if (strcmp(c.tabela, "tipo_pet") == 0)
            fila_enfileirar(f_tipo, &c);
        else
            fila_enfileirar(f_pet, &c);
    }
}

/* Das tres filas, devolve a que tem o comando mais antigo do script (NULL se todas vazias).
   Assim os comandos rodam na ordem em que foram escritos. */
static Fila *proxima_fila(Fila *f_pessoa, Fila *f_tipo, Fila *f_pet)
{
    Fila *filas[3] = {f_pessoa, f_tipo, f_pet};
    Fila *escolhida = NULL;

    for (int i = 0; i < 3; i++) {
        if (filas[i]->ini == NULL) continue;
        if (escolhida == NULL || filas[i]->ini->cmd.ordem < escolhida->ini->cmd.ordem)
            escolhida = filas[i];
    }
    return escolhida;
}

int main(void)
{
    Lista pessoas, tipos, pets;
    Fila fila = {0}, fila_pessoa = {0}, fila_tipo = {0}, fila_pet = {0};
    Fila *proxima;
    Comando c;

    /* 1. monta as listas a partir dos arquivos */
    lista_carregar(&pessoas, ARQ_PESSOAS, sizeof(Pessoa));
    lista_carregar(&tipos, ARQ_TIPOS_PET, sizeof(TipoPet));
    lista_carregar(&pets, ARQ_PETS, sizeof(Pet));

    /* 2. le o script, valida e separa os comandos em filas por tabela */
    ler_script(&fila);
    separar_comandos(&fila, &fila_pessoa, &fila_tipo, &fila_pet);

    /* 3. processa as filas de operacoes */
    while ((proxima = proxima_fila(&fila_pessoa, &fila_tipo, &fila_pet)) != NULL) {
        fila_desenfileirar(proxima, &c);
        printf("> %s\n", c.descricao);

        if (strcmp(c.tabela, "pessoa") == 0)
            pessoa_executar(&c, &pessoas, &pets);
        else if (strcmp(c.tabela, "tipo_pet") == 0)
            tipo_pet_executar(&c, &tipos, &pets);
        else
            pet_executar(&c, &pets, &pessoas, &tipos);
    }

    /* 4. grava as listas nos arquivos */
    lista_salvar(&pessoas, ARQ_PESSOAS);
    lista_salvar(&tipos, ARQ_TIPOS_PET);
    lista_salvar(&pets, ARQ_PETS);
    printf("\nDados salvos.\n");
    return 0;
}