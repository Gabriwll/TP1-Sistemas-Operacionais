#include "./list.h"

List initializeList(){
    List* list = (List*)malloc(sizeof(List));

    list->begin = NULL;
    list->last = NULL;

    list->size = 0;

    return *list;
}

Cell* initializeCell(type item){
    Cell* cell = (Cell*)malloc(sizeof(Cell));
    cell->item = item;

    cell->next = NULL;

    return cell;
}

Cell* getCell(List* list, type target){
    Cell* currentCell = list->begin;

    while(currentCell->item != target){
        if(!currentCell->next){
            return 0; //Cell not found
        }

        currentCell = currentCell->next;
    }

    return currentCell;
}

void append(List* list, type item){
    Cell* newCell = initializeCell(item);
    newCell->next = NULL;
    
    if(list->begin == NULL){
        list->begin = newCell;
        list->last = newCell;
    }else{
        list->last->next = newCell; 
        list->last = newCell;
    }

    list->size++;
}

int editContent(List* list, type target, type value){
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

int removeCell(List* list, Cell* target){
    Cell* currentCell = list->begin;
    Cell* previousCell = NULL;

    if(!list->begin) return 0; //Empty list

    while(currentCell != target && currentCell){        
        previousCell = currentCell;
        currentCell = currentCell->next;
    }
    if(!currentCell) return 0; //Not found

    if(!previousCell){
        list->begin = currentCell->next;
    }else{
        previousCell->next = currentCell->next;
    }

    if(currentCell == list->last){
        list->last = previousCell;
    }
    
    free(currentCell);
    list->size--;

    return 1;
}

void destroyList(List* list){ //Not validated
    if(!list) return;

    for(int i = 0; i < list->size; i++){
        removeCell(list, list->begin);
    }

    free(list);
}

void printCell(Cell cell){
    printf("%d\n", cell.item); //Currently the list has a blank implementation of the type, so it doesn't make sense to implementate this function yet. It will only serve as unitary test;
}

void printList(List list){
    Cell* currentCell = list.begin;

    while(currentCell){
        printCell(*currentCell);
        currentCell = currentCell->next;
    }
}

//Unitary Test for list TAD. To check the results, remove the comment below
/*
int main(){
    List list = initializeList();
    //test initialize list

    if(!list.begin){
        printf("Inicializacao concluida.\n");
    }
    
    //test append
    printf("Testando funcao append().\n");

    for(int i = 0; i < 10; i++){
        type item = i + 1;
        
        append(&list, item);
    }
    printList(list);
    printf("list size: %d\n\n", list.size);

    //test editContent

    printf("Testando funcao editContent().\n");
    editContent(&list, 2, 3);
    printList(list);

    //test removeCell
    printf("Testando funcao removeCell().\n");
    removeCell(&list, getCell(&list, 4));
    printList(list);
    printf("list size: %d\n\n", list.size);
    
    removeCell(&list, list.last);
    printList(list);
    printf("list size: %d\n", list.size);

    //test destroyList
    printf("Testando funcao destroyList().\n");
    destroyList(&list);

    return 0;
}
*/