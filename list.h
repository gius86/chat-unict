#ifndef _LIST_H
#define _LIST_H

int init_user(LISTA *list);
int init_room(ROOM *list_room);
int list_in(LISTA *l, info_type i);
void printl(LISTA l);
LISTA ricerca_user(LISTA l, info_type i);
int log_in(info_type user, info_type dati);
int save_l(LISTA l);
LISTA ricerca_sock(LISTA l, int sock);
void printl_room(ROOM l);
int list_in_room(ROOM *l, info_room i);
ROOM ricerca_room(ROOM l, char *r);
int delate(ROOM *list_room, char *room);
void clear_room(int sock, ROOM *list_room);
int join_room(int sock, char *buff, int byte, ROOM *list_room);
int leave_room(int sock, char *room, int byte, ROOM *list_room);

#endif
