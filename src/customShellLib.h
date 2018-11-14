#ifndef SHEll_H
#define SHELL_H

#include "treeLib.h"

int flag_spawn;      // Flag globale per la gestione del segnale di risposta p_spawn
int resumeParent;    // Flag globale per la sincronizzazione dei processi

void p_help();

void p_info(Process *process_tree, char *name);
void p_tree(Process *process_tree);
void p_treeall(Process *preocess_tree);
void p_list(Process *process_tree);
void p_listall(Process *preocess_tree);
void p_search(Process *process_tree, char *name);

void p_new(Process *process_tree, char *name);
void p_spawn(Process *process_tree, char *name, char *fifo);

void p_close(Process *process_tree, char *name);
void p_rmall(Process *process_tree, char *name);

#endif
