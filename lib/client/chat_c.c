#include "vsoc.h"

enum
{
	USR,
	USR2,
	SEP,
	MSG
};

void activeChat(int sC, const char *room)
{
	WINDOW *winput, *woutput;
	int winrows, wincols, i = 0, inChar;

	char inMesg[TEXT_LEN], outMesg[TEXT_LEN], user[SHORT_LEN];
	memset(inMesg, 0, TEXT_LEN);
	memset(outMesg, 0, TEXT_LEN);
	memset(user, 0, SHORT_LEN);

	initscr();
	raw();
	getmaxyx(stdscr, winrows, wincols);
	winput = newwin(3, wincols, winrows - 3, 0);
	woutput = newwin(winrows - 3, wincols, 0, 0);
	keypad(winput, true);
	scrollok(woutput, true);
	scrollok(winput, true);

	if (has_colors() == TRUE)
	{
		start_color();
		init_pair(USR, COLOR_RED, COLOR_BLACK);
		init_pair(USR2, COLOR_GREEN, COLOR_BLACK);
		init_pair(SEP, COLOR_YELLOW, COLOR_BLACK);
		init_pair(MSG, COLOR_WHITE, COLOR_BLACK);
	}


	fd_set all, read_fds;
	FD_ZERO(&all);
	FD_ZERO(&read_fds);
	FD_SET(0, &all);
	FD_SET(sC, &all);

	wprintw(woutput, "	Welcome to room '%s' \n	/online to see what users are in this room !\n	Use /quit to exit or ESC !\n\n", room);
	wrefresh(woutput);
	wrefresh(winput);
	signal(SIGWINCH, NULL);

	while (true)
	{
		read_fds = all;

		if (select(sC + 1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select() error or forced exit !\n");
			endwin();
			break;
		}

		if (FD_ISSET(sC, &read_fds))
		{
			//interpreteaza date(mesaje / comenzi)
			memset(user, 0, SHORT_LEN);
			memset(inMesg, 0, TEXT_LEN);
			safePrefRead(sC, user);
			safePrefRead(sC, inMesg);

			switch (user[0])
			{
			case '>':
				strcpy(user, user + 1);
				wprintw(woutput, "\n%s entered the room '%s'!\n\n", user, room);
				wrefresh(woutput);
				wrefresh(winput);
				break;

			case '<':
				strcpy(user, user + 1);
				wprintw(woutput, "\n%s exited the room '%s' !\n\n", user, room);
				wrefresh(woutput);
				wrefresh(winput);
				break;

			case '!':
				safePrefWrite(sC, "/quit");
				delwin(winput);
				delwin(woutput);
				endwin();
				endwin();
				printf("You've been disconnected !\n");
				quitforce();
				return;

			case '@':
			{
				int onCount = atoi(inMesg);
				wprintw(woutput, "\n	Online in room '%s': \n", room);
				wrefresh(woutput);

				for (i = 0; i < onCount; i++)
				{
					memset(user, 0, SHORT_LEN);
					safePrefRead(sC, user);
					wprintw(woutput, "		%s\n", user);
					wrefresh(woutput);
				}
				i = i - onCount; // fix pentru null invizibil
				wprintw(woutput, "\n");
				wrefresh(woutput);
				wrefresh(winput);
			}
			break;

			default:

				if (has_colors() == TRUE)
					wattron(woutput, COLOR_PAIR(USR2));
				wprintw(woutput, "%*.*s", 16, 16, user);
				if (has_colors() == TRUE)
					wattroff(woutput, COLOR_PAIR(USR2));

				if (has_colors() == TRUE)
					wattron(woutput, COLOR_PAIR(SEP));
				wprintw(woutput, " | ");
				if (has_colors() == TRUE)
					wattroff(woutput, COLOR_PAIR(SEP));

				if (has_colors() == TRUE)
					wattron(woutput, COLOR_PAIR(MSG));
				wprintw(woutput, "%s\n", inMesg);
				if (has_colors() == TRUE)
					wattroff(woutput, COLOR_PAIR(MSG));


				wrefresh(woutput);
				wrefresh(winput);
				break;
			}
		}


		if (FD_ISSET(0, &read_fds))
		{
			inChar = wgetch(winput);

			if (inChar == 27)
			{
				safePrefWrite(sC, "/quit");
				break;
			}

			if (inChar == KEY_UP || inChar == KEY_DOWN || inChar == KEY_LEFT || inChar == KEY_RIGHT)
				continue;

			if (inChar == KEY_BACKSPACE || inChar == KEY_DC || inChar == 127)
			{
				wdelch(winput);
				wrefresh(winput);
				if (i != 0)
				{
					outMesg[i - 1] = 0;
					i--;
				}
			}
			else
			{
				if (i != TEXT_LEN)
				{
					outMesg[i] = (char)inChar;
					i++;
				}
			}


			if (outMesg[i - 1] == '\n')
			{
				outMesg[i - 1] = 0;
				i = 0;

				if (outMesg[0] == 0)
					continue;

				if (strcmp(outMesg, "/quit") == 0)
				{
					safePrefWrite(sC, outMesg);
					break;
				}

				safePrefWrite(sC, outMesg);
				delwin(winput);
				winput = newwin(3, wincols, winrows - 3, 0);
				keypad(winput, true);
				wrefresh(winput);
				memset(outMesg, 0, TEXT_LEN);
			}
		}
	}

	delwin(winput);
	delwin(woutput);
	endwin();
	endwin();
}

//chat end ---------------------------------------------------------
