/*************************************
 * Lab 1 Exercise 3
 * Name: Ng Sihan, Ian
 * Student No: A0231053X
 * Lab Group: B09
 *************************************/

#include "node.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Add in your implementation below to the respective functions
// Feel free to add any headers you deem fit (although you do not need to)


// Traverses list and returns the sum of the data values
// of every node in the list.
long sum_list(list *lst) {
  if(lst->head == NULL){ //list is empty
    return 0;
  }
  node* curr = lst->head;
  long total = 0;
  while(curr != NULL){
    total += curr->data;
    curr = curr->next;
  }
  return total;
}

// Traverses list and returns the number of data values in the list.
int list_len(list *lst) {
  if(lst->head == NULL){
    return 0;
  }
  node* curr = lst->head;
  int total = 0;
  while(curr != NULL){
    total++;
    curr = curr->next;
  }
  return total;
}

// Inserts a new node with data value at index (counting from head
// starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) {
  node* new_node = malloc(sizeof(node));
  new_node->data = data;
  new_node->next = NULL;
  if(index == 0){
    new_node->next = lst->head;
    lst->head = new_node;
  }
  else{
    int idx = 0;
    node* curr = lst->head;
    while(idx != index-1){
      curr = curr->next;
      idx++;
    }
    node* temp = curr->next;
    curr->next = new_node;
    new_node->next = temp;
  }
}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.

void delete_node_at(list *lst, int index) {
  if(index == 0){
    if((lst->head)->next == NULL){
      free(lst->head);
      lst->head = NULL;
    }
    else{
      node* temp = lst->head;
      lst->head = temp->next;
      free(temp);
    }
  }
  else{
    int idx = 0;
    node* curr = lst->head;
    while(idx != index-1){
      curr = curr->next;
      idx++;
    }
    node* temp = curr->next;
    curr->next = temp->next;
    free(temp);
  }
}

// Search list by the given element.
// If element not present, return -1 else return the index. If lst is empty return -2.

int search_list(list *lst, int element) {
  if(lst->head == NULL){ //list is empty
    return -2;
  }
  node* curr = lst->head;
  int index = 0;
  while(curr != NULL){
    if(curr->data == element){
      return index;
    }
    curr = curr->next;
    index++;
  }
  return -1;
}

// Reverses the list with the last node becoming the first node.
void reverse_list(list *lst) {
  node* prev = NULL;
  node* next = NULL;
  node* curr = lst->head;
  while(curr != NULL){
    next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
  }
  lst->head = prev;
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
  while(lst->head != NULL){
    node* curr = lst->head;
    lst->head = (lst->head)->next;
    free(curr);
  }
}

// Traverses list and applies func on data values of
// all elements in the list.
void map(list *lst, int (*func)(int)) {
  if(lst->head == NULL){ //list is empty
    return;
  }
  node* curr = lst->head;
  while(curr != NULL){
    curr->data = func(curr->data);
    curr = curr->next;
  }
}
