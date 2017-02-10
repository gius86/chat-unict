#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "macro.h"
#include "list.h"
#include "connect.h"
#include "msg.h"
#include "admin.h"


int comand(int sock, char *cmd, char *buffer, int nbyte, LISTA *list, ROOM *list_room){
	LISTA user;
	

	user = ricerca_sock(*list, sock);

	if(user->info.permessi == 1)
	{
		if(strcmp(cmd, BROADCAST) == 0){
			broadcast_msg(*list, buffer);
			return 0;
		}
		else if(strcmp(cmd, KICK) == 0){
			kick_user(user, list_room, *list, buffer, nbyte);
		}
		else if(strcmp(cmd, BAN) == 0){
			ban_user(user, buffer, nbyte, list, list_room, 0);
		}
		else if(strcmp(cmd, UNBAN) == 0){
			ban_user(user, buffer, nbyte, list, list_room, 1);
		}
		else if(strcmp(cmd, ADM) == 0){
			ch_mod(user, buffer, nbyte, list, list_room, 1);
		}
		else if(strcmp(cmd, UNADM) == 0){
			ch_mod(user, buffer, nbyte, list, list_room, 0);
		}
		else printf("comando non riconosciuto.\n");

	}
	else{
		printf("user not admin\n.");
		send_msg(sock, "permesso negato.\n", 18);
		return -1;
	}
}



int kick_user(LISTA admin, ROOM *list_room, LISTA l, char *buffer, int nbyte){
	char room[DIM_ROOM+1];
	char system[DIM_BUFF_W+1];
	info_type infol;
	LISTA user;
	ROOM r;

	clear_buff(infol.nick, MAX_NICK+1);
	clear_buff(room, DIM_ROOM+1);
	clear_buff(system, DIM_BUFF_W+1);

	for(int i=0, j=0; i < nbyte && buffer[i] != '\n'; i++)
	{
		if(buffer[i] != SEP[0] && j == 0 && i < MAX_NICK)
			infol.nick[i] = buffer[i];
		else if(j != 0 && (i-j) < DIM_ROOM)
			room[i-j] = buffer[i];
		else j = i+1;
	}

	if((user = ricerca_user(l, infol)) == NULL)
	{
		printf("client non registrato\n");
		send_msg(admin->info.sock, "client non registrato\n", 23);
		return -1;
	}
	else
	{
		if((r = ricerca_room(*list_room, room)) == NULL)
		{
			printf("stanza non trovata\n");
			send_msg(admin->info.sock, "stanza non trovata\n", 20);
			return -2;
		}
		else
		{
			strcat(system, "[SYSTEM]: <");
			strcat(system, admin->info.nick);
			strcat(system, ">");
			strcat(system, MSG_KICK_USER);
			strcat(system, "<");
			strcat(system, room);
			strcat(system, ">.\n");
			
			leave_room(user->info.sock, room, strlen(room)+2, list_room);

			send_msg(user->info.sock, system, strlen(system)+1);

			system[0] = '\0';
			strcat(system, "[SYSTEM]: <");
			strcat(system, admin->info.nick);
			strcat(system, ">");
			strcat(system, MSG_KICK_ROOM);
			strcat(system, "<");
			strcat(system, user->info.nick);
			strcat(system, ">.\n");

			for(int j = 0; j < MAX_USER; j++){
				if(r->info.user[j] != -1)
					send_msg(r->info.user[j], system, strlen(system)+1);
			}
		}
		
	}

	
}

int ban_user(LISTA admin, char *nick, int byte, LISTA *list, ROOM *list_room, int cmd){
	LISTA user;
	info_type infol;
	char system[DIM_BUFF_W+1];

	clear_buff(system, DIM_BUFF_W+1);

	nick[byte-1] = '\0';	

	strcpy(infol.nick, nick);

	if((user = ricerca_user(*list, infol)) == NULL)
	{
		printf("client non registrato\n");
		send_msg(admin->info.sock, "client non registrato\n", 23);
		return -1;
	}
	else
	{
		if(cmd == 0){
			if(!user->info.ban){
				user->info.ban = 1;

				strcat(system, "[SYSTEM]: <");
				strcat(system, user->info.nick);
				strcat(system, ">");
				strcat(system, MSG_BAN);
				strcat(system, "<");
				strcat(system, admin->info.nick);
				strcat(system, ">.\n");
		
				broadcast_msg(*list, system);

				system[0] = '\0';
				strcat(system, "#BAN:[SYSTEM]: sei stato bannato da");
				strcat(system, "<");
				strcat(system, admin->info.nick);
				strcat(system, ">.\n");
		
				send_msg(user->info.sock, system, strlen(system)+1);
		
				return 0;
			}
			else{
				send_msg(admin->info.sock, "client già bannato.\n", 21);
				return 0;
			}
		}
		else{
			user->info.ban = 0;
			send_msg(admin->info.sock, "ban revocato.\n", 15);
			return 0;
		}
	}
	
}

int ch_mod(LISTA admin, char *nick, int byte, LISTA *list, ROOM *list_room, int cmd){
	LISTA user;
	info_type infol;
	char system[DIM_BUFF_W+1];

	clear_buff(system, DIM_BUFF_W+1);

	nick[byte-1] = '\0';	

	strcpy(infol.nick, nick);

	if((user = ricerca_user(*list, infol)) == NULL)
	{
		printf("client non registrato\n");
		send_msg(admin->info.sock, "client non registrato\n", 23);
		return -1;
	}
	else
	{
		if(!user->info.ban){
			user->info.permessi = cmd;

			send_msg(admin->info.sock, "permessi client modificati.\n", 29);
		
		}
		else	send_msg(admin->info.sock, "client bannato.\n", 17);
						
	}
	
}



