#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FSHELL_RL_BUFSIZE 1024
#define FSHELL_TOK_BUFSIZE 64
#define FSHELL_TOK_DELIM " \t\r\n\a"

// Function declarations for builtin shell commands
int fshell_cd(char **args);
int fshell_help(char **args);
int fshell_exit(char **args);
int fshell_frank(char **args);

// List of builting commands, and their functions
char *builtin_str[] = {
	"cd",
	"help",
	"exit",
	"frank"
};

int (*builtin_func[]) (char **) = {
	&fshell_cd,
	&fshell_help,
	&fshell_exit,
	&fshell_frank
};

char *fshell_read_line() {
	char *line = NULL;
	size_t bufsize = 0;

	getline(&line, &bufsize, stdin);

	return line;
}

char **fshell_split_line(char *line) {
	int bufsize = FSHELL_TOK_BUFSIZE;
	int position = 0;

	char **tokens = malloc(sizeof(char*) * bufsize);
	char *token;

	if (!tokens) {
		perror("fshell_split_line: malloc");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, FSHELL_TOK_DELIM);

	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += FSHELL_TOK_BUFSIZE;
			tokens = realloc(tokens, sizeof(char*) * bufsize);

			if (!tokens) {
				perror("fshell_split_line: realloc");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, FSHELL_TOK_DELIM);
	}

	tokens[position] = NULL;
	return tokens;
}

int fshell_launch(char **args) {
	pid_t pid;
	pid_t wpid;
	int status;

	pid = fork();

	if (pid == 0) {
		// Child process

		if (execvp(args[0], args) == -1) {
			perror("fshell");
		}

		// Should never get here
		exit(EXIT_FAILURE);

	} else if (pid < 0) {
		// Forking errir
		perror("fshell");

	} else {
		// Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int fshell_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

// Implemented builtin functions
int fshell_frank(char **args) {
	
	printf("This shell was built by Frank Karunaratna\n");
	printf("I'm a Computer Science student at the University of Toronto\n");
	printf("Shoot me a message at frank.karunaratna@gmail.com\n");
	
	return 1;
}

int fshell_cd(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "fshell: expected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("fshell");
		}
	}

	return 1;
}

int fshell_help(char **args) {
	int i;

	printf("Frank's Shell\n");
	printf("Built from a tutorial by Stephen Brennan\n");
	printf("Built in functions:\n");

	for (i = 0 ; i < fshell_num_builtins() ; i++) {
		printf(" %s\n", builtin_str[i]);
	}

	return 1;
}

int fshell_exit(char **args) {
	printf("Thanks for using fshell!\n");

	return 0;
}

int fshell_check_input_redirection(int argc, char **args) {
	char input_redir[2] = "<\n";
	
	for (int x = 0 ; x < argc ; x++) {
		if (strcmp(input_redir, args[x]) != 0) {
			printf("%s\n", args[x]);
		}
	}

}

int fshell_execute(char **args) {
	int i;

	if (args[0] == NULL) {
		// No args were provided
		return 1;
	}

	for (i = 0 ; i < fshell_num_builtins() ; i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}

	return fshell_launch(args);
}

void fshell_loop() {
	char *line;
	char **args;
	int status;

	do {
		printf("> ");
		line = fshell_read_line();
		args = fshell_split_line(line);
		status = fshell_execute(args);

		free(line);
		free(args);

	} while (status);
}

int main(int argc, char** argv) {
	// Load config files

	// Run command loop
	fshell_loop();

	// Perform any shutdown / cleanup

	return EXIT_SUCCESS;
}
