#!/bin/bash
# Dialogo per la selezione dei test da effettuare.

c='y'

until [ ${c} = 'n' ];
do
	clear
	echo "Quale test vuoi scegliere?";
	more assets/test-list.txt
	read opt;

	if [ ${opt} = 'A' ];
	then
		echo "Inserire i comandi. 'stop' per terminare"
		while read comando && [ "$comando" != "stop" ];
		do
			echo $comando >> assets/tests/test.txt
		done
		$1 assets/tests/test.txt
	else
		$1 assets/tests/test-0${opt}.txt
	fi

	# Attendo le notifiche dei processi terminati
	sleep 1
	echo "Vuoi proseguire? (y/n)"
	read c
done
rm -f assets/tests/test.txt
exit
