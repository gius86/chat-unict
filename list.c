#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "macro.h"
#include "list.h"

int init_user(LISTA *list){

	info_type infol;
	FILE *fd;

	if((fd = fopen(UTENTI, "r")) == NULL){
		perror("errore nell'apertura del file");
		return -1;
    	}

    	for(int i = 0; i < MAX_CLIENT; i++){
		if((fscanf(fd, "%s %s %d %d", infol.nick, infol.pass, &infol.permessi, &infol.ban)) == EOF) break;
		infol.sock = 0;
		infol.on = 0;
    		list_in(list, infol);
    	}

    	fclose(fd);

    	printl(*list);
	return 0;
}

int init_room(ROOM *list_room){
	info_room infor;
    	FILE *fd;

	if((fd = fopen(STANZE, "r")) == NULL){
		perror("errore nell'apertura del file");
		return -1;
    	}

    	while((fscanf(fd, "%s%*c", infor.room)) != EOF){
		for(int j = 0; j < MAX_USER; j++)
			infor.user[j] = -1;
		list_in_room(list_room, infor);
    	}


    	fclose(fd);

    	printl_room(*list_room);

	return 0;

}

int list_in(LISTA *l, info_type i){
	LISTA aux, cor = *l, prec;

	if((aux = (LISTA)malloc(sizeof(NODO))) == NULL){
		printf("errore allocazione memoria.\n");
		return -1;
	}


	aux->info = i;
	aux->next = NULL;
	

	if(*l == NULL) *l = aux;
	else{
		while(cor != NULL){
			prec = cor;
			cor = cor->next;
		}
		
		prec->next = aux;
	}

	return 0;
}

void printl(LISTA l){
	while(l != NULL){
		printf("%s %s %d %d\n", l->info.nick, l->info.pass, l->info.permessi, l->info.ban);
		l = l->next;
	}
}

int list_in_room(ROOM *l, info_room i){
	ROOM aux, cor = *l, prec;

	if((aux = (ROOM)malloc(sizeof(NODO_ROOM))) == NULL){
		printf("errore allocazione memoria.\n");
		return -1;
	}


	aux->info = i;
	aux->next = NULL;
	
	if(*l == NULL) *l = aux;
	else{
		while(cor != NULL){
			prec = cor;
			cor = cor->next;
		}
		
		prec->next = aux;
	}

	return 0;
}

void printl_room(ROOM l){
	while(l != NULL){
		printf("%s\n", l->info.room);
		l = l->next;
	}
}

ROOM ricerca_room(ROOM l, char *r){

	while(l != NULL){
		if((strcmp(l->info.room, r)) == 0) return l;
		else l = l->next;
	}
	
	return NULL;
}


LISTA ricerca_user(LISTA l, info_type i){

	while(l != NULL){
		if((strcmp(l->info.nick, i.nick)) == 0) return l;
		else l = l->next;
	}
	
	return NULL;
}

LISTA ricerca_sock(LISTA l, int sock){

	while(l != NULL){
		if(l->info.sock == sock) return l;
		else l = l->next;
	}
	
	return NULL;
}

int log_in(info_type user, info_type dati){

	if((strcmp(user.pass, dati.pass)) == 0 && user.ban == 0)
		return 1;
	
	return 0;
}

int save_l(LISTA l){
	FILE *fd;

	if((fd = fopen(UTENTI, "w")) == NULL){
		perror("errore nell'apertura del file");
		return 1;
    	}

    	while(l != NULL){
		fprintf(fd, "%s %s %d %d\n", l->info.nick, l->info.pass, l->info.permessi, l->info.ban);
		l = l->next;
	}

    	fclose(fd);

	return 0;
}

int delate(ROOM *list_room, char *room){
	ROOM cor, prec;

	prec = NULL;
	cor = *list_room;

	while((strcmp(cor->info.room, room) != 0) && cor != NULL){
		prec = cor;
		cor = cor->next;
	}

	if(prec == NULL) return -1;
	else if(prec == cor) *list_room = prec->next;
	else prec->next = cor->next;

	free(cor);

	return 0;

}

void clear_room(int sock, ROOM *list_room){

	ROOM r = *list_room;

	while(r != NULL){
		leave_room(sock, r->info.room, strlen(r->info.room)+2, list_room);
		r = r->next;
	}
}


int join_room(int sock, char *room, int byte, ROOM *list_room){
	ROOM r;
	info_room inf;

	room[byte-1] = '\0';

	if((r = ricerca_room(*list_room, room)) != NULL){
		for(int i = 0; i < MAX_USER; i++){
			if(r->info.user[i] == -1){
				r->info.user[i] = sock;
				return 0;
			}
		}		
		
		return -1;		
	}
	

	strcpy(inf.room, room);

	for(int j = 0; j < MAX_USER; j++)
		inf.user[j] = -1;

	inf.user[0] = sock;
	
	return list_in_room(list_room, inf);	
}

int leave_room(int sock, char *room, int byte, ROOM *list_room){
	ROOM r;

	room[byte-1] = '\0';

	if((r = ricerca_room(*list_room, room)) != NULL){
		for(int i = 0; i < MAX_USER; i++){
			if(r->info.user[i] == sock){
				r->info.user[i] = -1;
				break;
			}
		}
	}
	else return -1;
	

	for(int i = 0; i < MAX_USER; i++)
		if(r->info.user[i] != -1) return 1;


	return delate(list_room, room);
}

