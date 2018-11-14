#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "treeLib.h"
#include "customShellLib.h"


// Signal handlers definitions
void childHandler (int sig);
void cloneHandler (int sig);
void resumeHandler (int sig);

// Definizioni wrappers system calls
void makePipe(char *pathPipe);
void unlinkPipe(char *pathPipe);

int main(int argc, char* argv[]) {
	FILE *stream;
	char *line = NULL;
	char *token = NULL;
	size_t len = 0;
	int slen = 0;
	ssize_t nread;
	stream = stdin;

	if (argc == 1) {
		stream = stdin;
		if (stream == NULL) {
			perror("fopen");
			exit(EXIT_FAILURE);
		}
	} else {
		stream = fopen(argv[1], "r");
		if (stream == NULL) {
			perror("fopen");
			exit(EXIT_FAILURE);
		}
	}

	// Creazione pipe con nome "myfifo"
	char * myfifo = "myfifo";
	makePipe(myfifo);

	// Creazione e inizializzazione dell'albero dei processi
	Process *process_tree;
	int pid = getpid();
	init (&process_tree, pid);

	signal (SIGCHLD, childHandler);				// Install death-of-child handler
	signal (SIGCONT, cloneHandler);				// Install clone-handler
	signal (SIGUSR2, cloneHandler);				// Install clone-handler
	signal (SIGUSR1, resumeHandler);			// Install resume-handler

	// Gestore delle richieste della sub-shell
	char ans = 'y';
	while (ans == 'y' || ans == 'Y') {
		while ((nread = getline(&line, &len, stream)) != -1) {
			slen = strlen(line);

			if (slen > 1) {
				line[slen-1] = '\0';
			}

			token = strtok(line, " ");

			// COMMANDS

			// QUIT COMMAND
			if(strcmp(token, "quit") == 0) {
				printf("\nUscita...\n");
				// Rilascio la memoria allocata per l'albero dei processi
				deleteProcessTree(&process_tree);
				unlink(myfifo);
				exit(EXIT_SUCCESS);
			}

			// P-HELP COMMAND
			else if (strcmp(token, "phelp") == 0) {
				p_help();
			}

			// P-LIST COMMAND
			else if (strcmp(token, "plist") == 0) {
				p_list(process_tree);
			}

			// P-LISTALL COMMAND
			else if (strcmp(token, "plistall") == 0) {
				p_listall(process_tree);
			}

			// P-NEW COMMAND
			else if (strcmp(token, "pnew") == 0) {
				token = strtok(NULL, " ");

				if (token != NULL) {
					p_new(process_tree, token);
					token = strtok(NULL, " ");

					while (token != NULL) {
						p_new(process_tree, token);
						token = strtok(NULL, " ");
					}
				} else {
					printf("Process Manager: error: too few arguments to function 'pnew'\n"
						   "Usage: pnew <nome>\n");
				}
			}

			// P-INFO COMMAND
			else if (strcmp(token, "pinfo") == 0) {
				token = strtok(NULL, " ");

				if(token != NULL) {
					p_info(process_tree, token);
					token = strtok(NULL, " ");

					while (token != NULL) {
						p_info(process_tree, token);
						token = strtok(NULL, " ");
					}
				} else {
					printf("Process Manager: erorr: too few arguments to function 'pinfo'\n"
						   "Usage: pinfo <nome>\n");
				}
			}

			// P-CLOSE COMMAND
			else if (strcmp(token, "pclose") == 0) {
				token = strtok(NULL, " ");

				if (token != NULL) {
					p_close(process_tree, token);
					token = strtok(NULL, " ");

					while (token != NULL) {
						p_close(process_tree, token);
						token = strtok(NULL, " ");
					}
				} else {
					// P-CLOSE senza parametro
					printf("Process Manager: erorr: too few arguments to function 'pclose'\n"
						   "Usage: pclose <nome>\n");
				}
			}

			// P-SPAWN COMMAND
			else if (strcmp(token, "pspawn") == 0) {
				token = strtok(NULL, " ");

				if (token != NULL) {
					p_spawn(process_tree, token, myfifo);
					token = strtok(NULL, " ");

					while (token != NULL) {
						p_spawn(process_tree, token, myfifo);
						token = strtok(NULL, " ");
					}
				} else {
					// Pspawn senza parametro
					printf(	"Process Manager: erorr: too few arguments to function 'pspawn'\n"
							"Usage: pspawn <nome>\n");
				}
			}

			// P-RMALL COMMAND
			else if (strcmp(token, "prmall") == 0) {
				token = strtok(NULL, " ");

				if (token != NULL) {
					p_rmall(process_tree, token);
					token = strtok(NULL, " ");

					while (token != NULL) {
						p_rmall(process_tree, token);
						token = strtok(NULL, " ");
					}
				} else {
					printf(	"Process Manager: error: too few arguments to function 'prmall'\n"
							"Usage: prmall <nome>\n");
				}
			}

			// P-TREE COMMAND
			else if (strcmp(token, "ptree") == 0) {
				p_tree(process_tree);
			}

			// P-SEARCH COMMAND
			else if (strcmp(token, "psearch") == 0) {
				token = strtok(NULL, " ");

				if (token != NULL) {
					p_search(process_tree, token);
					token = strtok(NULL, " ");

					while (token != NULL) {
						p_search(process_tree, token);
						token = strtok(NULL, " ");
					}
				} else {
					printf(	"Process Manager: error: too few arguments to function 'psearch'\n"
							"Usage: psearch <nome>\n");
				}
			}

			// P-TREEALL COMMAND
			else if (strcmp(token, "ptreeall") == 0) {
				p_treeall(process_tree);
			} else {
				system(line);
			}
		}

		if (stream != stdin) {
			printf("Continuare ad inserire comandi? [y/n]\n");
			scanf("%c", &ans);
			if (ans == 'y' || ans == 'Y') {
				stream = stdin;

				if (stream == NULL) {
					perror("fopen");
					exit(EXIT_FAILURE);
				}
			}
		}
	}

	// Rilascio la memoria allocata per l'albero dei processi
	deleteProcessTree(&process_tree);

	unlinkPipe(myfifo);
	free(line);
	fclose(stream);
	exit(EXIT_SUCCESS);
}

/* ======================================================================================================= */

// SIGNAL_HANDLERS

// Eseguito se il figlio termina prima del padre
void childHandler (int sig) {
	int childPid, childStat;
	while((childPid = waitpid(-1, &childStat, WNOHANG)) > 0);
}

// Eseguito quando si riceve risposta alla clonazione di un processo
void cloneHandler (int sig) {
	// Setto il flag flag_spawn (successo = 1 / fallimento = 0)
	if (sig == SIGCONT) {
		flag_spawn = 1;
	} else if (sig = SIGUSR1) {
		flag_spawn = 0;
	}
}

// Signal Handler per la sincronizzazione dei processi in pspawn
void resumeHandler (int sig) {
	resumeParent = 1;
}

/* ======================================================================================================= */
// WRAPPERS SYSTEM CALLS

// Creazione pipe con nome
void makePipe(char *pathPipe) {
	int stat;

	stat = mknod(pathPipe, S_IFIFO, 0);
	if (stat != 0) {
		perror("mknod");
		exit(EXIT_FAILURE);
	} else {

		// Modifica dei permessi
		stat = chmod(pathPipe, 0666);
		if (stat != 0) {
			perror("chmod");
			exit(EXIT_FAILURE);
		}
	}
}

void unlinkPipe(char *pathPipe) {
	int stat;

	do {
		stat = unlink(pathPipe);
	} while (stat == -1);
}
