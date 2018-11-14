# SHELL = /bin/sh

CC = gcc 

PRJSRC = customShell.c treeLib.c treeLib.h customShellLib.c customShellLib.h
CHLDSRC = child.c
PMSRC = pmanager.c
TESTSRC = test-00.txt test-01.txt test-02.txt test-03.txt

ASSDIR = assets
BLDDIR = build
TESTDIR = tests

PRJMNG = ./$(BLDDIR)/pmanager

# specifica quali sono le regole e non i file
.PHONY: help clean build assets test run

help:
	@cp $(addprefix src/,README) $(CURDIR)
	@cat README

build: clean src/*
	@echo 'Creazione cartella build...'
	@mkdir $(BLDDIR)
	@echo 'Fatto'
	@echo 'Compilazione child...'
	@$(CC) -o $(BLDDIR)/child $(addprefix src/,$(CHLDSRC))
	@echo 'Fatto'
	@echo 'Compilazione della customShell...'
	@$(CC) -o $(BLDDIR)/customShell $(addprefix src/,$(PRJSRC))
	@echo 'Fatto'
	@echo 'Compilazione del Project Manager...'
	@$(CC) -o $(BLDDIR)/pmanager $(addprefix src/,$(PMSRC))
	@echo 'Fatto'
	@echo 'NOTA: per la corretta esecuzione di pmanager è necessario creare la directory /assets'

clean:
	@echo 'Rimozione cartella build...'
	@rm -rf $(BLDDIR)
	@echo 'Fatto'
	@echo 'Rimozione cartella assets...'
	@rm -rf $(ASSDIR)
	@echo 'Fatto'
	@echo 'Rimozione della Pipe...'
	@rm -f myfifo
	@echo 'Fatto'
	@echo 'Rimozione README...'
	@rm -f README
	@echo 'Fatto'

assets: build
	@echo 'Creazione cartella assets...'
	@mkdir -p $(ASSDIR)
	@echo 'Fatto'
	@echo 'Spostamento file nella cartella assets...'
	@cp $(addprefix src/,man.1) $(ASSDIR)
	@cp $(addprefix src/,test_choice.sh) $(ASSDIR)
	@cp $(addprefix src/,test-list.txt) $(ASSDIR)
	@echo 'Fatto'
	@echo 'Creazione cartella tests...'
	@mkdir -p $(ASSDIR)/$(TESTDIR)
	@echo 'Fatto'
	@echo 'Spostamento dei file di test'
	@cp $(addprefix src/,$(TESTSRC)) $(ASSDIR)/$(TESTDIR)
	@echo 'Fatto'
	@echo 'Rendo eseguibile lo script di testing...'
	@chmod +x $(addprefix $(ASSDIR)/,test_choice.sh)
	@echo 'Fatto'

test: assets
	@echo 'Avvio modalità di test...'
	@sh $(ASSDIR)/test_choice.sh $(PRJMNG)

run: assets
	@echo 'Avvio Project manager...'
	@$(PRJMNG)
