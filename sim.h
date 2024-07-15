#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  int type;
  double clock;
  struct node* next;
} node;

node* createNode(int, double);
node* insertNode(node*, node*);
node* removeNode(node*, node**);
void printLists(node*);

node* createNode(int type, double clock) {
  node* nodePtr = (node*)malloc(sizeof(node));
  nodePtr->type = type;
  nodePtr->clock = clock;
  nodePtr->next = NULL;

  return nodePtr;
}

node* insertNode(node* head, node* newNodePtr) {
  if (head == NULL) {
    return newNodePtr;
  } else if (head->clock > newNodePtr->clock) {
    newNodePtr->next = head;
    return newNodePtr;
  } else {
    node* prev = NULL;
    node* curr = head;

    while ((curr != NULL) && (curr->clock <= newNodePtr->clock)) {
      prev = curr;
      curr = curr->next;
    }
    prev->next = newNodePtr;
    newNodePtr->next = curr;

    return head;
  }
}

node* removeNode(node* head, node** nextRef) {
  *nextRef = head;

  if (head != NULL) {
    head = head->next;
  }

  (*nextRef)->next = NULL;

  return head;
}

void printLists(node* head) {
  node* printNode = head;

  // 依序印出節點內容
  while (printNode != NULL) {
    printf("type: %d clock:%2f -->", printNode->type, printNode->clock);
    printNode = printNode->next;
  }
  printf("\n");
}
