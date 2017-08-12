#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8888
#define CLIENTE "192.168.1.14"

//variáveis do socket
struct sockaddr_in s_destino, s_origem;
int s_socket, sockfd_cliente, len, lenRemoto, retornoBind;

//Método  que inicializa os navios em posições aleatórias entre 1 e 5.
void iniciaNavios(int navios[][2]){
        srand(time(NULL));
        int navio, anterior;

        for(navio=0 ; navio < 3 ; navio++){
        	//array navios que guarda as posições aleatórias a qual os navios irão estar no tabuleiro.
            navios[navio][0]= rand()%5;
            navios[navio][1]= rand()%5;
           // printf("L: %d, C: %d\n",navios[navio][0],navios[navio][1]);

            //agora vamos checar se esse par não foi sorteado
            //se foi, só sai do "do...while" quando sortear um diferente
            for(anterior=0 ; anterior < navio ; anterior++){
                if( (navios[navio][0] == navios[anterior][0])&&(navios[navio][1] == navios[anterior][1]) )
                    do{
                        navios[navio][0]= rand()%5;
                        navios[navio][1]= rand()%5;
                    }while( (navios[navio][0] == navios[anterior][0])&&(navios[navio][1] == navios[anterior][1]) );
            }

        }
    }

//Método que verifica se o tiro foi dado exatamente na posição onde se encontra um navio.
//Se acertou retorna 1, se não retorna 0.
int acertou(int tiro[2], int navios[][2]){
    int navio;
        for(navio=0 ; navio < 3 ; navio++){
            if(tiro[0]==navios[navio][0] && tiro[1]==navios[navio][1]){
                //printf("Você acertou o tiro (%d,%d)\n",tiro[0]+1,tiro[1]+1);
                return 1;
            }
        }
        return 0;
    }

main(){
	int tabuleiro[5][5];
    int navios[3][2];
	int tiro[2];
    int tentativas=0,acertos=0;
     
	//crição do socket.    
	s_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(s_socket < 0){
		perror("Erro ao criar socket");
    }
    len = sizeof(s_destino);
	bzero(&(s_destino.sin_zero),0);
    s_destino.sin_family = AF_INET;
    s_destino.sin_port = htons(PORT);
    s_destino.sin_addr.s_addr = inet_addr(CLIENTE);
	retornoBind = bind(s_socket, (struct sockaddr*) &s_destino, len);
    if(retornoBind < 0){
		printf("Erro ao associar endereco e porta");
    }
    listen(s_socket, 1);
    printf("\n");	
	printf("*******************************************************************************\n");
	printf("******************************** BATALHA NAVAL ********************************\n");
	printf("********************************************************Por Carol e Viviani****\n");
    
    //chama o método para iniciar os navios.
    iniciaNavios(navios);
    while(1){
		sockfd_cliente = accept( s_socket, (struct sockaddr *) &s_destino, &lenRemoto);
		
		int tiro[2];
		int resp;
		do{
			//recebe do cliente as informações de tiro[0] (linha) e tiro[1] (coluna).
			recv(sockfd_cliente, &tiro[0], sizeof(tiro[0]), 0);
			recv(sockfd_cliente, &tiro[1], sizeof(tiro[1]),0);
			//printf("Linha: %d\n", tiro[0]);
			//printf("Coluna: %d\n", tiro[1]);
			
			//variável "resp" recebe o método "acertou" para enviar para o cliente.
			resp=acertou(tiro,navios);
			//printf("Resp: %d\n",resp);
			
			//envia para o cliente o retorno (0 ou 1) de acertou e as posições dos navios
			send(sockfd_cliente, &resp,sizeof(resp), 0);
			send(sockfd_cliente, &navios, sizeof(navios), 0);
			//recebe do cliente a quantidade de acertos. 
			recv(sockfd_cliente, &acertos, sizeof(acertos), 0);
		}while(acertos!=3);
		//close(sockfd_cliente);
    }

}
