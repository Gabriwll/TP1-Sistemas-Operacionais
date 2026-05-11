#include "./list.h"

void initializeList(List* list){ //Not validated
    list = (List*)malloc(sizeof(List));

    list->begin = NULL;
    list->last = NULL;

    list->size = 0;
}

Cell* initializeCell(type item){ //Not validated
    Cell* cell = (Cell*)malloc(sizeof(Cell));
    cell->item = item;

    cell->next = NULL;

    return cell;
}

void append(List* list, type item){ //Not validated
    Cell* newCell = initializeCell(item);
    Cell* aux = list->begin;

    if(list->begin == NULL){
        list->begin = newCell;
    }
    list->last = newCell;

    while(aux->next != NULL){
        aux = aux->next;
    }
    aux->next = newCell;

    list->size++;
}

int editContent(List* list, type target, type value){ //Not validated
    Cell* currentCell = list->begin;
    
    while(currentCell->item != target){
        if(!currentCell->next){
            return 0; //Cell not found
        }
        
        currentCell = currentCell->next;
    }

    currentCell->item = value;
    
    return 1; //Cell removed
}

int removeCell(List* list, Cell target){ //Not validated
    Cell* currentCell = list->begin;
    Cell* previousCell = NULL;
    
    while(currentCell != &target){
        if(!currentCell->next){
            return 0; //Cell not found
        }
        
        previousCell = currentCell;
        currentCell = currentCell->next;
    }
    
    if(!previousCell){
        list->begin = NULL;
    }else{
        previousCell->next = currentCell->next;
    }
    
    free(currentCell);
    list->size--;

    return 1;
}

int destroyList(List* list){
    for(int i = 0; i < list->size; i++){
        if(!removeCell(list, (*list->begin))) return 0;
    }

    free(list);
    return 1;
}

Cell* getCell(List* list, type target){ //Not validated
    Cell* currentCell = list->begin;

    while(currentCell->item == target){
        if(!currentCell->next){
            return 0; //Cell not found
        }

        currentCell = currentCell->next;
    }

    return currentCell;
}

void printCell(Cell cell){
    printf("Celulas.\n"); //Currently the list has a blank implementation of the type, so it doesn't make sense to implementate this function yet. It will only serve as unitary test;
}

void printList(List list){
    Cell* currentCell = list.begin;

    while(currentCell->next){
        printCell(*currentCell);
        currentCell = currentCell->next;
    }
}