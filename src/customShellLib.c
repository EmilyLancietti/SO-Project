#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "customShellLib.h"
#include "treeLib.h"

/**
 * STAMPA A VIDEO DEL MANUALE DEI COMANDI
 * attraverso una system call
 */
void p_help() {
	system("man -l assets/man.1");
}

/* ======================================================================================================= */

/**
 * STAMPA A VIDEO DI INFORMAZIONI DETTAGLIATE SUL PROCESSO
 * Dato l'albero dei processi stampa a video tutti i processi con nome "name"
 * consentendo all'utente di selezionale il Pid del processo di cui si vuole ottenere informazioni
 *
 * @param process: Albero/sottoalbero dei processi
 * @param name: Nome del processo cercato
 */
void p_info(Process *process_tree, char *name) {
	int pid;				// Pid del processo di cui ottenere informazioni
	Process *process;		// Riferimento al processo di cui ottenere informazioni

	// Non importa che il processo sia attivo
	printf("\n");
	pid = findPid(process_tree, name, 0);
	printf("%d\n", pid);

	// PID valido (Il processo esiste e ha nome 'name')
	if (pid != 0) {
		process = searchByPid(process_tree, pid);
		printf("Richiesta di informazioni sul processo '%s ---%d'...\n\n", name, pid);
		processInfo(process);
		printf("\n");
	}
}

/* ======================================================================================================= */

/**
 * STAMPA A VIDEO LA GERARCHIA DEI PROCESSI ATTIVI
 * Dato l'albero dei processi stampa a video la struttura gerarchica di tutti i processi
 * generati attualmente attivi
 *
 * @param process: Albero/sottoalbero dei processi
 */
void p_tree (Process *process_tree) {
	printf("Richiesta della gerarchia dei processi generati attivi...\n\n");
	printProcessTree(process_tree, 0, 0);
	printf("\n");
}

/* ======================================================================================================= */

/**
 * STAMPA A VIDEO LA GERARCHIA DEI PROCESSI (ATTIVI E NON)
 * Dato l'albero dei processi stampa a video la struttura gerarchica di tutti i processi generati
 *
 * @param process: Albero/sottoalbero dei processi
 */
void p_treeall(Process *process_tree) {
	printf("Richiesta della gerarchia dei processi generati...\n\n");
	printProcessTree(process_tree, 0, 1);
	printf("\n");
}

/* ======================================================================================================= */

/**
 * STAMPA A VIDEO LA LISTA DEI PROCESSI ATTIVI
 * Dato l'albero dei processi stampa a video la lista di tutti i processi
 * generati attualmente attivi
 *
 * @param process: Albero/sottoalbero dei processi
 */
void p_list(Process *process_tree) {
	printf("Richiesta della lista dei processi generati attivi...\n\n");
	printProcessList(process_tree, 0);
	printf("\n");
}

/* ======================================================================================================= */

/**
 * STAMPA A VIDEO LA LISTA DEI PROCESSI (ATTIVI E NON)
 * Dato l'albero dei processi stampa a video la lista di tutti i processi generati
 *
 * @param process: Albero/sottoalbero dei processi
 */
void p_listall(Process *process_tree) {
	printf("Richiesta della lista dei processi generati...\n\n");
	printProcessList(process_tree, 1);
	printf("\n");
}

/* ======================================================================================================= */

/**
 * STAMPA A VIDEO LA LISTA DI TUTTI I PROCESSI OMONIMI
 * Dato l'albero dei processi stampa a video tutti i processi generati con lo stesso nome
 *
 * @param process: Albero/sottoalbero dei processi
 * @param name: Nome dei processi da cercare
 */
void p_search(Process *process_tree, char *name) {
	int count;      // Numero dei processi trovati con lo stesso nome

	printf("Richiesta della lista dei processi generati con nome '%s'...\n\n", name);
	count = searchNamesakes(process_tree, name, 0);
	printf("\n");

	// Non esistono processi con nome 'nome'
	if (count == 0) {
		printf("Non esistono processi con nome '%s'\n\n", name);
	}
}

/* ======================================================================================================= */

/**
 * CREAZIONE DI UN PROCESSO
 * Creazione di un nuovo processo con nome 'name' attraverso una fork() del processo principane 'pmanager'
 *
 * @param process: Albero/sottoalbero dei processi
 * @param name: Nome del nuovo processo
 */
void p_new(Process *process_tree, char *name) {
	if (strcmp(name, "pmanager") != 0) {
		printf("Richiesta creazione processo '%s' ...\n", name);
		int frk = forkProcess(process_tree, name);

		if (frk != 0) {
			printf("Generazione del processo '%s' fallita\n\n", name);
		} else {
			printf("Generazione del processo '%s' avvenuta con successo\n\n", name);
		}
	} else {
		printf("\nErrore: Non è consentito creare un processo con nome 'pmanager'\n\n");
	}
}

/* ======================================================================================================= */

/**
 * CLONAZIONE DI UN PROCESSO
 * Clonazione del processo 'name' effettuata attraverso la creazione di un nuovo processo con name_i,
 * dove name è il nome del processo clonato con i progressivo.
 * Qualora esistano piu processi omonimi viene stampata a video la lista dei processi con il nome
 * cercato e l'utente puo' selezionare il processo desiderato attraverso il suo Pid
 *
 * @param process: Albero/sottoalbero dei processi
 * @param name: Nome del processo da clonare
 * @parem fifo: Nome della Pipe da utilizzare per lo scambio di messaggi
 */
void p_spawn(Process *process_tree, char *name, char *fifo) {
	int pid;					// pid del processo da clonare e numero di processo 'nome'
	Process *process;			// Riferimento al processo 'nome'

	// I processi devono essere attivi
	printf("\n");
	pid = findPid(process_tree, name, 1);
	printf("\n");

	if (pid != 0) {
		int cln;
		process = searchByPid(process_tree, pid);

		printf("Richiesta clonazione del processo '%s --- %d'...\n\n", name, pid);
		cln = cloneProcess(process, pid, fifo);
		
		// Errore nella clonazione
		if (cln != 0) {
			printf("Clonazione del processo '%s --- %d fallita'\n\n", name, pid);
		} else {
			printf("Clonazione del processo '%s --- %d' avvenuta con successo\n\n", name, pid);
		}
	}
}

/* ======================================================================================================= */

/**
 * CHIUSURA DI UN PROCESSO
 * Chiusura del processo 'name'
 * Qualora esistano piu processi omonimi viene stampata a video la lista dei processi con il nome
 * cercato e l'utente puo' selezionare il processo desiderato attraverso il suo Pid
 *
 * @param process: Albero/sottoalbero dei processi
 * @param name: Nome del processo che si vuole chiudere
 */

void p_close(Process *process_tree, char *name) {
	int pid;				// Pid del processo da rimuovere
	Process *process;		// Riferimento al processo da rimuovere

	// Il processo deve essere attivo
	printf("\n");
	pid = findPid(process_tree, name, 1);
	printf("\n");

	// pid valido (Il processo esiste e ha nome 'name')
	if (pid != 0) {
		int rm;
		process = searchByPid(process_tree, pid);

		printf("Richiesta chiusura processo '%s --- %d'...\n\n", name, pid);
		rm = removeProcess(process);

		// Rimozione fallita
		if (rm != 0) {
			printf("Rimozione del processo '%s --- %d' fallita\n\n", name, pid);
		} else {
			printf("Rimozione del processo '%s --- %d' avvenuta con successo\n\n", name, pid);
		}
	}
}

/**
 * CHIUSURA DELL'ALBERO DEI PROCESSI
 * Chiusura dell'albero dei processi con radice il processo 'name'
 * Qualora esistano piu processi omonimi viene stampata a video la lista dei processi con il nome
 * cercato e l'utente puo' selezionare il processo desiderato attraverso il suo Pid
 *
 * @param process: Albero/sottoalbero dei processi
 * @param name: Nome del processo che si vuole chiudere
 */
void p_rmall(Process *process_tree, char *name) {
	int pid;
	Process *process;

	// I processi devono essere attivi
	printf("\n");
	pid = findPid(process_tree, name, 0);
	printf("\n");

	if (pid != 0) {
		int rm;
		process = searchByPid(process_tree, pid);

		if ((*process).state == 1) {
			printf("Richiesta terminazione ricorsiva del processo '%s --- %d'...\n", name, pid);
			rm = removeProcessTree(process);

			// Errore nella rimozione
			if (rm != 0) {
				printf("\nRimozione ricorsiva del processo '%s --- %d fallita'\n\n", name, pid);
			} else {
				printf("\nRimozione ricorsiva del processo '%s --- %d' avvenuta con successo\n\n", name, pid);
			}
		} else {
			printf("Rimozione fallita: il processo '%s' è gia stato terminato\n", name);
		}
	}
}

/**
 * GENERAZIONE DI UN NUOVO PROCESSO
 * attraverso la system call fork().
 *
 * @param process: Nodo relativo al processo padre
 * @param name: nome del processo-figlio
 *
 * @return 0 Generazione figlio avvenuta con successo
 * @return 1 Input non valido
 * @return 2 Fallimento exec
 * @return 3 Fallimento fork()
 */
int forkProcess(Process *process_tree, char *name) {
	int res;

	// Null pointers
	if (process_tree == NULL || name == NULL) {
		printf("Input non valido nella funzione forkProcess\n");
		res = 1;
	} else {
		int childPid = fork();

		// Successo Fork()
		if (childPid >= 0) {
			// Figlio
			if (childPid == 0) {
				// Eseguo il programma 'CHILD'
				execl("./build/child", "./build/child", (char *)0);
				printf("\nProcesso '%d':EXEC fallita\n\n", (int)getpid());
				res = 2;
			}
			// padre
			else {

				// Ritardo introdotto per la modalità di test
				sleep(1);

				// Inserimento del nodo nell'albero dei processi
				do {
					res = insertChild(process_tree, name, childPid);
				} while (res != 0);
			}
		} else {
			printf("\nFork() fallita\n\n");
	  perror("Fork");
			res = 3;
		}
	}
	return res;
}

/**
 * CLONAZIONE DI UN PROCESSO
 * Se il parametro in ingresso non e' il processo centrale pmanager
 * viene generato un segnale verso il processo che si richiede di clonare, dopodichè
 * il processo passa ad uno stato di attesa finchè non viene ricevuto un segnale di
 * risposta di avvenuta/mancata clonazione. Attraverso l'uso di una Pipe con nome viene letto
 * il ProcessIdentifier del nuovo processo generato che verrà inserito nella struttura dati.
 *
 * @param process: Il processo che verra' clonato
 * @param pid: Il pid del processo di cui si richiede la Clonazione
 * @param myfifo: nome della Pipe utilizzata
 *
 * @return 0 Clonazione avvenuta con successo
 * @return 1 Null Pointer passato come parametro
 * @return 2 Fallimento della syscall fork()
 * @return 3 Clonazione pmanager
 *
 */
 int cloneProcess(Process *process, int pid, char *fifo) {
	int res;

	// Nota: il controlo process != NULL è gia stato fatto prima di invocare cloneProcess()
	if (fifo == NULL) {
		printf("\nErrore: nome Pipe non valido nella funzione cloneProcess\n\n");
		res = 1;
	} else {
		if ((*process).ppid != 0) {
			
			//Invio di un segnale SIGUSR1 al processo che si desidera clonare e attendo finche non ricevo un segnale che notifica l'avvenuta creazione
			resumeParent = 0;
			kill(pid, SIGUSR1);
			while (resumeParent == 0) {
				sleep(1);
			}
			
			// Successo di fork()
			if (flag_spawn == 1) {
				int fd;

				// Reset del flag
				flag_spawn = 0;

				do {
					// Apertura Pipe in lettura
					fd = open(fifo, O_RDONLY);
				} while (fd == -1);

				// Attendo che il processo scriva il suo messaggio contenente il Pid del nuovo processo
				resumeParent = 0;
				while (resumeParent == 0) {
					sleep(1);
				}

				// Lettura del messaggio
				char buff[10];
				int n = read(fd, buff, 10);
				int childPid = atoi(buff);

				int stat;
				do {
					// Chiusura Pipe in lettura
					stat = close(fd);
				} while (stat != 0);

				// Attendo che venga chiuso anche il lato scrittura
				sleep(1);

				// Costruzione nome clone

				// 5 = numero massimo di cifre di clone_level
				int len = strlen(process->name) + 5;
				char *tmp = (char *) malloc(sizeof (char)*len);
				sprintf(tmp, "%s_%d", process->name, (*process).clone_level);
				(*process).clone_level++;

				// Inserimento del nodo relativo al nuovo processo nella struttura dati
				do {
					res = insertChild(process, tmp, childPid);
				} while (res != 0);

				free(tmp);
				flag_spawn = 0;
			}
			// Fallimento fork()
			else {
				printf("\nImpossibile clonare: Fallimento fork()\n\n");
				res = 2;
			}
		} else {
			printf("\nErrore: Non è consentito clonare il processo 'pmanager'\n\n");
			res = 2;
		}
	}
	return res;
}
