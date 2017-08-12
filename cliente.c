#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER "192.168.1.14"
#define PORT 8888

//variáveis do socket
int s_socket;
struct sockaddr_in destino;
int conexao, len;

//Método que inicializa todo o tabuleiro 5x5 com o valor -1.
void inicializaTabuleiro(int tabuleiro[][5]){
    int linha, coluna;
        for(linha=0 ; linha < 5 ; linha++ )
            for(coluna=0 ; coluna < 5 ; coluna++ )
                tabuleiro[linha][coluna]=-1;
}

//Método que imprime a matriz (tabuleiro).
void mostraTabuleiro(int tabuleiro[][5]){

    int linha, coluna;

        printf("\t1 \t2 \t3 \t4 \t5");
        printf("\n");
		
        for(linha=0 ; linha < 5 ; linha++ ){
        	//imprime os números de 1 a 5 na matriz.
            printf("%d",linha+1);
            for(coluna=0 ; coluna < 5 ; coluna++ ){
            	//se essa posição no tabuleiro for igual a -1 então preenche a posição com "~"
                if(tabuleiro[linha][coluna]==-1){
                    printf("\t~");
                //se essa posição no tabuleiro for igual a 0 então preenche a posição com "o"
                }else if(tabuleiro[linha][coluna]==0){
                    printf("\to");
                //se essa posição no tabuleiro for igual a 1 então preenche a posição com "X" 
                }else if(tabuleiro[linha][coluna]==1){
                    printf("\tX");
                }
            }
            printf("\n");
        }
    }
	
//Método que pede para o cliente a linha e a coluna que ele deseja atirar. 
//Guarda-se essas informações em um array de 2 posições.
void darTiro(int tiro[2]){
		
        printf("Linha: ");
        scanf("%d",&tiro[0]);
        tiro[0]--;

        printf("Coluna: ");
        scanf("%d",&tiro[1]);
        tiro[1]--;
}

//Método que informa para o cliente dicas para a próxima jodada.
void dica(int tiro[2], int navios[][2], int tentativa){
        int linha=0,
            coluna=0,
            fila;
            
        //conta quantos navios há na linha tiro[0]
        for(fila=0 ; fila < 3 ; fila++){
            if(navios[fila][0]==tiro[0])
                linha++;
            if(navios[fila][1]==tiro[1])
                coluna++;
        }
        
        printf("\nDica %d: \nlinha %d -> %d navios\ncoluna %d -> %d navios\n",tentativa,tiro[0]+1,linha,tiro[1]+1,coluna);
}

//Método que atualiza o tabuleiro com base nas tentativas de tiro do cliente.
void alteraTabuleiro(int tiro[2], int navios[][2], int tabuleiro[][5], int resp){
		//variável "resp" corresponde ao método "acertou" do servidor.
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

	//criação do socket.
	s_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(s_socket <0){
		perror("Erro na criação de socket!");
	}

    len = sizeof(destino);
    destino.sin_family = AF_INET;
    destino.sin_port = htons(PORT);
    destino.sin_addr.s_addr = inet_addr(SERVER);
    bzero(&(destino.sin_zero),0);

    conexao = connect(s_socket, (struct sockaddr * ) &destino, len);
    if(conexao < 0){
		perror("Erro na conexão");
        close(s_socket);
	}
	
	//chama o método para inicializar o tabuleiro.
    inicializaTabuleiro(tabuleiro);
	printf("\n");	
	printf("*******************************************************************************\n");
	printf("******************************** BATALHA NAVAL ********************************\n");
	printf("********************************************************Por Carol e Viviani****\n");
	printf("\n");
	
	//Laço de repetição para repetir essas ações até o cliente acertar a posição dos 3 navios.
	do{
		//chama o método mostraTabuleiro
	    mostraTabuleiro(tabuleiro);
		printf("\n");
		//chama o método darTiro
	    darTiro(tiro);
	    printf("\n");
	    //envia as informações de tiro[0] (linha) e tiro[1] (coluna) para o servidor.
	    send(s_socket, &tiro[0], sizeof(tiro[0]), 0);
	    send(s_socket, &tiro[1], sizeof(tiro[1]), 0);
        tentativas++;
		
		//variável que corresponde ao método "acertou" do servidor.
		int resp;
		//recebe do servidor o "acertou" (método que retorna 0 ou 1) e as posições dos navios.
		recv(s_socket, &resp, sizeof(resp), 0);
		recv(s_socket, &navios, sizeof(navios), 0);
		//printf("Resp: %d\n",resp);
		
		//se o método "acertou" do servidor retornou 1 então:
		if(resp==1){
			printf("VOCÊ ACERTOU O TIRO (na linha %d, coluna %d)!\n",tiro[0]+1,tiro[1]+1);
            dica(tiro,navios,tentativas);
            printf("\n");
            acertos++;
            //envia para o servidor os acertos do cliente.
			send(s_socket, &acertos,sizeof(acertos), 0);
        }
        //se o método "acertou" do servidor retornou 0 então:
        else{
			printf("VOCÊ ERROU O TIRO, TENTE NOVAMENTE!\n");
			dica(tiro,navios,tentativas);
			printf("\n");
			//envia para o servidor os acertos do cliente.
			send(s_socket, &acertos, sizeof(acertos), 0);
		}
		//chama o método que atualiza o tabuleiro de acordo com as novas informações.
		alteraTabuleiro(tiro,navios,tabuleiro,resp);
		
    }while(acertos!=3);
	
      printf("\nJOGO TERMINADO! Você acertou os 3 navios em %d tentativas.\n\n", tentativas);
      //mostra o tabuleiro final, já atualizado.
      mostraTabuleiro(tabuleiro);
      printf("\n");
	
    exit(0);
}
