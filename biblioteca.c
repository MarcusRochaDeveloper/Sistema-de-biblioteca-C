#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//========== DEFINICOES E CONSTANTES==========

#define MAX_LIVROS 1000
#define MAX_USUARIOS 1000
#define MAX_EMPRESTIMOS 2000

//========== TAMANHOS MAXIMOS PARA STRINGS COM ESPACO PARA CARACTER NULO==========

#define MAX_TITULO 101
#define MAX_AUTOR 81
#define MAX_EDITORA 61
#define MAX_NOME 101
#define MAX_CURSO 51
#define MAX_TEL 16

//========== ESTRUTURAS DE DADOS ==========
// Estruturas simples para representar uma data

typedef struct
{
    int dia, mes, ano;
} Data;

// Estrutura para representar um livro
typedef struct {
    int codigo;
    char titulo[MAX_TITULO];
    char autor[MAX_AUTOR];
    char editora[MAX_EDITORA];
    int ano;
    int exemnplares;
    int disponiveis;
    int vezes_emprestado;
}Livro;