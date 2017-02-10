#ifndef _ADMIN_H
#define _ADMIN_H

int comand(int sock, char *cmd, char *buffer, int nbyte, LISTA *list, ROOM *list_room);
int kick_user(LISTA admin, ROOM *list_room, LISTA l, char *buffer, int nbyte);
int ban_user(LISTA admin, char *nick, int byte, LISTA *list, ROOM *list_room, int cmd);
int ch_mod(LISTA admin, char *nick, int byte, LISTA *list, ROOM *list_room, int cmd);

#endif

