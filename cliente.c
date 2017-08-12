#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER "192.168.1.14"
#define PORT 8888

//vari�veis do socket
int s_socket;
struct sockaddr_in destino;
int conexao, len;

//M�todo que inicializa todo o tabuleiro 5x5 com o valor -1.
void inicializaTabuleiro(int tabuleiro[][5]){
    int linha, coluna;
        for(linha=0 ; linha < 5 ; linha++ )
            for(coluna=0 ; coluna < 5 ; coluna++ )
                tabuleiro[linha][coluna]=-1;
}

//M�todo que imprime a matriz (tabuleiro).
void mostraTabuleiro(int tabuleiro[][5]){

    int linha, coluna;

        printf("\t1 \t2 \t3 \t4 \t5");
        printf("\n");
		
        for(linha=0 ; linha < 5 ; linha++ ){
        	//imprime os n�meros de 1 a 5 na matriz.
            printf("%d",linha+1);
            for(coluna=0 ; coluna < 5 ; coluna++ ){
            	//se essa posi��o no tabuleiro for igual a -1 ent�o preenche a posi��o com "~"
                if(tabuleiro[linha][coluna]==-1){
                    printf("\t~");
                //se essa posi��o no tabuleiro for igual a 0 ent�o preenche a posi��o com "o"
                }else if(tabuleiro[linha][coluna]==0){
                    printf("\to");
                //se essa posi��o no tabuleiro for igual a 1 ent�o preenche a posi��o com "X" 
                }else if(tabuleiro[linha][coluna]==1){
                    printf("\tX");
                }
            }
            printf("\n");
        }
    }
	
//M�todo que pede para o cliente a linha e a coluna que ele deseja atirar. 
//Guarda-se essas informa��es em um array de 2 posi��es.
void darTiro(int tiro[2]){
		
        printf("Linha: ");
        scanf("%d",&tiro[0]);
        tiro[0]--;

        printf("Coluna: ");
        scanf("%d",&tiro[1]);
        tiro[1]--;
}

//M�todo que informa para o cliente dicas para a pr�xima jodada.
void dica(int tiro[2], int navios[][2], int tentativa){
        int linha=0,
            coluna=0,
            fila;
            
        //conta quantos navios h� na linha tiro[0]
        for(fila=0 ; fila < 3 ; fila++){
            if(navios[fila][0]==tiro[0])
                linha++;
            if(navios[fila][1]==tiro[1])
                coluna++;
        }
        
        printf("\nDica %d: \nlinha %d -> %d navios\ncoluna %d -> %d navios\n",tentativa,tiro[0]+1,linha,tiro[1]+1,coluna);
}

//M�todo que atualiza o tabuleiro com base nas tentativas de tiro do cliente.
void alteraTabuleiro(int tiro[2], int navios[][2], int tabuleiro[][5], int resp){
		//vari�vel "resp" corresponde ao m�todo "acertou" do servidor.
        if(resp==1)
            tabuleiro[tiro[0]][tiro[1]]=1;
        else
            tabuleiro[tiro[0]][tiro[1]]=0;
    }


main(){
    int tabuleiro[5][5];
    int navios[3][2];
    int tiro[2];
    int tentativas=0,acertos;

	//cria��o do socket.
	s_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(s_socket <0){
		perror("Erro na cria��o de socket!");
	}

    len = sizeof(destino);
    destino.sin_family = AF_INET;
    destino.sin_port = htons(PORT);
    destino.sin_addr.s_addr = inet_addr(SERVER);
    bzero(&(destino.sin_zero),0);

    conexao = connect(s_socket, (struct sockaddr * ) &destino, len);
    if(conexao < 0){
		perror("Erro na conex�o");
        close(s_socket);
	}
	
	//chama o m�todo para inicializar o tabuleiro.
    inicializaTabuleiro(tabuleiro);
	printf("\n");	
	printf("*******************************************************************************\n");
	printf("******************************** BATALHA NAVAL ********************************\n");
	printf("********************************************************Por Carol e Viviani****\n");
	printf("\n");
	
	//La�o de repeti��o para repetir essas a��es at� o cliente acertar a posi��o dos 3 navios.
	do{
		//chama o m�todo mostraTabuleiro
	    mostraTabuleiro(tabuleiro);
		printf("\n");
		//chama o m�todo darTiro
	    darTiro(tiro);
	    printf("\n");
	    //envia as informa��es de tiro[0] (linha) e tiro[1] (coluna) para o servidor.
	    send(s_socket, &tiro[0], sizeof(tiro[0]), 0);
	    send(s_socket, &tiro[1], sizeof(tiro[1]), 0);
        tentativas++;
		
		//vari�vel que corresponde ao m�todo "acertou" do servidor.
		int resp;
		//recebe do servidor o "acertou" (m�todo que retorna 0 ou 1) e as posi��es dos navios.
		recv(s_socket, &resp, sizeof(resp), 0);
		recv(s_socket, &navios, sizeof(navios), 0);
		//printf("Resp: %d\n",resp);
		
		//se o m�todo "acertou" do servidor retornou 1 ent�o:
		if(resp==1){
			printf("VOC� ACERTOU O TIRO (na linha %d, coluna %d)!\n",tiro[0]+1,tiro[1]+1);
            dica(tiro,navios,tentativas);
            printf("\n");
            acertos++;
            //envia para o servidor os acertos do cliente.
			send(s_socket, &acertos,sizeof(acertos), 0);
        }
        //se o m�todo "acertou" do servidor retornou 0 ent�o:
        else{
			printf("VOC� ERROU O TIRO, TENTE NOVAMENTE!\n");
			dica(tiro,navios,tentativas);
			printf("\n");
			//envia para o servidor os acertos do cliente.
			send(s_socket, &acertos, sizeof(acertos), 0);
		}
		//chama o m�todo que atualiza o tabuleiro de acordo com as novas informa��es.
		alteraTabuleiro(tiro,navios,tabuleiro,resp);
		
    }while(acertos!=3);
	
      printf("\nJOGO TERMINADO! Voc� acertou os 3 navios em %d tentativas.\n\n", tentativas);
      //mostra o tabuleiro final, j� atualizado.
      mostraTabuleiro(tabuleiro);
      printf("\n");
	
    exit(0);
}
