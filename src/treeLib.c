#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "treeLib.h"

/**
 * INIZIALIZZAZIONE DELL'ALBERO DEI PROCESSI
 * Inizializzazione dell'albero dei printProcessList: Creo il nodo radice con nome "pmanager" e un ppid fittizio = 0.
 *
 * @param pmanager Puntatore all'albero (vuoto)
 * @param pid Process Identifier del processo principale pmanager
 *
 * @return 0 L'inizializzazione e' andata a buon fine
 * @return 1 Inizializzazione fallita poiche' non e' stato possibile creare il nodo
 */
int init(Process **pmanager, int pid) {
	int res;
	res = newProcess(pmanager, "pmanager", pid, 0); // 0 = ppid fittizio!

	// newProcess failed
	if (res != 0) {
		printf("%sTree.c: Inizializzazione fallita: Nodo radice non creato%s\n", KRED, KNRM);
	}

	return res;
}

/* ======================================================================================================= */

/**
 * CREAZIONE DI UN NODO
 * Creazione del nodo relativo al processo "name"
 *
 * @param process: Puntatore al nodo relativo al processo
 * @param name: nome del nuovo processo
 * @param pid: Process Identifier del nuovo processo
 * @param ppid: Process identifier del processo che lo ha generato
 *
 * @return 0 La creazione del nodo e' andata a buon fine
 * @return 1 Creazione fallita (non e' stato possibile allocare memoria per il nodo oppure per il nome)
 */
int newProcess(Process **process, char *name, int pid, int ppid) {
	int res;

	*process = (Process *) malloc(sizeof (Process));
	Process *tmp = *process;

	// malloc failed
	if (tmp == NULL) {
		printf("%sTree.c: Allocazione memoria per il nodo fallita%s\n", KRED, KNRM);
		res = 1;
	} else {
		tmp->name = (char *) malloc(sizeof (char) * strlen(name));

		// malloc failed
		if (tmp->name == NULL) {
			printf("%sTree.c: Allocazione memoria per il nome fallita%s\n", KRED, KNRM);
			free(tmp);
			res = 1;
		} else {
			strcpy(tmp->name, name);
			(*tmp).pid = pid;
			(*tmp).ppid = ppid;
			(*tmp).clone_level = 0;
			(*tmp).no_childs = 0;
			(*tmp).state = 1;
			(*tmp).creation = time(NULL);
			(*tmp).termination = (time_t)0;
			tmp->parent = NULL;
			tmp->child = NULL;
			tmp->left_sibling = NULL;
			tmp->sibling = NULL;
			res = 0;
		}
	}
	return res;
}

/* ======================================================================================================= */

/**
 * INSERIMENTO DI UN NODO NELL'ALBERO DEI PROCESSI
 * Inserimento di un nodo nell'albero dei processi: L'inserimento avviene IN TESTA alla lista dei figli.
 *
 * @param process: Nodo al quale aggiungere il figlio
 * @param name: nome del processo figlio
 * @param pid: Process Identifier del processo figlio
 *
 * @return 0 Inserimento andato a buon fine
 * @return 1 Inserimento fallito poichè non è stato possibile creare il nuovo nodo
 * @return 2 Null Pointer come parametro
 */
int insertChild(Process *process, char *name, int pid) {
	int res;
	Process *child;

	if (process == NULL) {
	printf("%sTree.c: NULL pointer in insertChild%s\n", KRED, KNRM);

	res = 2;
	} else {
		res = newProcess(&child, name, pid, (*process).pid);

		// Nodo non creato
		if (res != 0) {
			printf("%sTree.c: Nodo figlio %d non creato%s\n", KRED, pid, KNRM);
		} else {
			(*process).no_childs++;
			Process *oldChild = process->child;

			// Non ci sono altri figli
			if (oldChild == NULL) {
				process->child = child;
				child->parent = process;
			} else {
				// Non e' il primo figlio, devo aggiornare i puntatori
				oldChild->left_sibling = child;
				child->parent = process;
				child->sibling = oldChild;
				process->child = child;
			}
		}
	}
	return res;
}

/* ======================================================================================================= */

/**
 * STAMPA A VIDEO INFORMAZIONI RELATIVE AL PROCESSO
 * Stampa a video informazioni relative al processo passato come parametro:
 *	- Nome del processo
 *	- Process Identifier
 *	- Process identifier del processo padre
 *
 * @param process: processo passato come parametro
 */
void printProcess(Process *process) {

	if (process == NULL) {
		printf("%sTree.c: Null pointer in printProcess%s\n", KRED, KNRM);
	} else {
		char *color;

		if ((*process).state == 1) {
			color = KBLU;
		} else {
			color = KGRY;
		}

		printf("%sPROCESS_NAME:%s %s --- %sPROCESS_PID:%s %d --- %sPROCESS_PPID:%s %d\n",
			color, KNRM, process->name, color, KNRM, (*process).pid, color, KNRM, (*process).ppid);
	}
}

/* ======================================================================================================= */

/**
*	STAMPA A VIDEO INFORMAZIONI DETTAGLIATE DEL PROCESSO
* Stampa a video informazioni dettagliate del processo passato come parametro:
*		- Nome del processo
* 	- Process Identifier
*		- Process Identifier del processo padre
* 	- Data di creazione del processo
*		- Stato del processo el enentuale data di terminazione
*
* @param process: processo passato come parametro
*/
void processInfo(Process *process) {
	if (process == NULL) {
		printf("%sTree.c: Null pointer in printProcess%s\n", KRED, KNRM);
	} else {
		char *color;

		// Modifica colore in base allo stato del processo
		if ((*process).state == 1) {
			color = KBLU;
		} else {
			color = KGRY;
		}

		char* c_time_creation = ctime(&((*process).creation));
		printf("%sPROCESS_NAME:%s %s\n", color, KNRM, process->name);
		printf("%sPROCESS_PID:%s %d\n", color, KNRM, (*process).pid);
		printf("%sPROCESS_PPID:%s %d\n", color, KNRM, (*process).ppid);
		printf("%sNUMBER_OF_ACTIVE_CHILDS:%s %d\n", color, KNRM, (*process).no_childs);
		printf("%sCREATION_TIME:%s %s", color, KNRM, c_time_creation);

		// Il processo è ancora attivo
		if ((*process).state == 1) {
			printf("%sSTATO:%s attivo\n", color, KNRM);
		} else {
			// Il processo è stato terminato; Stampo la data di terminazione
			char* c_time_termination = ctime(&((*process).termination));
			printf("%sSTATO:%s terminato\n", color, KNRM);
			printf("%sTERMINATION_TIME:%s %s", color, KNRM, c_time_termination);
		}
	}
}
/* ======================================================================================================= */

/**
 * STAMPA A VIDEO DELLA STRUTTURA GERARCHICA DELL'ALBERO DEI PROCESSI
 * Stampa a video la struttura gerarchica dell'albero dei processi attraverso una visita anticipata dell'albero.
 *
 * @param	process: radice dell'albero/sottoalbero da cui comincia la visita
 * @param level: profondita' del nodo (necessaria per le tab)
 * @param active: valore che indica se devono essere stampati solo i processi affettivamente attivi oppure anche quelli terminati
 */
void printProcessTree(Process *process, int level, int active) {
	if (process == NULL) {
	printf("%sTree.c: Null pointer in printProcessTree%s\n", KRED, KNRM);
	} else {
		// Stampo solo se il processo è attualmente attivo
		if (active == 1 || (*process).state == 1) {
			printProcess(process);
		}

		Process *child = process->child;

		while (child != NULL ) {
			if ((*child).state == 1 || active == 1) {
				int i = 0;

				// Indentazione
				for (i; i <= level  ; i++) {
					printf("|");
					printf("\t");
				}
				printf("|——");
				printProcessTree(child, level + 1, active);
				i = 0;
			}
			child = child->sibling;
		}
	}
}

/* ======================================================================================================= */

/**
 * STAMPA A VIDEO DELL'ALBERO
 * Stampa a video l'albero dei processi attraverso una visita anticipata.
 *
 * @param process: radice dell'albero/sottoalbero da cui comincia la visita
 * @param active: valore che indica se devono essere stampati solo i processi effettivamente attivi oppure anche quelli terminati
 */
void printProcessList(Process *process, int active) {
	if (process == NULL) {
		printf("%sTree.c: Null pointer in printProcessTree%s\n", KRED, KNRM);
	} else {
		if (active == 1 || (*process).state == 1) {
			printProcess(process);
		}

		Process *child = process->child;

		while (child != NULL) {
			printProcessList(child, active);
			child = child->sibling;
		}
	}
}

/* ======================================================================================================= */

/**
  * DISATTIVAZIONE DI UN NODO NELL'ALBERO DEI PROCESSI
  * la rimozione genera un segnale di terminazione che viene inviato al relativo processo
  * e gestito attraverso il terminationChildHandle.
  * Non e' possibile eliminare il nodo qualora esso si riferisca alla radice pmanager
  * oppure se il nodo in questione e' padre di almeno un processo.
  * La rimozione avviene attraverso l'aggiornameno dello stato del processo e
  * la relativa data di terminazione.
  *
  * @param process: In nodo relativo al processo che si vuole eliminare
  *
  * @return 0 Rimozione avvenuta con successo
  * @return 1 Rimozione fallita: puntatore NULL passato come parametro
  * @return 2 Rimozione fallita: non e' possibile rimuovere il nodo pmanager
  * @return 3 Rimozione fallita: non e' possibile elimanare un nodo che ha generato almeno un processo
  *
  */

int removeProcess(Process *process) {
	int res;

	// Puntatore al process non valido
	if (process == NULL) {
		printf("%sTree.c: NULL POINTER in removeProcess%s\n", KRED, KNRM);
		res = 1;
	}
	// Nodo radice
	else if (strcmp(process->name, "pmanager") == 0) {
		printf("%sTree.c: Nodo radice: impossibile eliminare\n%s", KRED, KNRM);
		res = 2;
	}
	// Padre
	else if ((*process).no_childs != 0) {
		printf("%sTree.c: Questo nodo %s ha figli: impossibile eliminare%s\n", KRED, process->name, KNRM);
		res = 3;
	} else {
		int pid = process->pid;
		kill(pid, SIGQUIT);
		waitpid(pid);
		// aggiornamento data terminazione e stato
		(*process).termination = time(NULL);
		(*process).state = 0;

		// Decremento il numero dei figli del padre (Nota: parent non NULL poichè non è possibile rimuovere pmanager)
		Process *parent = process->parent;
		(*parent).no_childs--;
		res = 0;
	}
	return res;
}

/* ======================================================================================================= */

/**
 * RIMOZIONE DI UN NODO NELL'ALBERO DEI PROCESSI
 * la rimozione genera un segnale di terminazione che viene inviato al relativo processo
 * e gestito attraverso il terminationChildHandle.
 * Non e' possibile eliminare il nodo qualora esso si riferisca alla radice pmanager
 * oppure se il nodo in questione e' padre di almeno un processo.
 * La rimozione avviene attraverso l'aggiornameno della lista dei figli
 * a seconda della posizione del nodo al suo interno,
 * dopodiche' la memoria precedentemente allocata viene rilasciata.
 *
 * @param process: In nodo che si vuole eliminare
 *
 * @return 0 Rimozione avvenuta con successo
 * @return 1 Rimozione fallita: puntatore NULL passato come parametro
 * @return 2 Rimozione fallita: non e' possibile rimuovere il nodo pmanager
 * @return 3 Rimozione fallita: non e' possibile elimanare un nodo che ha generato almeno un processo
 *
 */
int deleteProcess(Process **process) {
	int res;
	Process *temp = *process;

	// puntatore in input null o process null
	if (temp == NULL || process == NULL) {
		printf("%sTree.c: NULL POINTER in deleteProcess%s\n", KRED, KNRM);
		res = 1;
	}
	// Nodo radice
	else if (strcmp(temp->name, "pmanager") == 0) {
		printf("%sTree.c: Nodo radice: impossibile eliminare\n%s", KRED, KNRM);
		res = 2;
	}
	// Padre
	else if (temp->child != NULL) {
		printf("%sTree.c: Questo nodo %s ha figli: impossibile eliminare%s\n", KRED, temp->name, KNRM);
		res = 3;
	} else {
		Process *left_sibling = temp->left_sibling;

		// Primo nodo della lista
		if (left_sibling == NULL) {
			Process *sibling = temp->sibling;
			Process *parent = temp->parent;

			// Coda della lista
			if (sibling == NULL) {
				parent->child = NULL;
			}
			// Non e' la coda della lista
			else {
				parent->child = sibling;
				sibling->left_sibling = NULL;
			}

			kill(temp->pid, SIGQUIT);	// invio del segnale di terminazione
			free(temp->name);			// Rilascio della memoria allocata per il nome
			free(temp);					// Rilascio della memoria allocata per il nodo relativo al processo
			*process = NULL;
			res = 0;
		}
		// Non e' il primo elemento della lista
		else {
			Process *sibling = temp->sibling;

			// Coda della lista
			if (sibling == NULL) {
				left_sibling->sibling = NULL;
			}
			// Non e' la coda della lista
			else {
				left_sibling->sibling = sibling;
				sibling->left_sibling = left_sibling;
			}

			kill(temp->pid, SIGQUIT);	// invio del segnale di terminazione
			free(temp->name);			// Rilascio della memoria allocata per il nome
			free(temp);					// Rilascio della memoria allocata per il nodo relativo al processo

			*process = NULL;
			res = 0;
		}
	}

	return res;
}

/* ======================================================================================================= */

/**
 * ELIMINAZIONE DELL'ALBERO
 * Rimozione ricorsiva dell'albero/sottoalbero dei processi attraverso una visita posticipata
 *
 * NOTA
 * Se la rimozione ricorsiva comincia dal processo principale quest'ultimo non viene rimosso.
 * Essendo una visita posticipata non si cercherà mai di eliminare dei nodi che hanno generato altri processi
 * (res = 2, 3  non si possono verificare)
 *
 * @param process: puntatore all albero che si vuole eliminare
 *
 * @return 0 Rimozioneavvenuta con successo
 * @return 1 Rimozione fallita: Null poiter passato come parametro
 *
 */
int deleteProcessTree(Process **process) {
	int res = 0;
	Process *temp = *process;

	if (temp == NULL || process == NULL) {
		printf("%sTree.c: Null pointer in deleteProcessTree%s\n", KRED, KNRM);
		res = 1;
	} else {
		Process *child = temp->child;

		// Se ho riscontrato un'errore durante la rimozione ricorsiva il ciclo termina
		while (child != NULL && res == 0) {
			// Rimozione ricorsiva dei sottoalberi
			res = deleteProcessTree(&child);

			if (res != 0) {
				printf("%sTree.c: Errore durante l'eliminazione in deleteProcessTree (%d)%s\n", KRED, res, KNRM);
				return res;
			}
			child = child->sibling;
		}

		// se la radice e' il gestore dei processi non viene eliminato
		if (strcmp(temp->name, "pmanager") != 0) {
			res = deleteProcess(&temp);
		}
	}

	return res;
}

/* ======================================================================================================= */

/**
 * DISATTIVAZIONE DELL' ALBERO
 * Rimozione ricorsiva dell'albero/sottoalbero dei processi attraverso una visita posticipata
 *
 * NOTA
 * Se la rimozione ricorsiva comincia dal processo principale quest'ultimo non viene rimosso.
 * Essendo una visita posticipata non si cercherà mai di rimuovere dei nodi che hanno generato altri processi
 * (res = 2, 3  non si possono verificare)
 *
 * @param process: puntatore all albero che si vuole eliminare
 *
 * @return 0 Rimozioneavvenuta con successo
 * @return 1 Rimozione fallita: Null poiter passato come parametro
 *
 */
int removeProcessTree(Process *process) {
	int res = 0;

	if (process == NULL) {
		printf("%sTree.c: Null pointer in deleteProcessTree%s\n", KRED, KNRM);
		res = 1;
	} else {
		Process *child = process->child;

		// Se ho riscontrato un'errore durante la rimozione ricorsiva il ciclo termina
		while (child != NULL && res == 0) {
			// Rimozione ricorsiva dei sottoalberi
			res = removeProcessTree(child);

			if (res != 0) {
				printf("%sTree.c: Errore durante l'eliminazione in deleteProcessTree (%d)%s\n", KRED, res, KNRM);
				return res;
			}
			child = child->sibling;
		}

		// se la radice e' il gestore dei processi non viene eliminato
		if (strcmp(process->name, "pmanager") != 0) {
			res = removeProcess(process);
		}
	}
	return res;
}


/* ======================================================================================================= */

/**
 * RICERCA DI UN PROCESSO TRAMITE PID
 * Dato l'albero dei processi restituisce il puntatore al processo con ProcessIdentifier pid
 * attraverso una visita ricorsiva anticipata
 *
 * @param process: Albero/sottoalbero dei processi
 * @param pid: pid del processo cercato
 *
 * @return NULL Il processo pid non esiste
 * @return NULL Null poiter passato come parametro (albero inesistente)
 * @return Puntatore al processo se esiste
 */
Process* searchByPid(Process *process, int pid) {
	Process *res = NULL;

	if (process == NULL) {
		printf("%sTree.c: NULL POINTER in searchByPid%s\n", KRED, KNRM);
	} else {
		//  Il nodo cercato e' la radice dell'albero
		if ((*process).pid == pid ) {
			res = process;
		} else {
			Process *child = process->child;

			// Ciclo finche' non trovo il processo pid o non ho visitato l'intero albero
			while (child != NULL && res == NULL) {
			res = searchByPid(child, pid);
			child = child->sibling;
			}
		}
	}
	return res;
}

/* ======================================================================================================= */

/**
 * RICERCA DI UN PROCESSO TRAMITE NOME
 * Dato l'albero dei processi restituisce il puntatore al processo "name"
 * attraverso una visita ricorsiva anticipata
 *
 * @param process: Albero/sottoalbero dei processi
 * @param name: Nome del processo cercato
 *
 * @return NULL Il processo "nome" non esiste
 * @return NULL Null poiter passato come parametro (albero inesistente)
 * @return Puntatore al processo se esiste
 */
Process* searchByName(Process *process, char *name) {
	Process *res = NULL;

	if (process == NULL) {
		printf("%sTree.c: NULL POINTER in search%s\n", KRED, KNRM);
	} else {
		//  Il nodo cercato e' la radice dell'albero
		if (strcmp(process->name, name) == 0) {
			res = process;
		} else {
			Process *child = process->child;

			// Ciclo finche' non trovo il processo "nome" o non ho visitato l'intero albero
			while (child != NULL && res == NULL) {
			res = searchByName(child, name);
			child = child->sibling;
			}
		}
	}
	return res;
}

/* ======================================================================================================= */

/**
 * RICERCA DI PROCESSI OMONIMI
 * Dato l'albero dei processi stampa a video tutti i processi con nome "name"
 * attraverso una visita ricorsiva anticipata
 *
 * @param process: Albero/sottoalbero dei processi
 * @param name: Nome del processo cercato
 * @param counter: Numero di processi 'nome' attualmente trovati
 *
 * @return numero di processi 'nome' trovati al termine della visita ricorsiva
 */

int searchNamesakes(Process *process, char *name, int counter) {
	int n = counter;

	if (process == NULL) {
		printf("%sTree.c: NULL POINTER in search%s\n", KRED, KNRM);
	} else {
		//  Il nodo cercato e' la radice dell'albero
		if (strcmp(process->name, name) == 0) {
			// Incremento il numero di processi trovati
			n++;
			printf("%d: ", n);
			printProcess(process);
		}
		Process *child = process->child;

		// Ciclo finche' non trovo il processo "nome" o non ho visitato l'intero albero
		while (child != NULL) {
		n = searchNamesakes(child, name, n);
		child = child->sibling;
		}
	}
	return n;
}

/* ======================================================================================================= */

/**
 * SELEZIONE PID PROCESSI OMONIMI
 * Dato l'albero dei processi stampa a video tutti i processi con nome "name"
 * attraverso una visita ricorsiva anticipata, dopodiche se esistono processi omonimi
 * consente all'utente di selezionare il pid del processo desiderato effettuando controlli sulla sua validita
 *
 * @param process: Albero/sottoalbero dei processi
 * @param name: Nome del processo cercato
 * @param active: specifica se il processo cercato debba essere attivo oppure no
 * (NB per la funzione pinfo è necessario stampare ed avere la possibilità di selezionare anche processi chiusi)
 *
 * @return numero di processi 'nome' trovati al termine della visita ricorsiva
 */

int findPid(Process *process_tree, char *name, int active) {
	int pid, counter;
	Process *process;

	printf("Ricerca dei processi con nome '%s'...\n", name);
	counter = searchNamesakes(process_tree, name, 0);
	// Non esistono processi con nome 'name'
	if (counter == 0) {
		printf("Non esistono processi con nome '%s'\n", name);
		pid = 0;
	} else if (counter == 1) {
		// Esiste un'unico processo 'name'; estraggo il pid e terminato
		process = searchByName(process_tree, name);

		// Il processo cercato deve essere attivo (active = 1)
		if (active == 1 && ((*process).state == 1)) {
			pid = (*process).pid;
		} else if (active == 0) {
			// Non è necessario che il processo cercato sia attivo (active = 0)
			pid = (*process).pid;
		} else  {
			pid = 0;
		}
	} else {
		// Esistono piu processi con lo stesso nome
		printf("Inserisci il pid del processo scelto: ");
		scanf("%d", &pid);

		// Ricerco il processo scelto
		process = searchByPid(process_tree, pid);

		// Processo trovato
		if (process != NULL) {
			// Controllo di non aver ricevuto il pid di un processo con un altro nome
			if (strcmp(process->name, name) == 0) {
				// Il processo deve essere attivo (state = 1) (NB: se active = 0 ritorno pid indipendentemente dal supo stato)
				if (active == 1 && ((*process).state == 0)) {
					printf("Richiesta non valida: Il processo '%s --- %d' è già terminato\n", name, pid);
					pid = 0;
				}
			} else {
				// pid non valido, non appartiene al processo 'nome'
				printf("Richiesta non valida: il Pid '%d' non appartiene al processo '%s'\n", pid, name);
				pid = 0;
			}
		} else {
			// Pid non valido (NB: counter != 0)
			printf("Pid '%d' non valido: Nessun processo trovato\n", pid);
			pid = 0;
		}

	}

	return pid;
}
