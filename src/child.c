#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

// Flag che viene settato a 1 qualora venga ricevuta una richiesta di clonazione
int flag_fork = 0;
// Flag per la sincronizzazione dei processi che utilizzano la Pipe
int resumeChild = 0;

// Pid del processo principale 'pmanager' (Utilizzato per l'invio di segnali di risposta)
pid_t pmanager_pid;

// Definizioni signal handlers
void childHandler (int sig);
void terminationHandler(int sig);
void resumeHandler(int sig);
void infoHandler(int sig, siginfo_t *siginfo, void *context);

int main(int argc, char *argv[]) {
	int fd;
	char *myfifo = "myfifo";

	struct sigaction sa;

	sa.sa_sigaction = (void *)infoHandler;
	sa.sa_flags = SA_RESTART | SA_SIGINFO;

	sigaction(SIGUSR1, &sa, NULL);					// Install fork-request handler
	signal(SIGQUIT, terminationHandler);			// Install termination handler
	signal (SIGCHLD, childHandler);					// Install death-of-child handler
	signal (SIGUSR2, resumeHandler);				// Install wake-up-handler

	printf("PROCESSO '%d' CREATO\n\n", getpid());

	while(1) {
		// Il processo attende finchè non riceve un segnale (clonazione oppure terminazione)
		pause();
		// Ho ricevuto una richiesta di clonazione (Flag fork viene settato a uno appena ricevo il segnale SIGUSR1)
		if (flag_fork == 1) {
			int childPid = fork();

			// Successo fork()
			if (childPid >= 0) {

				//figlio
				if (childPid == 0) {
					// Eseguo il programma 'CHILD '
					execl("./build/child", "./build/child", (char *)0);
					printf("Processo '%d':EXEC fallita\n", (int)getpid());
					perror("Exec");
				}
				// Padre
				else {
					// Resetto Flag
					flag_fork = 0;

					// Segnalo al processo principale che la fork ha avuto successo
					kill(pmanager_pid, SIGCONT);

					// Sblocco il processo principale dallo stato di attesa
					kill(pmanager_pid, SIGUSR1);
					do {
						// Apertura Pipe in scrittura
						fd = open("myfifo", O_WRONLY);
					} while (fd == -1);

					// Creazione del messaggio contenente il Pid del figlio
					char message[6];
					int message_len;
					sprintf(message, "%d", childPid);
					message_len = strlen(message);

					// Scrittura nella Pipe
					write(fd, message, message_len);

					// Invio un segnale a pmanager notificando la fine della scrittura
					kill(pmanager_pid, SIGUSR1);
					int stat;
					do {
						// Chiusura pipe in scrittura
						stat = close(fd);
					} while (stat != 0);
				}
			} else {
				printf("Processo '%d': FORK() fallita\n", (int)getpid());
				perror("Fork");
				// Invio un segnale SIGUSR2 al processo principale per indicare che la fork è fallita
				kill(pmanager_pid, SIGUSR2);
			}
		}
	}

	return 0;
}

/* ======================================================================================================= */

// SIGNAL_HANDLERS

// Eseguito se il figlio termina prima del padre
void childHandler (int sig) {
	int childPid, childStat;
	while((childPid = waitpid(-1, &childStat, WNOHANG)) > 0);
}

// Eseguito quando riceve un segnale di terminazione
void terminationHandler (int sig) {
	printf("Chiusura processo %d\n", getpid());
	exit(0);
}

// Eseguito quando riceve una richiesta di clonazione
void infoHandler(int sig, siginfo_t *siginfo, void *context) {
	if (sig == SIGUSR1) {
		// Pid del processo che ha generato il segnale SIGUSR1
		pmanager_pid = siginfo->si_pid;
		printf("Processo '%d': Richiesta di clonazione ricevuta dal processo '%d'\n", getpid(), (int)pmanager_pid);
		// Setto il flag di clonazione
		flag_fork = 1;
	}
}

// Eseguito quando riceve un segnale di wake-up
void resumeHandler(int sig) {
	resumeChild = 1;
}
