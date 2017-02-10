#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "macro.h"
#include "list.h"
#include "connect.h"
#include "admin.h"
#include "msg.h"


void gestionemsg(int fd_num, char *buffer, int nbyte, LISTA *list, ROOM *list_room)
{
	char cmd[DIM_CMD+1];
	int i;

	clear_buff(cmd, DIM_CMD+1);

	for(i = 0; i < DIM_CMD && buffer[i] != SEP[0];i++)
		cmd[i] = buffer[i];

	i++;

	if(strcmp(cmd, "/w") == 0){
		if(whisper(fd_num, buffer+i, nbyte-i, *list) != 0){
			printf("messaggio non consegnato.\n");
			send_msg(fd_num, "messaggio non consegnato.\n", 26);
		}
	}
	else if(strcmp(cmd, "/g") == 0){
		if(group(fd_num, buffer+i, nbyte-i, *list, *list_room) != 0){
			printf("messaggio non consegnato.\n");
			send_msg(fd_num, "messaggio non consegnato.\n", 26);
		}
	}
	else if(strcmp(cmd, "/join") == 0){
		if(join_room(fd_num, buffer+i, nbyte-i, list_room) < 0){
			printf("stanza piena.\n");
			send_msg(fd_num, "stanza piena.\n", 15);
		}
		
		printl_room(*list_room);
	}
	else if(strcmp(cmd, "/leave") == 0){
		leave_room(fd_num, buffer+i, nbyte-i, list_room);
		printl_room(*list_room);
	}
	else if(strncmp(cmd, "#", 1) == 0){
		comand(fd_num, cmd, buffer+i, nbyte-i, list, list_room);
	}
	else printf("la stringa ricevuta è: %s. %d.\n", buffer, nbyte);	

}

void clear_buff(char *buff, int dim)
{
	for(int i = 0; i < dim; i++)
		buff[i] = '\0';

}

int whisper(int sock, char *buff, int byte, LISTA l){
	LISTA from, to;
	info_type infol;
	int i;
	char buff_tx[DIM_BUFF_W+1];

	clear_buff(buff_tx, DIM_BUFF_W+1);

	for(i = 0; i < byte && i < DIM_NICK && buff[i] != SEP[0]; i++)
		infol.nick[i] = buff[i];
	infol.nick[i] = '\0';

	if((to = ricerca_user(l, infol)) == NULL)
	{
		printf("client non registrato\n");
		send_msg(sock, "client non registrato\n", 23);
		return -1;
	}
	else
	{
		if(to->info.on == 1 && to->info.ban == 0){
			from = ricerca_sock(l, sock);
		
			strcat(buff_tx, "from [");
			strcat(buff_tx, from->info.nick);
			strcat(buff_tx, "]: ");
			strcat(buff_tx, buff+i+1);

			send_msg(to->info.sock, buff_tx, strlen(buff_tx)+1);
		}
		else
		{
			printf("user ofline\n");
			send_msg(sock, "user offline\n", 14);
			return -2;
		}
	}

	return 0;
}

int group(int sock, char *buff, int byte, LISTA l, ROOM list_room){
	LISTA from;
	ROOM r;
	int i, pres = 0;
	char buff_tx[DIM_BUFF_W+1];
	char room[DIM_ROOM+1];

	clear_buff(buff_tx, DIM_BUFF_W+1);
	clear_buff(room, DIM_ROOM+1);

	for(i = 0; i < byte && i < DIM_ROOM && buff[i] != SEP[0]; i++)
		room[i] = buff[i];
	

	if((r = ricerca_room(list_room, room)) == NULL)
	{
		printf("stanza non trovata\n");
		send_msg(sock, "stanza non trovata\n", 20);
		return -1;
	}
	else
	{
		for(int j = 0; j < MAX_USER; j++){
			if(r->info.user[j] == sock){
				pres = 1;
				break;
			}
		}

		if(!pres) return -2;

		from = ricerca_sock(l, sock);
		
		if(!from->info.ban){
			strcat(buff_tx, "[");
			strcat(buff_tx, room);
			strcat(buff_tx, "][");
			strcat(buff_tx, from->info.nick);
			strcat(buff_tx, "]: ");
			strcat(buff_tx, buff+i+1);

			for(int j = 0; j < MAX_USER; j++){
				if(r->info.user[j] != -1 && r->info.user[j] != sock)
					send_msg(r->info.user[j], buff_tx, strlen(buff_tx)+1);
			}
		}
		else return -3;
	}

	return 0;

}

void broadcast_msg(LISTA l, char *buffer){
	char temp[DIM_BUFF_W+1];

	clear_buff(temp, DIM_BUFF_W+1);

	strcat(temp, "[Broadcast]: ");
	strcat(temp, buffer);

	while(l != NULL){
		if(l->info.on == 1 && l->info.ban == 0)
			send_msg(l->info.sock, temp, strlen(temp)+1);

		l = l->next;
	}
}



