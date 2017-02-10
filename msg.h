#ifndef _MSG_H
#define _MSG_H

void gestionemsg(int fd_num, char *buffer, int nbyte, LISTA *list, ROOM *list_room);
void clear_buff(char *buff, int dim);
int whisper(int sock, char *buff, int byte, LISTA l);
int group(int sock, char *buff, int byte, LISTA l, ROOM list_room);
void broadcast_msg(LISTA list, char *buffer);

#endif

