#ifndef _CONNECT_H
#define _CONNECT_H

int server_ipc(void);
int start_server(int sock_server, LISTA *list, ROOM *list_room);
int send_msg(int sock,char *buff, int num);
int verifica(int sock, LISTA *list, int *old_sock);

#endif
