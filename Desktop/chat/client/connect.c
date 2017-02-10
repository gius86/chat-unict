#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "macro.h"
#include "msg.h"
#include "connect.h"



int log_in(char *nick, char *pass){
    struct sockaddr_in server_addr;
    int sock;
    struct hostent *hp;
    char buff[MAX_NICK+MAX_PASS+5];

    clear_buff(buff, MAX_NICK+MAX_PASS+5);

    hp = gethostbyname(SERVER_NAME);
    
    if(hp == NULL){
        perror("errore nell'acquisire il nome del server");
        return -1;
    }
    
    server_addr.sin_addr = *(struct in_addr *) *hp->h_addr_list;
    server_addr.sin_family = AF_INET;
    
    server_addr.sin_port = htons(NUM_PORT);
    
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Errore nella creazione del socket");
        return -1;
    }
    
    if((connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr))) == -1){
        perror("Errore nella connessione col server");
        return -1;
    }

    strcat(buff, "/log");
    strcat(buff, SEP);
    strcat(buff, nick);
    strcat(buff, SEP);
    strcat(buff, pass);

    printf("\n%s\n",buff);

    write(sock, buff, strlen(buff)+1);

    clear_buff(buff, MAX_NICK+MAX_PASS+5);

    read(sock, buff, MAX_NICK+MAX_PASS+4);

    if(buff[0] == 'o' && buff[1] == 'k'){
	printf("login effetuato con successo.\n");

    	return sock;
	
    }

    printf("login fallito: %s\n", buff);
    close(sock);
    return -1;    

}

void start_client(int sock){

	char buff_tx[DIM_BUFF_W+1], buff_rx[DIM_BUFF_R+1], room[DIM_ROOM+1];
	fd_set readfd, testfd, writefd, testfd_w;
    	int fd_num, nbyte = -1, flag = 1;

    	clear_buff(room, DIM_ROOM+1);

    	FD_ZERO(&readfd);      
    	FD_SET(sock, &readfd);
    	FD_SET(fileno(stdin), &readfd); 

    	FD_ZERO(&writefd);      
    	FD_SET(sock, &writefd);      
    
    	while(flag){

		testfd = readfd;
		testfd_w = writefd;
   
		clear_buff(buff_rx, DIM_BUFF_R+1);
		clear_buff(buff_tx, DIM_BUFF_W+1);

		if(select(FD_SETSIZE, &testfd, &testfd_w, (fd_set *) 0, (struct timeval *) 0) < 1){
			printf("errore nella select.\n");
			exit(EXIT_FAILURE);
		}

		for(fd_num = 0; fd_num < FD_SETSIZE; fd_num++){

			if(FD_ISSET(fd_num, &testfd)){printf("fd: %d\n", fd_num);
		
				ioctl(fd_num, FIONREAD, &nbyte);  //restituisce il numero di byte che posso leggere dalla fd_num in nbyte

				//if(nbyte > DIM_BUFF_W) nbyte = DIM_BUFF_W;

				if(fd_num == sock){printf("1:%d\n", nbyte);
			
					if(nbyte == 0){
						printf("chiudo il socket.\n");
						FD_CLR(fd_num, &readfd);					
						close(fd_num);
						flag = 0;
					}
					else{
					
						read(fd_num, buff_rx, nbyte);
						if(strncmp(buff_rx, "#BAN", 4) == 0){
							write(fileno(stdout), buff_rx+5, nbyte-5);

							printf("chiudo il socket.\n");
							FD_CLR(fd_num, &readfd);					
							close(fd_num);
							flag = 0;
						
						}
						else write(fileno(stdout), buff_rx, nbyte);
					}

				}
				else{  printf("fd: %d\n", fd_num);
					if(nbyte > MAX_MSG) nbyte = MAX_MSG;
printf("2:%d\n", nbyte);
					read(fd_num, buff_tx, nbyte);
					scelta(buff_tx, room, &nbyte);
printf("2.1:%d...%s\n", nbyte, buff_tx);
int res =0;
					while(!res)printf("attendo per la scrittura: %d\n", (res = FD_ISSET(sock, &testfd_w)));
					write(sock, buff_tx, nbyte);				
				}
	clear_buff(buff_rx, DIM_BUFF_R+1);
	clear_buff(buff_tx, DIM_BUFF_W+1);
		}
		
	}//fine for
        
    } //fine while


    
    close(sock);

}
