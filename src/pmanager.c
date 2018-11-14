#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char* argv[]) {
	char command[100];

	if (argc == 1) {
		system("/bin/bash -c ./build/customShell");
	} else {
		sprintf(command, "/bin/bash -c './build/customShell %s'", (char *)argv[1]);
		system(command);
	}

	return 0;
}
