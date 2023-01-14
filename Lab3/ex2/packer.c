#include <semaphore.h>
#include <stddef.h>
#include "packer.h"
#include <stdlib.h>
////////////////
int countR = 0;
int countG = 0;
int countB = 0;
int *idsR;
int *idsG;
int *idsB;
int N;
sem_t matchR;
sem_t matchG;
sem_t matchB;
sem_t mutexR;
sem_t mutexG;
sem_t mutexB;
////////////////

void packer_init(int balls_per_pack) {
  N = balls_per_pack;
  idsR = calloc(N, sizeof(int));
  idsG = calloc(N, sizeof(int));
  idsB = calloc(N, sizeof(int));
  sem_init(&matchR, 0, 1);
  sem_init(&matchG, 0, 1);
  sem_init(&matchB, 0, 1);
  sem_init(&mutexR, 0, 0);
  sem_init(&mutexG, 0, 0);
  sem_init(&mutexB, 0, 0);
}

void packer_destroy(void) {
  free(idsR);
  free(idsG);
  free(idsB);
  sem_destroy(&matchR);
  sem_destroy(&matchG);
  sem_destroy(&matchB);
  sem_destroy(&mutexR);
  sem_destroy(&mutexG);
  sem_destroy(&mutexB);
}

void populateId(int *ball_remaining, int ball_id, int *ball_ids, int *other_ids, sem_t *match, sem_t *mutex) {
  sem_wait(mutex);
  int other_idx = 0;
  for(int i = 0; i < N; i++) {
    if(ball_ids[i] == ball_id)
      continue;
    other_ids[other_idx] = ball_ids[i];
    other_idx++;
  }
  (*ball_remaining)--;
  if(*ball_remaining > 0) {
    sem_post(mutex);
  }
  else {
    sem_post(match);
  }
}

void pack_ball(int colour, int id, int *other_ids) {
  if(colour == 1) { //RED
    sem_wait(&matchR);
    idsR[countR] = id;
    countR++;
    if(countR < N) {
      sem_post(&matchR);
    }
    else {
      sem_post(&mutexR);
    }
    populateId(&countR, id, idsR, other_ids, &matchR, &mutexR);
  }
  else if(colour == 2) { //GREEN
    sem_wait(&matchG);
    idsG[countG] = id;
    countG++;
    if(countG < N) {
      sem_post(&matchG);
    }
    else {
      sem_post(&mutexG);
    }
    populateId(&countG, id, idsG, other_ids, &matchG, &mutexG);
  }
  else { //BLUE
    sem_wait(&matchB);
    idsB[countB] = id;
    countB++;
    if(countB < N) {
      sem_post(&matchB);
    }
    else {
      sem_post(&mutexB);
    }
    populateId(&countB, id, idsB, other_ids, &matchB, &mutexB);
  }
}
