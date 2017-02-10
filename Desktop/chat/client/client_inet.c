#include <stdio.h>
#include <stdlib.h>


#include "macro.h"
#include "msg.c"
#include "connect.c"


int main(void){
	int sock = -1;
    	char nick[MAX_NICK+1], pass[MAX_PASS+1];
    
    	clear_buff(nick, MAX_NICK+1);
	clear_buff(pass, MAX_PASS+1);

   	 while(sock <= 0){
		printf("inserisci il nick:\n");
		fscanf(stdin, "%s%*c", nick);
        
        	printf("inserisci la password:\n");
		fscanf(stdin, "%s%*c", pass);

		sock = log_in(nick, pass);
	
    	} 

	start_client(sock);   
    
    return 0;
    
}









