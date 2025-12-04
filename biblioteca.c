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
        fclose(fCheck); // Fecha pois so quero checar a existencia
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

// Salvar todos os livros em um txt

void salvarLivros() {
    char tmp[260];
    snprintf(tmp, sizeof(tmp), "livros.txt.tmp");

    FILE *arq = fopen(tmp, "w");
    if (!arq) {
        printf("ERRO! Nao foi possivel salvar os livros.\n");
        return;
    }

    for (int i = 0; i < total_Livros; i++) {
        fprintf(arq, "%d;%s;%s;%s;%d;%d;%d;%d\n",
                livros[i].codigo,
                livros[i].titulo,
                livros[i].autor,
                livros[i].editora,
                livros[i].ano,
                livros[i].exemnplares,
                livros[i].disponiveis,
                livros[i].vezes_emprestado);
    }

    fclose(arq);

    if (!safe_replace_with_backup("livros.txt", tmp)) {
        printf("ERRO! Nao foi possivel substituir os livros.\n");
    }
}


// Salvar todos os usuarios em um txt

void salvarUsuarios() {
    char tmp[260];
    snprintf(tmp, sizeof(tmp), "usuarios.txt.tmp");
    FILE *arq = fopen(tmp, "w");

    if (!arq) {
        printf("ERRO! Nao foi possivel salvar os usuarios.\n");
        return;
    }

    for (int i = 0; i < total_Usuarios; i++) {
        fprintf(arq, "%d;%s;%s;%s;%d/%d/%d\n",
                usuarios[i].matricula,
                usuarios[i].nome,
                usuarios[i].curso,
                usuarios[i].telefone,
                usuarios[i].cadastro.dia,
                usuarios[i].cadastro.mes,
                usuarios[i].cadastro.ano);
    }
    fclose(arq);
    if (!safe_replace_with_backup("usuarios.txt", tmp)) {
        printf("ERRO! Nao foi possivel substituir os usuarios.\n");
    }
}

//  Salvar todos os emprestimos em um txt

void salvarEmprestimos() {
    char tmp[260];
    snprintf(tmp, sizeof(tmp), "emprestimos.txt.tmp");
    FILE *arq = fopen(tmp, "w");

    if (!arq) {
        printf("ERRO! Nao foi possivel salvar os emprestimos.\n");
        return;
    }

    fprintf(arq, "%d\n", proximoCodEmprestimo);
    for (int i = 0; i < total_Emprestimos; i++) {
        fprintf(arq, "%d;%d;%d;%d/%d/%d;%d/%d/%d;%d/%d/%d;%d\n",
                emprestimos[i].codigo,
                emprestimos[i].matricula,
                emprestimos[i].codigo_livro,
                emprestimos[i].emprestimo.dia,
                emprestimos[i].emprestimo.mes,
                emprestimos[i].emprestimo.ano,
                emprestimos[i].devolucao.dia,
                emprestimos[i].devolucao.mes,
                emprestimos[i].devolucao.ano,
                emprestimos[i].previsto.dia,
                emprestimos[i].previsto.mes,
                emprestimos[i].previsto.ano,
                emprestimos[i].status);
    }
    fclose(arq);
    if (!safe_replace_with_backup("emprestimos.txt", tmp)) {
        printf("ERRO! Nao foi possivel substituir os emprestimos.\n");
    }
}

// Aqui carrega os livros de um arquivo txt, preenchendo o array em memoria 

void carregarLivros() {
    FILE *arq = fopen("livros.txt", "r");
    if (!arq) {
        return;
    }

    char line[512];
    total_Livros = 0;

    while (fgets(line, sizeof(line), arq) != NULL) {
        int codigo, ano, exem, dispon, vezes;
        char titulo[MAX_TITULO];
        char autor[MAX_AUTOR];
        char editora[MAX_EDITORA];

        // formato salvo: %d;%s;%s;%s;%d;%d;%d;%d\n 

        if (sscanf(line, "%d;%100[^;];%80[^;];%60[^;];%d;%d;%d;%d",
                   &codigo, titulo, autor, editora, &ano, &exem, &dispon, &vezes) != 8) {

            continue; // linha mal-formada ou vazia
        }

        if (total_Livros >= MAX_LIVROS) break;

        livros[total_Livros].codigo = codigo;
        strncpy(livros[total_Livros].titulo, titulo, MAX_TITULO - 1);
        livros[total_Livros].titulo[MAX_TITULO - 1] = '\0';
        strncpy(livros[total_Livros].autor, autor, MAX_AUTOR - 1);
        livros[total_Livros].autor[MAX_AUTOR - 1] = '\0';
        strncpy(livros[total_Livros].editora, editora, MAX_EDITORA - 1);
        livros[total_Livros].editora[MAX_EDITORA - 1] = '\0';
        livros[total_Livros].ano = ano;
        livros[total_Livros].exemnplares = exem;
        livros[total_Livros].disponiveis = dispon;
        livros[total_Livros].vezes_emprestado = vezes;

        total_Livros++;
    }

    fclose(arq);
}

// Aqui carrega os usuarios de um arquivo txt, preenchendo o array em memoria

void carregarUsuarios() {
    FILE *arq = fopen("usuarios.txt", "r");
    if (!arq) {
        return;

        total_Usuarios = 0;

        while (fscanf(arq, "%d", &usuarios[total_Usuarios].matricula) == 1) {
            fgetc(arq);

            fgets(usuarios[total_Usuarios].nome, MAX_NOME, arq);
            usuarios[total_Usuarios].nome[strcspn(usuarios[total_Usuarios].nome, "\n")] = 0;
            fgets(usuarios[total_Usuarios].curso, MAX_CURSO, arq);
            usuarios[total_Usuarios].curso[strcspn(usuarios[total_Usuarios].curso, "\n")] = 0;
            fgets(usuarios[total_Usuarios].telefone, MAX_TEL, arq);
            usuarios[total_Usuarios].telefone[strcspn(usuarios[total_Usuarios].telefone, "\n")] = 0;

            if (fscanf(arq, "%d/%d/%d\n",
                       &usuarios[total_Usuarios].cadastro.dia,
                       &usuarios[total_Usuarios].cadastro.mes,
                       &usuarios[total_Usuarios].cadastro.ano) != 3) {
                break; 

                total_Usuarios++;
                if(total_Usuarios >= MAX_USUARIOS) break;
            }
            
                fclose(arq);
        
            }
        }
    }

// Aqui carrega os emprestimos de um arquivo txt, preenchendo o array em memoria

void carregarEmprestimos() {
    FILE* arq = fopen("emprestimos.txt", "r");

    if (!arq) {
        return;

        if (fscanf(arq, "%d\n", &proximoCodEmprestimo) != 1) {
            proximoCodEmprestimo = 1; 

            total_Emprestimos = 0;
            while (fscanf(arq, "%d", &emprestimos[total_Emprestimos].codigo) == 1) {
                if (fscanf(arq, "%d%d", &emprestimos[total_Emprestimos].matricula,
                           &emprestimos[total_Emprestimos].codigo_livro) != 2) break; 
                            if(fscanf(arq, "%d%d%d", &emprestimos[total_Emprestimos].emprestimo.dia,
                                       &emprestimos[total_Emprestimos].emprestimo.mes,
                                       &emprestimos[total_Emprestimos].emprestimo.ano) != 3) break;
                            if(fscanf(arq, "%d%d%d", &emprestimos[total_Emprestimos].devolucao.dia,
                                       &emprestimos[total_Emprestimos].devolucao.mes,
                                       &emprestimos[total_Emprestimos].devolucao.ano) != 3) break;
                            if(fscanf(arq, "%d%d%d", &emprestimos[total_Emprestimos].previsto.dia,
                                       &emprestimos[total_Emprestimos].previsto.mes,
                                       &emprestimos[total_Emprestimos].previsto.ano) != 3) break;
                            if(fscanf(arq, "%d\n", &emprestimos[total_Emprestimos].status) != 1) break;

                            total_Emprestimos++;
                            if(total_Emprestimos >= MAX_EMPRESTIMOS) break;
                }

                fclose(arq);
            }
       
        }
    }

//========== FUNCOES DO SISTEMA ==========
// Implementacao das funcoes principais do sistema de biblioteca
// como cadastrar livros, usuarios, realizar emprestimos, devolucoes etc.
// A primeira funcao a ser implementada e o cadastro de livros.
// e tambem faz validacoes para evitar cadastros duplicados.
void cadastrarLivro() {
    limparTela();
    if (total_Livros >= MAX_LIVROS) {

        printf("Limite de livros atingido!\n");
        pausar();
        return;
    }

    Livro l;
    printf ("=== Cadastro do Livro ===\n");
    printf("Codigo: ");
    if (scanf("%d", &l.codigo) != 1 || l.codigo <= 0 ) {
        printf("Codigo invalido!\n");
        limparBuffer();
        pausar();
        return;
    }

    // Verifica se o codigo ja existe
    for (int i = 0; i < total_Livros; i++) {
        if (livros[i].codigo == l.codigo) {
            printf("Codigo ja cadastrado!\n");
            pausar();
            return;
        }
    }

    limparBuffer(); 

    printf("Titulo: ");
    fgets(l.titulo, MAX_TITULO, stdin);
    l.titulo[strcspn(l.titulo, "\n")] = 0;

    printf("Autor: ");
    fgets(l.autor, MAX_AUTOR, stdin);
    l.autor[strcspn(l.autor, "\n")] = 0;

    printf("Editora: ");
    fgets(l.editora, MAX_EDITORA, stdin);
    l.editora[strcspn(l.editora, "\n")] = 0;

    printf("Ano de publicacao: ");
    if (scanf("%d", &l.ano) != 1 || l.ano <= 0 ) {
        printf("Ano invalido!\n");
        limparBuffer();
        pausar();
        return;

    }

    printf("Numero de exemplares: ");
    if (scanf("%d", &l.exemnplares) != 1 || l.exemnplares <= 0 ) {
        printf("Numero de exemplares invalido!\n");
        limparBuffer();
        pausar();
        return; 
    }

    l.disponiveis = l.exemnplares; // Inicialmente todos disponiveis
    l.vezes_emprestado = 0;

    livros[total_Livros++] = l; // Adiciona ao array
    salvarLivros(); // Salva no arquivo

    printf("Livro cadastrado com sucesso!\n");
    pausar();

}

// Implementacao da funcao de cadastro de usuarios
void cadastrarUsuario() {
    limparTela();
    if (total_Usuarios >= MAX_USUARIOS) {
        printf("Limite de usuarios atingido!\n");
        pausar();
        return;
    }

    Usuario u;

    printf("=== Cadastro de Usuario ===\n");
    printf("Matricula: ");
    if (scanf("%d", &u.matricula) != 1 || u.matricula <= 0 ) { // Valida matricula
        printf("\nMatricula invalida!\n");
        limparBuffer();
        pausar();
        return;

    }

    for (int i = 0; i < total_Usuarios; i++) {
        if (usuarios[i].matricula == u.matricula) { // Verifica duplicidade
            printf("\nMatricula ja cadastrada!\n");
            pausar();
            return;

        }
    }

    limparBuffer();

    printf("Nome: ");
    fgets(u.nome, MAX_NOME, stdin);
    u.nome[strcspn(u.nome, "\n")] = 0;

    printf("Curso: ");
    fgets(u.curso, MAX_CURSO, stdin);
    u.curso[strcspn(u.curso, "\n")] = 0;

    printf("Telefone: ");
    fgets(u.telefone, MAX_TEL, stdin);
    u.telefone[strcspn(u.telefone, "\n")] = 0;

    u.cadastro = dataAtual();
    usuarios[total_Usuarios++] = u; // Adiciona ao array
    salvarUsuarios(); // Salva no arquivo   

    printf("\nUsuario cadastrado com sucesso!\n");

    pausar();
}    

// Implementacao da funcao de realizar emprestimo
void realizarEmprestimo() {
    limparTela();
    int mat, cod;
    printf("=== Realizar emprestimo ===\n");
    printf("Matricula do usuario: ");

    if (scanf("%d", &mat) != 1 ) {
        printf("Matricula invalida. \n");
        limparBuffer();
        pausar();
        return;
    }

    int idxUsuario = -1;
    for(int i = 0; i <total_Usuarios; i++) {
        if (usuarios[i].matricula == mat) {
            idxUsuario = i;
            break;
        }
    }

    if (idxUsuario == -1) {
        printf("Usuario nao encontrado.\n");
        pausar();
        return;
    }

    printf("Codigo do livro: ");
    if (scanf("%d", &cod) != 1 ) {
        printf("Codigo invalido. \n");
        limparBuffer();
        pausar();
        return;
}

