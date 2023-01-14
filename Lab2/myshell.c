/**
 * CS2106 AY22/23 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */


#include "myshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

struct PCBTable* array_of_pcbs[MAX_PROCESSES];
int* curr;
int* num_exited;
int* num_running;
int* num_terminating;

void info_cmd(char** input) {
  if(input[1] == NULL) {
    fprintf(stderr, "Wrong command\n");
    return;
  }
  char* option = input[1];
  if(strcmp(option, "0") == 0) {
    for(int i = 0; i < MAX_PROCESSES; i++) {
      if(array_of_pcbs[i]) {
        printf("[%d] ", array_of_pcbs[i]->pid);
        if(array_of_pcbs[i]->status == 1) {
          printf("Exited %d\n", array_of_pcbs[i]->exitCode);
        }
        else if(array_of_pcbs[i]->status == 2) {
          printf("Running\n");
        }
        else if(array_of_pcbs[i]->status == 3) {
          printf("Terminating\n");
        }
      }
    }
  }
  else if(strcmp(option, "1") == 0) {
    printf("Total exited process: %d\n", *num_exited);
  }
  else if(strcmp(option, "2") == 0) {
    printf("Total running process: %d\n", *num_running);
  }
  else if(strcmp(option, "3") == 0) {
    printf("Total terminating process: %d\n", *num_terminating);
  }
  else {
    fprintf(stderr, "Wrong command\n");
  }
}

void wait_cmd(char** input) {
  char* id = input[1];
  int pid = atoi(id);
  for(int i = 0; i < MAX_PROCESSES; i++) {
    if(array_of_pcbs[i]) {
      if(array_of_pcbs[i]->pid == pid && array_of_pcbs[i]->status == 2) {
        int status;
        waitpid(array_of_pcbs[i]->pid, &status, 0);
        (*num_exited)++;
        (*num_running)--;
        array_of_pcbs[i]->status = 1;
        array_of_pcbs[i]->exitCode = WEXITSTATUS(status);
      }
    }
  }
  return;
}


void terminate_cmd(char** input) {
  char* id = input[1];
  int pid = atoi(id);
  for(int i = 0; i < MAX_PROCESSES; i++) {
    if(array_of_pcbs[i]) {
      if(array_of_pcbs[i]->pid == pid && array_of_pcbs[i]->status == 2) {
        array_of_pcbs[i]->status = 3;
        (*num_terminating)++;
        (*num_running)--;
        kill(array_of_pcbs[i]->pid, SIGTERM);
      }
    }
  }
  return;
}

void sig_handler() {
  pid_t pid;
  int status;
  while(1) {
    pid = waitpid(-1, &status, WNOHANG);
    if(pid <= 0) {
      break;
    }
    for(int i = 0; i < MAX_PROCESSES; i++) {
      if(array_of_pcbs[i]->pid == pid) {
        if(array_of_pcbs[i]->status == 2) {
          (*num_running)--;
          array_of_pcbs[i]->exitCode = WEXITSTATUS(status);
        }
        else if(array_of_pcbs[i]->status == 3) {
          (*num_terminating)--;
          array_of_pcbs[i]->exitCode = WTERMSIG(status);
        }
        array_of_pcbs[i]->status = 1;
        (*num_exited)++;
        break;
      }
    }
  }
}

void my_init(void) {

  // Initialize what you need here
  signal(SIGCHLD, sig_handler);
  *array_of_pcbs = calloc(MAX_PROCESSES, sizeof(struct PCBTable));
  curr = malloc(sizeof(int));
  *curr = 0;
  num_exited = malloc(sizeof(int));
  *num_exited = 0;
  num_running = malloc(sizeof(int));
  *num_running = 0;
  num_terminating = malloc(sizeof(int));
  *num_terminating = 0;
  for(int i = 0; i < MAX_PROCESSES; i++) {
    array_of_pcbs[i] = NULL;
  }
  return;
}



void my_process_command(size_t num_tokens, char **tokens) {

  // Your code here, refer to the lab document for a description of the
  // arguments
  char** input = tokens;
  size_t length = num_tokens;
  for(size_t i = 0; i < num_tokens; i++) {
    if(tokens[i]) {
      if(strcmp(tokens[i], ";") == 0) {
        length = i + 1;
        break;
      }
    }
  }
  tokens = &tokens[length]; //new subarray of tokens for later call
  input[length-1] = NULL; //remove symbol
  num_tokens -= length; //remove curr length

  char* arg0 = input[0];
  if(strcmp(arg0, "info") == 0) {
    info_cmd(input);
    return;
  }
  if(strcmp(arg0, "wait") == 0) {
    wait_cmd(input);
    return;
  }
  if(strcmp(arg0, "terminate") == 0) {
    terminate_cmd(input);
    return;
  }
  if(strcmp(arg0, "quit") == 0) {
    my_quit();
    return;
  }

  int acc_valid = access(arg0, F_OK);
  if(acc_valid == -1) {
    fprintf(stderr,"%s not found\n", arg0);
    return;
  }
  int file_in = -10; //dummy
  int file_out = -10; //dummy
  int file_err = -10; //dummy
  for(size_t i = 0; i < length; i++) {
    if(input[i]) {
      if(strcmp(input[i], "<") == 0) {
        file_in = (int) i + 1;
      }
      else if(strcmp(input[i], ">") == 0) {
        file_out =  (int) i + 1;
      }
      else if(strcmp(input[i], "2>") == 0) {
        file_err = (int) i + 1;
      }
    }
  }

  char* arg_last = input[length-2];
  if(strcmp(arg_last, "&") == 0) {
    input[length-2] = NULL;
  }
  int curr_proc = *curr;
  pid_t child_pid = fork();
  if(child_pid != 0) { //we are in parent process
    (*num_running)++;
    struct PCBTable* pcb = malloc(sizeof(struct PCBTable));
    pcb->pid = child_pid;
    pcb->status = 2;
    pcb->exitCode = -1;
    array_of_pcbs[curr_proc] = pcb;
    (*curr)++;
    if(strcmp(arg_last, "&") == 0) {
      printf("Child [%d] in background\n", child_pid);
    }
    else {
      int status;
      waitpid(child_pid, &status, 0);
      (*num_exited)++;
      (*num_running)--;
      array_of_pcbs[curr_proc]->status = 1;
      array_of_pcbs[curr_proc]->exitCode = WEXITSTATUS(status);
    }
  }
  else { //we are in child process
    if(file_in != -10) {
      int file_desc = open(input[file_in], O_RDONLY);
      if(file_desc == -1) {
        fprintf(stderr, "%s does not exist\n", input[file_in]);
        exit(1);
      }
      dup2(file_desc, STDIN_FILENO);
      close(file_desc);
      input[file_in-1] = NULL; //remove symbol from args
    }
    if(file_out != -10) {
      int file_desc = open(input[file_out], O_CREAT | O_WRONLY | O_TRUNC, 0666);
      dup2(file_desc, STDOUT_FILENO);
      close(file_desc);
      input[file_out-1] = NULL; //remove symbol from args
    }
    if(file_err != -10) {
      int file_desc = open(input[file_err], O_CREAT | O_WRONLY | O_TRUNC, 0666);
      dup2(file_desc, STDERR_FILENO);
      close(file_desc);
      input[file_err-1] = NULL; //remove symbol from args
    }
    execv(input[0], input);
  }
  if(num_tokens > 0) {
    my_process_command(num_tokens, tokens);
  }
  return;
}



void my_quit(void) {

  // Clean up function, called after "quit" is entered as a user command
  for(int i = 0; i < MAX_PROCESSES; i++) {
    if(array_of_pcbs[i]) {
      if(array_of_pcbs[i]->status == 2) {
        printf("Killing [%d]\n", array_of_pcbs[i]->pid);
        kill(array_of_pcbs[i]->pid, SIGTERM);
      }
    }
  }
  free(*array_of_pcbs);
  free(curr);
  free(num_exited);
  free(num_running);
  free(num_terminating);

  printf("\nGoodbye\n");
}

