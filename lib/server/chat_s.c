#include "vsoc.h"

void activeChat(int sC, char *currentUser, const char *room)
{
	bool run = true;
	char mesg[TEXT_LEN], actionUser[LONG_LEN];


	sprintf(actionUser, ">%s", currentUser);
	dbSendMsgToRoom(actionUser, room, "");

	while (run)
	{
		memset(actionUser, 0, LONG_LEN);
		memset(mesg, 0, TEXT_LEN);
		safePrefRead(sC, mesg);

		if (strcmp(mesg, "/quit") == 0)
		{
			run = false;
			sprintf(actionUser, "<%s", currentUser);

			dbSendMsgToRoom(actionUser, room, "");
			continue;
		}

		if (strcmp(mesg, "/online") == 0)
		{
			int onCount = dbInChatCount(room);
			memset(actionUser, 0, LONG_LEN);
			memset(mesg, 0, TEXT_LEN);

			sprintf(actionUser, "@");
			sprintf(mesg, "%d", onCount);

			safePrefWrite(sC, actionUser);
			safePrefWrite(sC, mesg);
			dbInChat(room, sC);
			continue;
		}


		safePrefWrite(sC, currentUser);
		safePrefWrite(sC, mesg);
		dbSendMsgToRoom(currentUser, room, mesg);
	}

	return;
}
