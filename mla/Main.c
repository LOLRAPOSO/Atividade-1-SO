#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

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
    for(int i = 0; i < argc; i++){
        nova->argv[i] = argv[i];
    }
    nova->argc = argc;
    strncpy(nova->input, input, MAX);
    strncpy(nova->output, output, MAX);
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

int main(int argc, char *argv[]){ //////////////MAIN//////////////
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
        if(strcmp(token[0], "exit") == 0){
            break;
        }
        //PARSER (Despachador de outros comandos)
        //(task, run, input, output, append, workdir, start, jobs, wait)
    }
    if(commands != stdin){
        fclose(commands);
    }
}