
#include "macro.h"
#include "list.c"
#include "msg.c"
#include "admin.c"
#include "connect.c"


int main(void){
    	LISTA list = NULL;
    	ROOM list_room = NULL;    	
	int sock_server;

	
    	if(init_user(&list) != 0) return 3;
	if(init_room(&list_room) != 0) return 4;    	

    	if((sock_server = server_ipc()) < 0) return 5;
	else start_server(sock_server, &list, &list_room);

       
    	return 0;
}





























