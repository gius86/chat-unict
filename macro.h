#ifndef _MACRO_H
#define _MACRO_H

#define MAX_NICK 15
#define MAX_PASS 15
#define DIM_ROOM 15
#define DIM_BUFF_R MAX_NICK+DIM_ROOM+2+40
#define DIM_BUFF_W DIM_BUFF_R+DIM_ROOM+MAX_NICK


#define MAX_USER 10
#define DIM_NICK 15
#define DIM_PASS 15
#define MAX_CLIENT 10
#define DIM_CMD 15

#define NUM_PORT 38938

#define UTENTI "permessi"
#define STANZE "stanze"

#define SEP ":"
#define MSG_KICK_USER " ti ha kickato dalla stanza "
#define MSG_KICK_ROOM " ha kickato "
#define MSG_BAN " è stato bannato da "

//macro
#define BROADCAST "#BRDC"
#define KICK "#KICK"
#define BAN "#BAN"
#define UNBAN "#UNBAN"
#define ADM "#ADM"
#define UNADM "#UNADM"

typedef struct ir{
	char room[DIM_ROOM+1];
	int user[MAX_USER];
}info_room;

typedef struct r{
	info_room info;
	struct r *next;
}*ROOM, NODO_ROOM;

typedef struct il{
	char nick[DIM_NICK];
	char pass[DIM_PASS];
	int permessi;			//0: utente semplice. 1:admin.
	int ban;			//0: non bannato. 1: bannato
	int sock;
	int on;				//0: offline. 1:online.
}info_type;

typedef struct l{
	info_type info;
	struct l *next;
}*LISTA, NODO;

#endif
