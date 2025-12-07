#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//========== DEFINICOES E CONSTANTES ==========
#define MAX_LIVROS 1000
#define MAX_USUARIOS 1000
#define MAX_EMPRESTIMOS 2000

#define MAX_TITULO 101
#define MAX_AUTOR 81
#define MAX_EDITORA 61
#define MAX_NOME 101
#define MAX_CURSO 51
#define MAX_TEL 16

//========== ESTRUTURAS DE DADOS ==========
typedef struct {
    int dia, mes, ano;
} Data;

typedef struct {
    int codigo;
    char titulo[MAX_TITULO];
    char autor[MAX_AUTOR];
    char editora[MAX_EDITORA];
    int ano;
    int exemplares; 
    int disponiveis;
    int vezes_emprestado;
} Livro;

typedef struct {
    int matricula;
    char nome[MAX_NOME];
    char curso[MAX_CURSO];
    char telefone[MAX_TEL];
    Data cadastro;
} Usuario;

typedef struct {
    int codigo;
    int matricula;
    int codigo_livro;
    Data emprestimo;
    Data devolucao;
    Data previsto;
    int status; // 0 - devolvido, 1 - ativo
} Emprestimo;

//========== VARIAVEIS GLOBAIS ==========
Livro livros[MAX_LIVROS];
Usuario usuarios[MAX_USUARIOS];
Emprestimo emprestimos[MAX_EMPRESTIMOS];

int total_Livros = 0;
int total_Usuarios = 0;
int total_Emprestimos = 0;
int proximoCodEmprestimo = 1;

//========== UTILITARIOS ==========

void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pausar() {
    printf("\nPressione Enter para continuar...");
    getchar();
}

Data dataAtual() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    Data d = {tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900};
    return d;
}

int compararDatas(Data d1, Data d2) {
    if (d1.ano != d2.ano) return d1.ano - d2.ano;
    if (d1.mes != d2.mes) return d1.mes - d2.mes;
    return d1.dia - d2.dia;
}

void adicionarDias(Data *d, int dias) {
    struct tm tm = {0};
    tm.tm_mday = d->dia;
    tm.tm_mon = d->mes - 1;
    tm.tm_year = d->ano - 1900;
    tm.tm_isdst = -1;

    tm.tm_mday += dias;
    mktime(&tm);

    d->dia = tm.tm_mday;
    d->mes = tm.tm_mon + 1;
    d->ano = tm.tm_year + 1900;
}

//========== ARQUIVOS E BACKUP ==========

int safe_replace_with_backup(const char *filename, const char *tmpname) {
    char backup[260];
    snprintf(backup, sizeof(backup), "%s.bak", filename);
    remove(backup);

    FILE *fCheck = fopen(filename, "r");
    if (fCheck != NULL) {
        fclose(fCheck);
        if (rename(filename, backup) != 0) return 0;
    }

    if (rename(tmpname, filename) != 0) {
        rename(backup, filename);
        return 0;
    }
    return 1;
}

void salvarLivros() {
    char tmp[260] = "livros.txt.tmp";
    FILE *arq = fopen(tmp, "w");
    if (!arq) return;

    for (int i = 0; i < total_Livros; i++) {
        fprintf(arq, "%d;%s;%s;%s;%d;%d;%d;%d\n",
                livros[i].codigo, livros[i].titulo, livros[i].autor,
                livros[i].editora, livros[i].ano, livros[i].exemplares,
                livros[i].disponiveis, livros[i].vezes_emprestado);
    }
    fclose(arq);
    safe_replace_with_backup("livros.txt", tmp);
}

void salvarUsuarios() {
    char tmp[260] = "usuarios.txt.tmp";
    FILE *arq = fopen(tmp, "w");
    if (!arq) return;

    for (int i = 0; i < total_Usuarios; i++) {
        fprintf(arq, "%d;%s;%s;%s;%d/%d/%d\n",
                usuarios[i].matricula, usuarios[i].nome, usuarios[i].curso,
                usuarios[i].telefone, usuarios[i].cadastro.dia,
                usuarios[i].cadastro.mes, usuarios[i].cadastro.ano);
    }
    fclose(arq);
    safe_replace_with_backup("usuarios.txt", tmp);
}

void salvarEmprestimos() {
    char tmp[260] = "emprestimos.txt.tmp";
    FILE *arq = fopen(tmp, "w");
    if (!arq) return;

    fprintf(arq, "%d\n", proximoCodEmprestimo);
    for (int i = 0; i < total_Emprestimos; i++) {
        fprintf(arq, "%d;%d;%d;%d/%d/%d;%d/%d/%d;%d/%d/%d;%d\n",
                emprestimos[i].codigo, emprestimos[i].matricula, emprestimos[i].codigo_livro,
                emprestimos[i].emprestimo.dia, emprestimos[i].emprestimo.mes, emprestimos[i].emprestimo.ano,
                emprestimos[i].devolucao.dia, emprestimos[i].devolucao.mes, emprestimos[i].devolucao.ano,
                emprestimos[i].previsto.dia, emprestimos[i].previsto.mes, emprestimos[i].previsto.ano,
                emprestimos[i].status);
    }
    fclose(arq);
    safe_replace_with_backup("emprestimos.txt", tmp);
}

void carregarLivros() {
    FILE *arq = fopen("livros.txt", "r");
    if (!arq) return;

    char line[512];
    total_Livros = 0;

    while (fgets(line, sizeof(line), arq) != NULL) {
        if (total_Livros >= MAX_LIVROS) break;
        
        int codigo, ano, exem, dispon, vezes;
        char titulo[MAX_TITULO], autor[MAX_AUTOR], editora[MAX_EDITORA];

        if (sscanf(line, "%d;%100[^;];%80[^;];%60[^;];%d;%d;%d;%d",
                   &codigo, titulo, autor, editora, &ano, &exem, &dispon, &vezes) == 8) {
            
            livros[total_Livros].codigo = codigo;
            strcpy(livros[total_Livros].titulo, titulo);
            strcpy(livros[total_Livros].autor, autor);
            strcpy(livros[total_Livros].editora, editora);
            livros[total_Livros].ano = ano;
            livros[total_Livros].exemplares = exem;
            livros[total_Livros].disponiveis = dispon;
            livros[total_Livros].vezes_emprestado = vezes;
            total_Livros++;
        }
    }
    fclose(arq);
}

void carregarUsuarios() {
    FILE *arq = fopen("usuarios.txt", "r");
    if (!arq) return;

    char line[512];
    total_Usuarios = 0;

    while (fgets(line, sizeof(line), arq) != NULL) {
        if (total_Usuarios >= MAX_USUARIOS) break;

        int matricula, d, m, a;
        char nome[MAX_NOME], curso[MAX_CURSO], tel[MAX_TEL];

        if (sscanf(line, "%d;%100[^;];%50[^;];%15[^;];%d/%d/%d",
                   &matricula, nome, curso, tel, &d, &m, &a) == 7) {
            
            usuarios[total_Usuarios].matricula = matricula;
            strcpy(usuarios[total_Usuarios].nome, nome);
            strcpy(usuarios[total_Usuarios].curso, curso);
            strcpy(usuarios[total_Usuarios].telefone, tel);
            usuarios[total_Usuarios].cadastro.dia = d;
            usuarios[total_Usuarios].cadastro.mes = m;
            usuarios[total_Usuarios].cadastro.ano = a;
            total_Usuarios++;
        }
    }
    fclose(arq);
}

void carregarEmprestimos() {
    FILE *arq = fopen("emprestimos.txt", "r");
    if (!arq) return;

    if (fscanf(arq, "%d\n", &proximoCodEmprestimo) != 1) proximoCodEmprestimo = 1;

    char line[512];
    total_Emprestimos = 0;

    while (fgets(line, sizeof(line), arq) != NULL) {
        if (total_Emprestimos >= MAX_EMPRESTIMOS) break;

        Emprestimo e;
        if (sscanf(line, "%d;%d;%d;%d/%d/%d;%d/%d/%d;%d/%d/%d;%d",
                   &e.codigo, &e.matricula, &e.codigo_livro,
                   &e.emprestimo.dia, &e.emprestimo.mes, &e.emprestimo.ano,
                   &e.devolucao.dia, &e.devolucao.mes, &e.devolucao.ano,
                   &e.previsto.dia, &e.previsto.mes, &e.previsto.ano,
                   &e.status) == 13) {
            emprestimos[total_Emprestimos++] = e;
        }
    }
    fclose(arq);
}

//========== FUNCOES PRINCIPAIS ==========

void cadastrarLivro() {
    limparTela();
    if (total_Livros >= MAX_LIVROS) {
        printf("Limite de livros atingido!\n");
        pausar();
        return;
    }

    Livro l;
    printf("=== Cadastro do Livro ===\n");
    printf("Codigo: ");
    if (scanf("%d", &l.codigo) != 1 || l.codigo <= 0) {
        printf("Codigo invalido!\n");
        limparBuffer();
        pausar();
        return;
    }

    for (int i = 0; i < total_Livros; i++) {
        if (livros[i].codigo == l.codigo) {
            printf("Erro: Codigo ja cadastrado!\n");
            limparBuffer();
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

    // --- Validacao de Data ---
    int anoLimite = dataAtual().ano; // Pega o ano atual do sistema (ex: 2025)
    
    printf("Ano de publicacao (1900 a %d): ", anoLimite);
    if (scanf("%d", &l.ano) != 1) {
        printf("Entrada invalida! Digite apenas numeros.\n");
        limparBuffer();
        pausar();
        return;
    }
    
    if (l.ano < 1900 || l.ano > anoLimite) {
        printf("Ano invalido! O ano deve ser entre 1900 e %d.\n", anoLimite);
        limparBuffer();
        pausar();
        return;
    }
    limparBuffer();
   
    printf("Numero de exemplares: ");
    if (scanf("%d", &l.exemplares) != 1 || l.exemplares <= 0) {
        printf("Numero de exemplares invalido!\n");
        limparBuffer();
        pausar();
        return;
    }
    limparBuffer();

    l.disponiveis = l.exemplares;
    l.vezes_emprestado = 0;

    livros[total_Livros++] = l;
    salvarLivros();
    printf("Livro cadastrado com sucesso!\n");
    pausar();
}

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
    if (scanf("%d", &u.matricula) != 1 || u.matricula <= 0) {
        printf("Matricula invalida!\n");
        limparBuffer();
        pausar();
        return;
    }
    
    // Importante limpar buffer aqui, mas ANTES verificar duplicidade
    limparBuffer();

    for (int i = 0; i < total_Usuarios; i++) {
        if (usuarios[i].matricula == u.matricula) {
            printf("Erro: Matricula ja cadastrada!\n");
            pausar();
            return;
        }
    }

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
    usuarios[total_Usuarios++] = u;
    salvarUsuarios();
    printf("Usuario cadastrado com sucesso!\n");
    pausar();
}

void realizarEmprestimo() {
    limparTela();
    int mat, cod;
    
    printf("=== Realizar Emprestimo ===\n");
    printf("Matricula do usuario: ");
    if (scanf("%d", &mat) != 1) {
        limparBuffer();
        printf("Matricula invalida.\n");
        pausar();
        return;
    }
    limparBuffer();

    int idxUsuario = -1;
    for (int i = 0; i < total_Usuarios; i++) {
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
    if (scanf("%d", &cod) != 1) {
        limparBuffer();
        printf("Codigo invalido.\n");
        pausar();
        return;
    }
    limparBuffer();

    int idxLivro = -1;
    for (int i = 0; i < total_Livros; i++) {
        if (livros[i].codigo == cod) {
            idxLivro = i;
            break;
        }
    }

    if (idxLivro == -1) {
        printf("Livro nao encontrado.\n");
        pausar();
        return;
    }

    if (livros[idxLivro].disponiveis <= 0) {
        printf("Nenhum exemplar disponivel.\n");
        pausar();
        return;
    }

    Emprestimo e;
    e.codigo = proximoCodEmprestimo++;
    e.matricula = mat;
    e.codigo_livro = cod;
    e.emprestimo = dataAtual();
    e.previsto = e.emprestimo;
    adicionarDias(&e.previsto, 7);
    e.devolucao = (Data){0, 0, 0};
    e.status = 1;

    emprestimos[total_Emprestimos++] = e;
    livros[idxLivro].disponiveis--;
    livros[idxLivro].vezes_emprestado++;

    salvarEmprestimos();
    salvarLivros();

    printf("Emprestimo realizado! Devolucao prevista: %02d/%02d/%04d\n",
           e.previsto.dia, e.previsto.mes, e.previsto.ano);
    pausar();
}

void realizarDevolucao() {
    limparTela();
    int codEmp;
    int encontrou = 0;

    printf("=== Realizar Devolucao ===\n");
    printf("Codigo do emprestimo: ");
    if (scanf("%d", &codEmp) != 1) {
        limparBuffer();
        printf("Codigo invalido.\n");
        pausar();
        return;
    }
    limparBuffer();

    for (int i = 0; i < total_Emprestimos; i++) {
        if (emprestimos[i].codigo == codEmp && emprestimos[i].status == 1) {
            emprestimos[i].status = 0;
            emprestimos[i].devolucao = dataAtual();
            encontrou = 1;

            for (int j = 0; j < total_Livros; j++) {
                if (livros[j].codigo == emprestimos[i].codigo_livro) {
                    livros[j].disponiveis++;
                    break;
                }
            }

            salvarEmprestimos();
            salvarLivros();

            printf("Devolucao realizada com sucesso!\n");
            if (compararDatas(emprestimos[i].devolucao, emprestimos[i].previsto) > 0) {
                printf("ATENCAO: Livro entregue com atraso!\n");
            }
            break;
        }
    }

    if (!encontrou) {
        printf("Emprestimo nao encontrado ou ja encerrado.\n");
    }
    pausar();
}

void pesquisarLivro() {
    limparTela();
    int op, busca_int;
    char busca_str[100];
    int encontrou = 0;

    printf("=== Pesquisar Livro ===\n");
    printf("1. Por Titulo\n2. Por Autor\n3. Por Codigo\nEscolha: ");
    if (scanf("%d", &op) != 1) {
        limparBuffer();
        printf("Opcao invalida.\n");
        pausar();
        return;
    }
    limparBuffer();

    if (op == 1 || op == 2) {
        printf("Digite o termo: ");
        fgets(busca_str, sizeof(busca_str), stdin);
        busca_str[strcspn(busca_str, "\n")] = 0;

        printf("\nResultados:\n");
        for (int i = 0; i < total_Livros; i++) {
            char *campo = (op == 1) ? livros[i].titulo : livros[i].autor;
            if (strstr(campo, busca_str)) {
                printf("[#%d] %s - %s (Disp: %d)\n",
                       livros[i].codigo, livros[i].titulo, livros[i].autor, livros[i].disponiveis);
                encontrou = 1;
            }
        }
    } else if (op == 3) {
        printf("Codigo: ");
        if (scanf("%d", &busca_int) == 1) {
            for (int i = 0; i < total_Livros; i++) {
                if (livros[i].codigo == busca_int) {
                    printf("[#%d] %s - %s (Disp: %d)\n",
                           livros[i].codigo, livros[i].titulo, livros[i].autor, livros[i].disponiveis);
                    encontrou = 1;
                    break;
                }
            }
        }
        limparBuffer();
    } else {
        printf("Opcao invalida.\n");
    }

    if (!encontrou) printf("Nenhum livro encontrado.\n");
    pausar();
}

void pesquisarUsuario() {
    limparTela();
    int op, mat;
    char termo[100];
    int encontrou = 0;

    printf("=== Pesquisar Usuario ===\n");
    printf("1. Por Matricula\n2. Por Nome\nEscolha: ");
    if (scanf("%d", &op) != 1) {
        limparBuffer();
        printf("Opcao invalida.\n");
        pausar();
        return;
    }
    limparBuffer();

    if (op == 1) {
        printf("Matricula: ");
        if (scanf("%d", &mat) == 1) {
            for (int i = 0; i < total_Usuarios; i++) {
                if (usuarios[i].matricula == mat) {
                    printf("Encontrado: %s (Curso: %s)\n", usuarios[i].nome, usuarios[i].curso);
                    encontrou = 1;
                    break;
                }
            }
        }
        limparBuffer();
    } else if (op == 2) {
        printf("Nome: ");
        fgets(termo, sizeof(termo), stdin);
        termo[strcspn(termo, "\n")] = 0;

        for (int i = 0; i < total_Usuarios; i++) {
            if (strstr(usuarios[i].nome, termo)) {
                printf("Encontrado: [#%d] %s\n", usuarios[i].matricula, usuarios[i].nome);
                encontrou = 1;
            }
        }
    }

    if (!encontrou) printf("Usuario nao encontrado.\n");
    pausar();
}

void listarEmprestimosAtivos() {
    limparTela();
    printf("=== Emprestimos Ativos ===\n");
    int count = 0;

    for (int i = 0; i < total_Emprestimos; i++) {
        if (emprestimos[i].status == 1) {
            char nome[100] = "Desc.";
            char titulo[100] = "Desc.";

            for (int u = 0; u < total_Usuarios; u++) {
                if (usuarios[u].matricula == emprestimos[i].matricula) {
                    strcpy(nome, usuarios[u].nome);
                    break;
                }
            }
            for (int l = 0; l < total_Livros; l++) {
                if (livros[l].codigo == emprestimos[i].codigo_livro) {
                    strcpy(titulo, livros[l].titulo);
                    break;
                }
            }
            printf("Emp [#%d] | %s pegou '%s' | Devolver em: %02d/%02d/%04d\n",
                   emprestimos[i].codigo, nome, titulo,
                   emprestimos[i].previsto.dia, emprestimos[i].previsto.mes, emprestimos[i].previsto.ano);
            count++;
        }
    }

    if (count == 0) printf("Nenhum emprestimo ativo.\n");
    pausar();
}

void relatorioMaisEmprestados() {
    limparTela();
    printf("=== Top Livros Mais Emprestados ===\n");

    Livro temp[MAX_LIVROS];
    int qtd = total_Livros;
    
    // Copia para array temporario
    for (int i = 0; i < qtd; i++) temp[i] = livros[i];

    // Bubble Sort Decrescente
    for (int i = 0; i < qtd - 1; i++) {
        for (int j = 0; j < qtd - i - 1; j++) {
            if (temp[j].vezes_emprestado < temp[j + 1].vezes_emprestado) {
                Livro t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }

    int n = (qtd < 10) ? qtd : 10;
    for (int i = 0; i < n; i++) {
        printf("%d. %s - %d emprestimos\n", i + 1, temp[i].titulo, temp[i].vezes_emprestado);
    }
    pausar();
}

void relatorioAtrasados() {
    limparTela();
    Data hoje = dataAtual();
    printf("=== Relatorio de Atrasos ===\n");
    int count = 0;

    for (int i = 0; i < total_Emprestimos; i++) {
        if (emprestimos[i].status == 1 && compararDatas(hoje, emprestimos[i].previsto) > 0) {
            char nome[100] = "?";
            char titulo[100] = "?";

            for (int u = 0; u < total_Usuarios; u++)
                if (usuarios[u].matricula == emprestimos[i].matricula) strcpy(nome, usuarios[u].nome);

            for (int l = 0; l < total_Livros; l++)
                if (livros[l].codigo == emprestimos[i].codigo_livro) strcpy(titulo, livros[l].titulo);

            printf("ATRASADO: '%s' com %s (Venceu: %02d/%02d/%04d)\n",
                   titulo, nome,
                   emprestimos[i].previsto.dia, emprestimos[i].previsto.mes, emprestimos[i].previsto.ano);
            count++;
        }
    }

    if (count == 0) printf("Nenhum atraso encontrado.\n");
    pausar();
}

void renovarEmprestimo() {
    limparTela();
    int cod;
    printf("=== Renovar Emprestimo ===\n");
    printf("Codigo do emprestimo: ");
    if (scanf("%d", &cod) != 1) {
        limparBuffer();
        printf("Invalido.\n");
        pausar();
        return;
    }
    limparBuffer();

    for (int i = 0; i < total_Emprestimos; i++) {
        if (emprestimos[i].codigo == cod && emprestimos[i].status == 1) {
            if (compararDatas(dataAtual(), emprestimos[i].previsto) > 0) {
                printf("Nao e possivel renovar: Livro ja esta atrasado.\n");
            } else {
                adicionarDias(&emprestimos[i].previsto, 7);
                salvarEmprestimos();
                printf("Renovado! Nova data: %02d/%02d/%04d\n",
                       emprestimos[i].previsto.dia, emprestimos[i].previsto.mes, emprestimos[i].previsto.ano);
            }
            pausar();
            return;
        }
    }
    printf("Emprestimo nao encontrado ou inativo.\n");
    pausar();
}

void menuPrincipal() {
    int op;
    do {
        limparTela();
        printf("=== Biblioteca ===\n");
        printf("1. Cadastrar Livro\n2. Cadastrar Usuario\n3. Realizar Emprestimo\n");
        printf("4. Realizar Devolucao\n5. Pesquisar Livro\n6. Pesquisar Usuario\n");
        printf("7. Listar Emprestimos Ativos\n8. Top Livros Emprestados\n");
        printf("9. Relatorio de Atrasos\n10. Renovar Emprestimo\n0. Sair\n");
        printf("Opcao: ");

        if (scanf("%d", &op) != 1) {
            limparBuffer();
            op = -1;
        } else {
            limparBuffer();
        }

        switch (op) {
            case 1: cadastrarLivro(); break;
            case 2: cadastrarUsuario(); break;
            case 3: realizarEmprestimo(); break;
            case 4: realizarDevolucao(); break;
            case 5: pesquisarLivro(); break;
            case 6: pesquisarUsuario(); break;
            case 7: listarEmprestimosAtivos(); break;
            case 8: relatorioMaisEmprestados(); break;
            case 9: relatorioAtrasados(); break;
            case 10: renovarEmprestimo(); break;
            case 0: printf("Saindo...\n"); break;
            default: printf("Opcao invalida.\n"); pausar(); break;
        }
    } while (op != 0);
}

int main() {
    carregarLivros();
    carregarUsuarios();
    carregarEmprestimos();
    menuPrincipal();
    return 0;
}