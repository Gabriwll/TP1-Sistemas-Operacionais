#ifndef PROCESS_H
#define PROCESS_H

#define STATE int
#define EXECUTION    0
#define BLOCKED      1
#define READY        2

typedef struct CPU;
typedef struct ProcessTable;
typedef struct ProcessManager;

typedef struct ExecutionState;
typedef struct ReadyState;
typedef struct BlockedState;

typedef struct CPU{ //Used to simulate a process execution 
    int* process; //Pointer to the currently executing process
    int programCounter;

    int structMem; //Simulate the intern memory of the CPU
    int quantumLimit; //In ms (maybe)
}CPU;

typedef struct ProcessTable{
    int processID;
    int fatherProcessID;

    int processCounterPointer;

    int priority; //Priority of the process execution (1: Very Important, 2: Important, etc).
    int quantQuantum; //Number of quantums already 
    int structMem; //dataStructure Vector. What I undertood is that this attribute will be a simulation of the primary memory, who will store the instruction of each process as long the ProcessTable will be 
                   //instantiated as a list.
                   //FIXME: Evaluate if int is the most fitable type of this variable
    STATE processState;
    int beginTime;
    int time;
}ProcessTable;
/*TODO: The ideal implementation of the PT may be a List, spectially thinking about the dinamic variation of size. In other hand, the N complexity
* of this data structure will not interfere in the funtionallity of this simulator, knowing that the CPU must navigate throught the Processes in
* arrive order while doing the context switch.
* In Conclusiont, an array-like Procces Table implementation may be way more complex than simply adapt a implementation of a list foe the context of
* this variable.
*/

typedef struct ProcessManager{
    int time;
    CPU cpu;  //Used to simulate the process execution that is in execution state
    ProcessTable processTable;
}ProcessManager;

typedef struct ExecutionState{
    int* executionProcessesIDs; //List of IDs from the currently execution process
    //TODO: implement this attribute as a list
}ExecutionState;

typedef struct ReadyState{
    int* readyProcessesIDs; //List of IDs from every ready processes
    //TODO: implement this attribute as a list
}ReadyState;

typedef struct BlockedState{
    int* blockedProcessesIDs; //List of IDs from every blocked processes
    //TODO: implement this attribute as a list
}BlockedState;


void initializeProcessManager(ProcessManager* processManager);
void initializeProcessTable(ProcessTable* processTable);

#endif //PROCESS_H