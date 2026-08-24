#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>    
#include <fcntl.h>
#include <stdbool.h>

#define MAX 256
#define MAX_COMMANDS 1024
#define TAM_NOME 64
#define MAX_TAREFAS 100

typedef struct tarefa { 
    char nome[TAM_NOME];
    char *argv[64];
    int argc;
    char input[MAX];
    char output[MAX];
    struct tarefa *next;
} Tarefa;

typedef struct lista{
    Tarefa *inic;
} Lista;

//////////////////GUARDAR TAREFA////////////////////////
Tarefa* registrar_tarefa(char *nome, char *argv[], int argc, char *input, char *output){
    Tarefa *nova = (Tarefa*)malloc(sizeof(Tarefa));

    strncpy(nova->nome, nome, TAM_NOME);
    nova->nome[TAM_NOME - 1] = '\0';

    for(int i = 0; i < argc; i++){
        nova->argv[i] = strdup(argv[i]);
    }
    nova->argv[argc] = NULL;
    nova->argc = argc;

    strncpy(nova->input, input, MAX - 1);
    nova->input[MAX - 1] = '\0';

    strncpy(nova->output, output, MAX - 1);
    nova->output[MAX - 1] = '\0';
    nova->next = NULL;

    return nova;    
}

void guardar_tarefa(Lista* p, char *nome, char *argv[], int argc, char *input, char *output){
    Tarefa *nova = registrar_tarefa(nome, argv, argc, input, output);

    if(p->inic == NULL){
        p->inic = nova;
        return;
    }

    nova->next = p->inic;
    p->inic = nova;
}
//////////////////GUARDAR TAREFA////////////////////////

Tarefa* achar_tarefa(Lista* p, char* nome){
    Tarefa* atual = p->inic;

    while(atual != NULL){
        if(strcmp(atual->nome, nome) == 0){
            return atual;
        }
        atual = atual->next;
    }
    return NULL;
}

int executar_tarefa(Tarefa *t){
    if(t == NULL){
        return -1;
    }

    pid_t pid = fork();

    if(pid < 0){
        printf("Erro: falha ao criar processo para '%s'.\n", t->nome);
        return -1;
    }

    if(pid == 0){
        execvp(t->argv[0], t->argv);
        fprintf(stderr, "Erro: nao foi possivel executar '%s'.\n", t->argv[0]);
        exit(1);
    }

    int status;
    waitpid(pid, &status, 0);

    int terminou_normal = (status & 0x7F) == 0;

    if(terminou_normal){
        int codigo = (status >> 8) & 0xFF;
        if(codigo != 0){
            printf("Aviso: tarefa '%s' terminou com codigo %d.\n", t->nome, codigo);
        }
        return codigo;
    } else {
        int sinal = status & 0x7F;
        printf("Aviso: tarefa '%s' foi encerrada pelo sinal %d.\n", t->nome, sinal);
        return -1;
    }
}
/////////////////TAREFAS///////////////////////////////
void cmd_task(Lista *p, char *token[], int tok){
    
    if(tok < 3){
        printf("Erro: uso correto e' task <nome> <programa> [argumentos...]\n");
        return;
    }

    char *nome = token[1];

    if(achar_tarefa(p, nome) != NULL){
        printf("Erro: tarefa '%s' ja foi cadastrada.\n", nome);
        return;
    }
    
    char *exec_argv[64];
    int exec_argc = 0;
    for(int i = 2; i < tok; i++){
        exec_argv[exec_argc++] = token[i];
    }

    guardar_tarefa(p, nome, exec_argv, exec_argc, "", "");
    printf("Tarefa '%s' cadastrada.\n", nome);
}

void run_sequential(Lista *p, char *token[], int tok){

    if(tok < 3){
        printf("Erro: uso correto e' run sequential <tarefa1> <tarefa2> ...\n");
        return;
    }

    for(int i = 2; i < tok; i++){
        Tarefa *t = achar_tarefa(p, token[i]);

        if(t == NULL){
            printf("Erro: tarefa '%s' nao encontrada.\n", token[i]);
            continue; 
        }

        executar_tarefa(t);
    }
}

void run_parallel(Lista *p, char *token[], int tok){

    if(tok < 3){
        printf("Erro: uso correto e' run parallel <tarefa1> <tarefa2> ...\n");
        return;
    }

    pid_t pids[64];
    char *nomes[64];  
    int total = 0;

    for(int i = 2; i < tok && total < 64; i++){
        Tarefa *t = achar_tarefa(p, token[i]);

        if(t == NULL){
            printf("Erro: tarefa '%s' nao encontrada.\n", token[i]);
            continue;
        }

        pid_t pid = fork();

        if(pid < 0){
            printf("Erro: falha ao criar processo para '%s'.\n", t->nome);
            continue;
        }

        if(pid == 0){
            execvp(t->argv[0], t->argv);
            fprintf(stderr, "Erro: nao foi possivel executar '%s'.\n", t->argv[0]);
            exit(1);
        }

        pids[total] = pid;
        nomes[total] = t->nome;
        total++;
    }

    for(int i = 0; i < total; i++){
        int status;
        waitpid(pids[i], &status, 0);

        int terminou_normal = (status & 0x7F) == 0;

        if(terminou_normal){
            int codigo = (status >> 8) & 0xFF;
            if(codigo != 0){
                printf("Aviso: tarefa '%s' terminou com codigo %d.\n", nomes[i], codigo);
            }
        } else {
            int sinal = status & 0x7F;
            printf("Aviso: tarefa '%s' foi encerrada pelo sinal %d.\n", nomes[i], sinal);
        }
    }
}
/////////////////TAREFAS///////////////////////////////
int tokenizar(char *linha, char *tokens[], int max_tokens) {
    int count = 0;
    char *p = linha;

    while (*p != '\0' && count < max_tokens) {
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        tokens[count] = p;
        count++;

        while (*p != '\0' && *p != ' ' && *p != '\t') {
            p++;
        }

        if (*p != '\0') {
            *p = '\0';
            p++;
        }
    }
    return count;
}

void cmd_run(Lista *p, char *token[], int tok){
    // processflow> run <nome>
    // (sequential/parallel/pipe entram depois, quando token[1] for essas palavras)

    if(tok < 2){
        printf("Erro: uso correto e' run <nome>\n");
        return;
    }

    Tarefa *t = achar_tarefa(p, token[1]);

    if(t == NULL){
        printf("Erro: tarefa '%s' nao encontrada.\n", token[1]);
        return;
    }

    executar_tarefa(t);
}
/////////////////TAREFAS///////////////////////////////

int main(int argc, char *argv[]){ //////////////MAIN//////////////
    Lista tarefas = {NULL}; //POR NÃO SER UM PONTEIRO TEM QUE ADICIONAR ISSO "{}"!!

    FILE *commands = NULL;
    char linhas[MAX_COMMANDS];
    char *token[64];
    bool workflow = (argc == 2);

    if(argc > 2){
        printf("Ultrapassou número de elementos.");
        return 1;
    }

    if(workflow){
        commands = fopen(argv[1], "r");
        if(commands == NULL){
            printf("Erro ao abrir o arquivo.");
            return 1;
        }
    } else {
        commands = stdin;
    }

    while(1){
        if (!workflow) {
            printf("processflow> ");
            fflush(stdout);
        }

        if (fgets(linhas, MAX_COMMANDS, commands) == NULL) {
            break; 
        }

        linhas[strcspn(linhas, "\n")] = '\0';

        if (workflow) {
            printf("%s\n", linhas);
        }
        
        int tok = tokenizar(linhas, token, 64);
        if(tok == 0){
            continue;
        }

        if(strcmp(token[0], "exit") == 0){
            break;
        }

        if(strcmp(token[0], "task") == 0){ 
            cmd_task(&tarefas, token, tok);
        }
        else if(strcmp(token[0], "run") == 0){
            cmd_run(&tarefas, token, tok);
        }
        else {
            printf("\nErro: comando desconhecido '%s'\n", token[0]);
        }
        //PARSER (Despachador de outros comandos)
        //(task, run, input, output, append, workdir, start, jobs, wait)
    }
    if(commands != stdin){
        fclose(commands);
    }
    return 0;
}