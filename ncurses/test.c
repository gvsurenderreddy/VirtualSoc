#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <pthread.h>
#include <sqlite3.h>
#include <assert.h>
#include <stdio_ext.h>
#include <stdbool.h>
#include <ncurses.h>

static int winrows, wincols;
static WINDOW *winput, *woutput;

// chat part -----------------------------------------------------

void activeChat(int sC, const char *currentUser, const char *room)
{
	char inMesg[513], outMesg[513];
	char user[33];

	initscr();
	cbreak();
	getmaxyx(stdscr, winrows, wincols);
	winput = newwin(1, wincols, winrows - 1, 0);
	woutput = newwin(winrows - 1, wincols, 0, 0);
	keypad(winput, true);
	////box(winput, 0, 0);
	////box(woutput, 0, 0);
	scrollok(woutput, true);
	wrefresh(woutput);
	wrefresh(winput);


	fd_set all;
	fd_set read_fds;
	FD_ZERO(&all);
	FD_ZERO(&read_fds);
	FD_SET(0, &all);
	FD_SET(sC, &all);

	while (true)
	{
		read_fds = all;
		if (select(sC + 1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select() error !\n");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(sC, &read_fds))
		{
			memset(inMesg, 0, 513);
			safePrefRead(sC, user);
			safePrefRead(sC, inMesg);
			wprintw(woutput, "%s : %s\n", user, inMesg);
			wrefresh(woutput);
			wrefresh(winput);
		}
		if (FD_ISSET(0, &read_fds))
		{

			wgetnstr(winput, outMesg, 513);
			if (outMesg[0] == 0)
				continue;
			if (strcmp(outMesg, "/quitChat") == 0)
			{
				safePrefWrite(sC, outMesg);
				break;
			}
			safePrefWrite(sC, outMesg);
			delwin(winput);
			winput = newwin(1, wincols, winrows - 1, 0);
			keypad(winput, true);
			wrefresh(winput);
		}
	}

	delwin(winput);
	delwin(woutput);
	endwin();
	//nocbreak();




	return;
}



int main(int argc, char **argv)
{
	initscr();



	return 1;
}
