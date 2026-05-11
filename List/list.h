#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct List List;
typedef struct Cell Cell;

typedef int type;


typedef struct List{
    Cell* begin;
    Cell* last;

    int size;
}List;

typedef struct Cell{
    Cell* next;
    type item;
}Cell;

void initializeList(List* list);
Cell* initializeCell(type item);
void append(List* list, type item);
int editContent(List* list, type target, type value);
int removeCell(List* list, Cell target);
Cell* getCell(List* list, type target);
void printCell(Cell cell);
void printList(List list);

#endif //LIST_H