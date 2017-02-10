#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "macro.h"
#include "list.h"
#include "msg.h"
#include "connect.h"

socklen_t addr_len;

int server_ipc(void){
	int sock_server;
	struct sockaddr_in server_addr;
	

	if((sock_server = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        	perror("Errore nella creazione del socket");
        	return -1;
    	}
    
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    	server_addr.sin_port = htons(NUM_PORT);
    	addr_len = (socklen_t) sizeof(server_addr);
    
    	if((bind(sock_server, (struct sockaddr *) &server_addr, addr_len)) == -1){
       		perror("errore nella bind");
        	return -2;
    	}
    
    	getsockname(sock_server, (struct sockaddr*) &server_addr, &addr_len);
    
    	printf("porta del server: %d\n", ntohs(server_addr.sin_port));
    
    	listen(sock_server, 5);

	return sock_server;
}

int start_server(int sock_server, LISTA *list, ROOM *list_room){

	struct sockaddr_in client_addr;
    	int sock_client, flag = 1, i, old_sock, fd_num, nbyte = -1;
    	char buff_rx[DIM_BUFF_R+1]; //buff_tx[DIM_BUFF_W+1];
    	LISTA tmp;
    	fd_set readfd, writefd, testfd_r, testfd_w;

    	FD_ZERO(&readfd);
    	FD_ZERO(&writefd);
    
    	FD_SET(sock_server, &readfd);
    	//FD_SET(sock_server, &writefd);   //attualmente non ha senso che il server invii messaggi. solo il sock del client deve inviare.



    	while(flag){

		testfd_r = readfd;
     		testfd_w = writefd;
	
		clear_buff(buff_rx, DIM_BUFF_R+1);
		//clear_buff(buff_tx, DIM_BUFF_W+1);

		if(select(FD_SETSIZE, &testfd_r, &testfd_w, (fd_set *) 0, (struct timeval *) 0) < 1){
			printf("errore nella select.\n");
			return -1;
		}

		for(fd_num = 0; fd_num < FD_SETSIZE; fd_num++){

			if(FD_ISSET(fd_num, &testfd_r)){
			
				if(fd_num == sock_server){
				
					sock_client = accept(sock_server, (struct sockaddr *) &client_addr, &addr_len);

					if(sock_client < 0){
						printf("errore nell'accettare il client.\n");  perror("accept");
						continue;
					}

				
					if(verifica(sock_client, list, &old_sock) > 0){
						FD_SET(sock_client, &readfd);
    				 		FD_SET(sock_client, &writefd);

						if(old_sock > 0){
							FD_CLR(old_sock, &readfd);
							FD_CLR(old_sock, &writefd);
							close(old_sock);
						}
					}
					else{
						printf("accesso negato.\n");
						close(sock_client);
					}
				}
				else{
				
					ioctl(fd_num, FIONREAD, &nbyte);  //restituisce il numero di byte che posso leggere dalla fd_num in nbyte

					if(nbyte == 0){ printf("chiudo il socket del client\n");
						FD_CLR(fd_num, &readfd);
						FD_CLR(fd_num, &writefd);
					
						clear_room(fd_num, list_room);
						tmp = ricerca_sock(*list, fd_num);
						tmp->info.on = 0;
						tmp->info.sock = 0;

						close(fd_num);
					}
					else{
					
						//read(fd_num, buff_rx, DIM_BUFF_R);
						read(fd_num, buff_rx, nbyte);
        					printf("stringa ricevuta: %s\n", buff_rx);
						gestionemsg(fd_num, buff_rx, nbyte, list, list_room);
					}

				}		
				

			}

			/*if(FD_ISSET(fd_num, &testfd_w)){
				printf("invio il messaggio\n");
				//ioctl(fd_num, FIONWRITE, &nbyte);  //restituisce il numero di byte che posso scrivere nel fd_num in nbyte
				ioctl(fd_num, FIONREAD, &nbyte);

				if(nbyte < DIM_BUFF_W) continue;
				else{
					write(fd_num, "ciao sono il server, ti saluto.\n", DIM_BUFF_W);
				}

			}*/

		}//fine for
        
    	} //fine while

    
    close(sock_server);

}

int send_msg(int sock,char *buff, int num){ printf("messaggio inviato: %s...%d\n", buff, num);
	return write(sock, buff, num);
}

int verifica(int sock, LISTA *list, int *old_sock){
	char buff[DIM_BUFF_R];
	int nbyte, i, j;
	info_type infol;

	clear_buff(buff, DIM_BUFF_R);

	nbyte = read(sock, buff, DIM_BUFF_R);

	if(buff[0] == '/' && buff[1] == 'l' && buff[2] == 'o' && buff[3] == 'g'){

		for(j = 0, i = 5; i < nbyte && buff[i] != '\0'; i++)
		{
			if(buff[i] != SEP[0] && j == 0)
				infol.nick[i-5] = buff[i]; 
			else if(j > 0) infol.pass[i-j] = buff[i];
			else
			{
				infol.nick[i-5] = '\0'; 
				j = i+1; 
			}
		}

		infol.pass[i-j] = '\0';


		LISTA c1;
		
		if((c1 = ricerca_user(*list, infol)) == NULL)
		{
			printf("client non registrato\n");
			send_msg(sock, "client non registrato\n", 22);
		}
		else
		{
			if(log_in(c1->info, infol))
			{
				printf("login effettuato\n");
				send_msg(sock, "ok", 3);
				
				if(c1->info.on == 1) *old_sock = c1->info.sock;
				else *old_sock = 0;

				c1->info.sock = sock;
				c1->info.on = 1;

				return 1;
			}
			else
			{
				printf("nickname o password errati\n");
				send_msg(sock, "nickname o password errati\n", 27);
			}
		}

		

	}
	else return 0;

	return -1;

}



