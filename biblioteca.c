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

// Estrutura para representar um usuario
typedef struct {
    int matricula;
    char nome[MAX_NOME];
    char curso[MAX_CURSO];
    char telefone[MAX_TEL];
    Data cadastro;
}Usuario;

// Estrutura para representar um emprestimo
typedef struct {
    int codigo;
    int matricula;
    int codigo_livro;
    Data emprestimo;
    Data devolucao;
    Data previsto;
    int status; // 0 - ativo, 1 - devolvido
}Emprestimo;

//========== VARIAVEIS GLOBAIS ==========
// Arrays para armazenar livros, usuarios e emprestimos

Livro livros[MAX_LIVROS]; 
Usuario usuarios[MAX_USUARIOS];
Emprestimo emprestimos[MAX_EMPRESTIMOS];

// Contadores para o numero de livros, usuarios e emprestimos cadastrados
int total_Livros = 0; 
int total_Usuarios = 0;
int total_Emprestimos = 0;
int proximoCodEmprestimo = 1; // Contador para o proximo codigo de emprestimo

//========== PROTOTIPOS DAS FUNCOES ==========
// Utilitarias para usar durante o programa
void limparTela();
void pausar();
void limparBuffer();
Data dataAtual();
int compararDatas(Data d1, Data d2);
void adicionarDias(Data *data, int dias);

// Arquivos
void salvarLivros();
void salvarUsuarios();
void salvarEmprestimos();
void carregarLivros();
void carregarUsuarios();
void carregarEmprestimos();
void backupAutomatico(const char *nome);
int safe_replace_with_backup(const char *filename, const char *tmpname);

//Operacoes essenciais do sistema
void cadastrarLivro();
void cadastrarUsuario();
void realizarEmprestimo();
void realizarDevolucao();
void pesquisarLivro();
void pesquisarUsuario();
void listarEmprestimosAtivos();
void relatorioMaisEmprestados();
void relatorioAtrasados();
void renovarEmprestimo();
void menuPrincipal();

//========== FUNCAO UTILITARIAS ==========
// Implementei uma funcao de limpar a tela do terminal 
// para melhorar a experiencia do usuario.
// Porem essa funcao pode nao funcionar em todos os sistemas operacionais 
// no caso do windows, use o comando "cls".
// Tambem foi adicionado uma funcao de pausar a execucao
// para que o usuario possa ler as mensagens antes de prosseguir.

void limparTela() {
    system("clear");
}

void pausar() {
    printf("Pressione Enter para continuar...");
    limparBuffer(); // garante que o buffer esteja limpo antes do getchar
    getchar();
}

// Limpa os caracateres restantes no buffer de entrada stdin
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Essa funcao vai ser responsavel por retornar a data atual do sistema
Data dataAtual() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    Data d = {tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900};
    return d;
}

// Aqui vamos comparar duas datas,
// retorna um valor negativo se d1 < d2, 
// zero se forem iguais, positivo se d1 > d2

int compararDatas(Data d1, Data d2)
{
    if (d1.ano != d2.ano)
        return d1.ano - d2.ano;
    if (d1.mes != d2.mes)
        return d1.mes - d2.mes;
    return d1.dia - d2.dia;
}

// Pensei em usar o mktime para facilitar a adicao de dias
void adicionarDias(Data *d, int dias) {

    struct tm tm = {0};
    tm.tm_mday = d->dia;
    tm.tm_mon = d->mes - 1;
    tm.tm_year = d->ano - 1900;
    tm.tm_isdst = -1; 

    tm.tm_mday += dias; // Soma os dias
    mktime(&tm); // Normaliza a estrutura tm

    d->dia = tm.tm_mday;
    d->mes = tm.tm_mon + 1;
    d->ano = tm.tm_year + 1900;
}

//========== DADOS ==========
// Apesar da atividade mapa nao solicitar a implementacao de backup,
// achei interessante implementar uma nova funcao.
// Ela vai gerar um arquivo a partir da criacao do original
// com sufixo .bak

void backupAutomatico(const char *nome) {

    // Implementei uma logica que espera que exista um arquivo temporario

    char tmp[260];
    snprintf(tmp, sizeof(tmp), "%s.tmp", nome);
    safe_replace_with_backup(nome, tmp);
}

int safe_replace_with_backup(const char *filename, const char *tmpname)
{
    char backup[260];
    snprintf(backup, sizeof(backup), "%s.bak", filename);

    // remove backup antigo
    remove(backup);

    // se existe o arquivo original, renomeia para backup
    FILE *fCheck = fopen(filename, "r");
    if (fCheck != NULL)
    {
        fclose(fCheck); // Fecha pois so quero checar existencia
        if (rename(filename, backup) != 0)
            return 0; // falha
    }

    // renomeia temp -> filename
    if (rename(tmpname, filename) != 0)
    {
        // tentativa de restaurar backup caso falhe
        rename(backup, filename);
        return 0;
    }
    return 1;
}


