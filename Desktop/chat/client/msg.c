#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "macro.h"
#include "msg.h"





void clear_buff(char *buff, int dim)
{
	for(int i = 0; i < dim; i++)
		buff[i] = '\0';

}



void scelta(char *buff_tx, char *room, int *num)
{
	

	if(strncmp(buff_tx, "/w", 2) == 0){
		compila1(buff_tx, room);
	}
	else if(strncmp(buff_tx, "/g", 2) == 0){
		compila1(buff_tx, room);
	}
	else if(strncmp(buff_tx, "/", 1) == 0){
		compila3(buff_tx);
	}
	else if(strncmp(buff_tx, "#", 1) == 0){
		compila4(buff_tx);
	}
	else *num = compila2(buff_tx, room);
}

int compila1(char *buff_tx, char *room)
{
	int i;

	buff_tx[2] = SEP[0];

	for(i=0; i<DIM_BUFF_W && buff_tx[i] != ' '; i++)
	{
		if(i < DIM_ROOM) room[i] = buff_tx[i];
		else return -1;                                  //se supero la dimensione massima della stanza ritorno con errore per superamento del limite del nome.
	}

	buff_tx[i] = SEP[0];

	return 0;

}

int compila2(char *buff_tx, char *room)
{
	char temp[DIM_BUFF_W+1];
	int i, j;

	clear_buff(temp, DIM_BUFF_W+1);

	for(i=0; i<DIM_BUFF_W && room[i] != '\0'; i++)
		temp[i] = room[i];

	temp[i] = SEP[0];

	for(i += 1, j = 0; i<DIM_BUFF_W && buff_tx[j] != '\0'; i++, j++)
		temp[i] = buff_tx[j];
	
	clear_buff(buff_tx, DIM_BUFF_W+1);
	strcpy(buff_tx, temp);

	return i;
}

int compila3(char *buff_tx)
{
	for(int i=0; i<DIM_BUFF_W; i++)
	{
		if(buff_tx[i] == ' ')
		{
			buff_tx[i] = SEP[0];
			return 0;                                 
		}
	}

	return -1;

}

void compila4(char *buff_tx)
{
	if(strncmp(buff_tx, "#BRDC", 5) == 0)
		buff_tx[5] = SEP[0];
	else{
		for(int i=0; i<DIM_BUFF_W; i++)
		{
			if(buff_tx[i] == ' ')
				buff_tx[i] = SEP[0];                                 
		}
	}

}
