#include "packer.h"
#include <semaphore.h>

// You can declare global variables here
int countR = 0;
int countG = 0;
int countB = 0;
int idsR[2];
int idsG[2];
int idsB[2];
int N = 2;
sem_t matchR;
sem_t matchG;
sem_t matchB;
sem_t mutexR;
sem_t mutexG;
sem_t mutexB;
/////////////////

void packer_init(void) {
  // Write initialization code here (called once at the start of the program).
  sem_init(&matchR, 0, 1);
  sem_init(&matchG, 0, 1);
  sem_init(&matchB, 0, 1);
  sem_init(&mutexR, 0, 0);
  sem_init(&mutexG, 0, 0);
  sem_init(&mutexB, 0, 0);
}

void packer_destroy(void) {
  // Write deinitialization code here (called once at the end of the program).
  sem_destroy(&matchR);
  sem_destroy(&matchG);
  sem_destroy(&matchB);
  sem_destroy(&mutexR);
  sem_destroy(&mutexG);
  sem_destroy(&mutexB);
}

int retrieveId(int *ball_remaining, int ball_id, int *ball_ids, sem_t *match, sem_t *mutex) {
  sem_wait(mutex);
  int matching;
  if(ball_ids[0] != ball_id) {
    matching = ball_ids[0];
  }
  else {
    matching = ball_ids[1];
  }
  (*ball_remaining)--;
  if(*ball_remaining > 0) {
    sem_post(mutex);
  }
  else {
    sem_post(match);
  }
  return matching;
}

int pack_ball(int colour, int id) {
  // Write your code here.
  int matching;
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
    matching = retrieveId(&countR, id, idsR, &matchR, &mutexR);
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
    matching = retrieveId(&countG, id, idsG, &matchG, &mutexG);
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
    matching = retrieveId(&countB, id, idsB, &matchB, &mutexB);
  }
  return matching;
}
