#include "./threadsList.h"

List initializeList() {
    List list;

    list.begin = NULL;
    list.last = NULL;
    list.size = 0;

    return list;
}

Cell* initializeCell(type item){
    Cell* cell = (Cell*)malloc(sizeof(Cell));
    cell->item = item;

    cell->next = NULL;

    return cell;
}

Cell* getCell(List* list, type target){
    Cell* currentCell = list->begin;

    while(currentCell && currentCell->item != target){
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

/*como vai funcionar, vai percorrer a lista de threads até achar a posição dada, e realocar as threads ja existentes
e inserir a nova na posição certa*/

void insertIntoPos(List* list, type item, int pos){ 
    Cell * newCell = initializeCell(item);// cria a nova cell já com o item
    Cell* current = list->begin;
    Cell* previous = NULL;

    for(int i = 0; i < pos && current; i++){
        previous = current;
        current = currentt -> next;
    }
    
    //encaixa a nova cell entre previous  e current
    newCell->next = current;

    if(!previous){
        list->begin = newCell;//insere no inicio
    }else{
        previous->next = newCell;
    }

    if(!current) list->last = newCell;//insere final
    list->size++;

    return newCell;
}

int editContent(List* list, type target, type value){
    Cell* currentCell = list->begin;

    while(currentCell && currentCell->item != target){
        currentCell = currentCell->next;
    }

    if(!currentCell)
        return 0;

    currentCell->item = value;

    return 1;
}

int removeCell(List* list, Cell* target){
    Cell* currentCell = list->begin;
    Cell* previousCell = NULL;

    if(!list->begin) return 0; //Empty list

    while(currentCell && currentCell != target){        
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

void destroyList(List* list){
    if(!list)
        return;

    while(list->begin){
        removeCell(list, list->begin);
    }
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