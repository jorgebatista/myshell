#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char linha[4096];
char* palavras[100];

// Iniciar processo
void m_start() {
  long pid;

  pid = fork();
  if(pid < 0) {
    perror ("Erro");
    exit(-1);
  }
 
  else { 
    if(pid > 0)
      printf("myshell: processo %ld iniciado.\n", pid);
    else {
      if(execvp(palavras[0], palavras) == -1)
        perror("Erro");
      exit(0);
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////
void m_wait(long pid) {
  int status;

  pid = waitpid(pid, &status, 0);
  if(pid < 0)
    printf("myshell: não há processos restantes.\n");
  else if(WIFEXITED(status))
    printf("myshell: processo %ld finalizou normalmente com status %d.\n", pid, WEXITSTATUS(status));
  else if(WIFSIGNALED(status))
    printf("myshell: processo %ld finalizou de forma anormal com sinal %d: %s.\n", pid, WTERMSIG(status), strsignal(WTERMSIG(status)));
}
////////////////////////////////////////////////////////////////////////////////////////
void m_run() {
  long pid;

  pid = fork();
  if(pid < 0) {
    perror ("Erro");
    exit(-1);
  }
 
  else { 
    if(pid > 0) {
      printf("myshell: processo %ld iniciado.\n", pid);
      m_wait(pid);
    }
    else {
      if(execvp(palavras[0], palavras) == -1)
        perror("Erro");
      exit(0);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////
int tratamento() {
  long pid;

  if(palavras[0] == 0)
    printf("myshell: ERRO --> %s: faltando argumento.\n", linha);
  else {
    pid = atoi(palavras[0]);
    if(pid == 0)
      printf("myshell: ERRO --> %s: argumento deve ser um inteiro maior que 0.\n", linha);
    else if(kill(pid, 0))
      printf("myshell: ERRO --> %s: processo %ld não encontrado.\n", linha, pid);
    else
      return 1;
  }
  return 0;
}
////////////////////////////////////////////////////////////////////////////////////////
void m_kill() {
  long pid;

  if(tratamento()) {
    pid = atoi(palavras[0]);
    kill(pid, SIGKILL);
    m_wait(pid);
  }
}
////////////////////////////////////////////////////////////////////////////////////////
void m_stop() {
  long pid;

  if(tratamento()) {
    pid = atoi(palavras[0]);
    kill(pid, SIGSTOP);
    printf("myshell: processo %ld parou a execução.\n", pid);
  }
}
////////////////////////////////////////////////////////////////////////////////////////
void m_continue() {
  long pid;

  if(tratamento()) {
    pid = atoi(palavras[0]);
    kill(pid, SIGCONT);
    printf("myshell: processo %ld voltou a execução.\n", pid);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
int main() {

  int i;

  while(1) {
    printf("myshell> ");
    fflush(stdout);
    if(fgets(linha, 4096, stdin) == 0) {
      printf("\n");
      return 0;
    }

    strtok(linha, " \t\n");
    palavras[0] = strtok(0, " \t\n");
    i = 0;
    while(palavras[i] != 0 && i < 100) {
      i++;
      palavras[i] = strtok(0, " \t\n");
    }

    if(strcmp(linha, "quit") == 0 || strcmp(linha, "exit") == 0)
      return 0;
    else if(strcmp(linha, "start") == 0)
      m_start();
    else if(strcmp(linha, "wait") == 0)
      m_wait(-1);
    else if(strcmp(linha, "run") == 0)
      m_run();
    else if(strcmp(linha, "kill") == 0)
      m_kill();
    else if(strcmp(linha, "stop") == 0)
      m_stop();
    else if(strcmp(linha, "continue") == 0)
      m_continue();
    else if(strcmp(linha, "\n") == 0)
      ;
    else
      printf("myshell: comando desconhecido: %s\n", linha);
  }

  return 0;
}
