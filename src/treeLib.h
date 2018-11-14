#ifndef TREE_H
#define TREE_H

#include <time.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KBLU "\x1B[34m"
#define KGRY "\e[90m"

struct Node;
typedef struct Node Process;

struct Node {
	char *name;				// Il nome del processo
	int pid;				// Il PID
	int ppid;				// Il PPID
	int clone_level;		// Il no. di cloni generati da quel processo, parte da 0
	int state;				// Indica lo stato del processo (attivo/chiuso)
	int no_childs;			// Numero di figli generati attualmente attivi

	time_t creation;		// Data di creazione del processo
	time_t termination;		// Data di terminazione del processo

	Process *parent;		// Riferimento al padre
	Process *child;			// Riferimento al figlio
	Process *left_sibling;	// Riferimento al fratello SX
	Process *sibling;		// Riferimento al fratello DX
};

int init(Process **root, int pid);
int newProcess(Process **process, char *name, int pid, int ppid);

void printProcess(Process *process);
void processInfo(Process *process);
void printProcessTree(Process *process, int level, int active);
void printProcessList(Process *process, int active);

int insertChild(Process *process, char *name, int pid);
int removeProcess(Process *process);
int removeProcessTree(Process *process);
int deleteProcess(Process **process);
int deleteProcessTree(Process **process);
int findPid(Process *process_tree, char *name, int active);

int searchNamesakes(Process *process, char *name, int counter);
Process* searchByPid(Process *process, int pid);
Process* searchByName(Process *process, char *name);

#endif
