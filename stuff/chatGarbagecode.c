//chat

//server
/*
		  if (write(tdL.client, &clientAnswer,
sizeof(int)) <=
0) {
				 printf("[Thread %d] ", tdL.idThread);
				 perror("[Thread]Write error to
client.\n");
		  }

		  else
				 printf("[Thread %d]Mesajul a fost
trasmis cu
succes.\n",
						tdL.idThread);
		  printf("[Thread %d]Answer for command %d\n",
tdL.idThread,
				 clientAnswer);*/

//client
/*  pid_t readSlave;
if ((readSlave = fork()) < 0) {
perror("[client]Fork error ! fork(). \n");
return errno;
}
if (readSlave == 0) {
int serverResult;
if (read(socketConnect, &serverResult,
sizeof(int)) <= 0)
{
perror("[client]Read error from server !
\n");
return errno;
}
printf("[client]Result = %d \n", serverResult);
exit(1);
} // readSlave - client
else {
wait(NULL);

} */
