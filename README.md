# ProcessFlow

Orquestrador de processos desenvolvido para a disciplina de Infraestrutura de Software (CESAR School).
O ProcessFlow cadastra tarefas que representam programas do sistema e as executa por meio de processos
filhos, criados e gerenciados diretamente com `fork()`, `exec()` e `wait()`/`waitpid()`.

## Arquivos do projeto

| Arquivo            | Responsabilidade                                                             |
|---------------------|-------------------------------------------------------------------------------|
| `Main.c`            | Código-fonte completo: parser de comandos, cadastro e execução de tarefas    |
| `Makefile`          | Compilação, limpeza e teste automático                                      |
| `README.md`         | Este arquivo                                                                  |
| `evidencias.log`    | Log de comandos e saídas do terminal durante o desenvolvimento (gerado com `script -a`) |

## Sistema operacional utilizado

<!-- Preencha com o SO real usado no desenvolvimento, por exemplo: -->
Desenvolvido e testado em **[Linux Ubuntu 22.04 / macOS 14 / WSL Ubuntu — ajustar conforme o seu ambiente]**.
Utiliza apenas chamadas de sistema POSIX padrão (`fork`, `execvp`, `waitpid`, `pipe`, `dup2`), portanto
deve ser compatível com qualquer sistema Linux, Unix ou macOS.

## Como compilar

```bash
make clean
make
```

Isso gera o executável `processflow` na raiz do projeto.

## Como executar

**Modo interativo** (sem argumentos):

```bash
./processflow
```

O programa apresenta o prompt `processflow>`, aguardando comandos digitados manualmente.

**Modo workflow** (lendo comandos de um arquivo `.pf`):

```bash
./processflow arquivo.pf
```

Nesse modo, cada linha lida do arquivo é impressa antes de ser processada, e o prompt não é exibido.

Em ambos os modos, o programa é finalizado com o comando `exit` (ou `CTRL-D` no modo interativo).

## Como testar

```bash
make test
```

Executa um teste básico automatizado (cadastro e execução de uma tarefa simples). Testes adicionais podem
ser feitos manualmente digitando comandos no modo interativo ou criando arquivos `.pf` de exemplo.

## Comandos suportados

| Comando                                   | Descrição                                                          | Status |
|--------------------------------------------|----------------------------------------------------------------------|--------|
| `task <nome> <programa> [args...]`         | Cadastra uma tarefa                                                  | ✅ Implementado |
| `run <nome>`                               | Executa uma tarefa cadastrada                                        | ✅ Implementado |
| `run sequential <t1> <t2> ...`             | Executa tarefas em sequência, uma após o término da anterior         | ✅ Implementado |
| `run parallel <t1> <t2> ...`               | Executa tarefas em paralelo, iniciando todas antes de esperar        | ✅ Implementado |
| `run pipe <t1> <t2> ...`                   | Encadeia a saída de uma tarefa como entrada da próxima               | ⏳ Pendente |
| `input <nome> <arquivo>`                   | Redireciona a entrada da tarefa a partir de um arquivo               | ⏳ Pendente |
| `output <nome> <arquivo>`                  | Redireciona a saída da tarefa para um arquivo (sobrescreve)          | ⏳ Pendente |
| `append <nome> <arquivo>`                  | Redireciona a saída da tarefa para um arquivo (acrescenta)           | ⏳ Pendente |
| `workdir <diretório>`                      | Altera o diretório de trabalho usado pelas tarefas seguintes         | ⏳ Pendente |
| `start <nome>`                             | Inicia uma tarefa em background, sem bloquear o prompt               | ⏳ Pendente |
| `jobs`                                     | Lista os jobs em execução em background                              | ⏳ Pendente |
| `wait <jobId>`                             | Aguarda o término de um job específico                               | ⏳ Pendente |
| `exit`                                     | Encerra o ProcessFlow                                                | ✅ Implementado |

## Tratamento de erros implementado

- Número incorreto de argumentos ao iniciar o ProcessFlow (encerra a execução)
- Arquivo `.pf` inexistente ou que não pode ser aberto (encerra a execução)
- Comando desconhecido no prompt (mensagem de erro, continua a execução)
- Tarefa não encontrada em `run` (mensagem de erro, continua a execução)
- Programa associado à tarefa inexistente ou não executável (mensagem de erro no processo filho, continua a execução)
- Linha de comando vazia (ignorada, sem erro)
- Processo terminando com código de saída diferente de zero (aviso, sem interromper o fluxo)

## Exemplo de uso

```
processflow> task listar /bin/ls -l
Tarefa 'listar' cadastrada.
processflow> task contar /usr/bin/wc -l
Tarefa 'contar' cadastrada.
processflow> run listar
processflow> run sequential listar contar
processflow> run parallel listar contar
processflow> exit
```

## Autor

[Matheus Lima Ramalho] — [mlr2]@cesar.school
