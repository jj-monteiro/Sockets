#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> //necessário para executar no Windows


#define CONEXOES_PENDENTES 5
#define TAMANHO_BUFFER 128
#define COMANDO_SAIDA "#sair"


int i;

int socket_local = 0;
int socket_remoto = 0;

int comprimento_msg = 0;
int tam_end_remoto = 0;

unsigned short porta_local = 11111;

struct sockaddr_in endereco_local;  // armazena endereco do servidor (maquina local)
struct sockaddr_in endereco_remoto; // armazena endereco do cliente

char nome[TAMANHO_BUFFER];  //armazena o nome que vem do cliente
char sem_registro[]= "Nao achei."; //mensagem que sera enviada para o cliente caso o nome nao seja encontrado

//estrutura de armazenamento de dados
struct pessoa{
	char nome[TAMANHO_BUFFER];
	char idade[4];
};

//preenchimento da lista de pessoas (banco de dados)
struct pessoa lista[10] = {"ana","20","fernanda","21","pedro","22","paulo","23","lucas","24","marcos","25","mateus","26",
						   "maria","27","eva","28","laura","29"};

 
// funcao para exibir mensagens de erro e terminar o programa
void exibe_msg(char *msg) {
   printf(msg);
   system("PAUSE");
   exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
   
   // estrutura WSADATA que recebe os detalhes da implementacao do WinSock do Windows
   // https://docs.microsoft.com/en-us/windows/desktop/api/winsock/ns-winsock-wsadata
   WSADATA wsa_data;
   
   // inicia o WinSock versao 2.2
   if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
      exibe_msg("Inicializacao do WinSock falhou.\n");
   
   
   // cria o socket local para o servidor 
   // AF_INET: IPv4
   // SOCK_STREAM: usa conexao TCP
   // IPPROTO_TCP: usa conexao TCP
   // https://docs.microsoft.com/en-us/windows/desktop/api/Winsock2/nf-winsock2-socket
   socket_local = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //retorna o socket descriptor
   if (socket_local == INVALID_SOCKET){
      WSACleanup(); //finaliza o a utilizacao da biblioteca WinSock
      exibe_msg("Criacao do socket falhou.\n");
   }
   
   // zera a estrutura endereco_local
   memset(&endereco_local, 0, sizeof(endereco_local));

   // configura o tipo da familia do endereco ip
   endereco_local.sin_family = AF_INET;

   // configura a porta local utilizada pelo socket
   // htonl: converte um short de Little Endian (x86) para Big Endian (network)
   endereco_local.sin_port = htons(porta_local);

   // configura o endereco ip endereco ip
   // htonl: converte um long de Little Endian (x86) para Big Endian (network)
   // INADDR_ANY: o socket usa todos os IP's do servidor para responder 
   endereco_local.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr("127.0.0.1")
   
   // vincula o socket com o endere�o local
   if (bind(socket_local, (struct sockaddr *) &endereco_local, sizeof(endereco_local)) == SOCKET_ERROR){
      WSACleanup();
      closesocket(socket_local); // fecha o socket
      exibe_msg("Vinculacao do socket com endereco falhou (bind).\n");
   }
  
   // coloca o socket para escutar as conexoes dos clientes
   // CONEXOES_PENDENTES = quantidade de conexoes que o listen pode colocar na fila para serem processadas
   if (listen(socket_local, CONEXOES_PENDENTES) == SOCKET_ERROR){
      WSACleanup();
      closesocket(socket_local);
      exibe_msg("Problema na ""escuta"".\n");
   }
   
   // tam_end_remoto recebe o tamanho da estrutura endereco_remoto do tipo sockaddr_in
   tam_end_remoto = sizeof(endereco_remoto);
   
   printf("TRABALHO 03 - SISTEMAS DISTRIBUIDOS - FSMA\n");
   printf("JOAO FIDALGO - 201202017\n");
   printf("SISTEMA CLIENTE-SERVIDOR PARA CONSULTA DE NOMES\n");
   printf("Modulo servidor...\n\n");
   printf("Aguardando conexao do cliente...\n");
   
   // aceita a conexao quando detectada
   // retorna o socket descriptor do cliente
   socket_remoto = accept(socket_local, (struct sockaddr *) &endereco_remoto, &tam_end_remoto);
   if(socket_remoto == INVALID_SOCKET){
      WSACleanup();
      closesocket(socket_local);
      exibe_msg("Falha no aceite da conexao.\n");
   }
   
   printf("Conexao estabelecida com %s\n", inet_ntoa(endereco_remoto.sin_addr));
   printf("Aguardando consulta...\n");
   do{
      // limpa o buffer
      memset(&nome, 0, TAMANHO_BUFFER);
 
      // recebe a mensagem do cliente e armazena em nome
      comprimento_msg = recv(socket_remoto, nome, TAMANHO_BUFFER, 0);
      if(comprimento_msg == SOCKET_ERROR)
         exibe_msg("Falha na recebimento dos dados.\n");
      
      //testa se o nome � igual o comando de sa�da do programa
	  //se for igual o resultado do strcmp � 0, e o programa sai do do...while
      if(strcmp(nome, COMANDO_SAIDA)){
	  	// exibe a mensagem na tela
      	printf("%s consultou: %s\n", inet_ntoa(endereco_remoto.sin_addr), nome);
      
      	//testa se o nome recebido est� contido nos vetor de estrutura
	   	for (i=0; i<10; i++){
	   		if(strcmp(nome,lista[i].nome) == 0){
      			printf("Nome encontrado.\n");
      			comprimento_msg = strlen(lista[i].idade); //comprimento do stream de dados [idade] que o servidor enviara
      			
      			// envia idade para o cliente
				if (send(socket_remoto, lista[i].idade, comprimento_msg, 0) == SOCKET_ERROR){
       				WSACleanup();
        			closesocket(socket_remoto);
        			exibe_msg("Falha ao enviar os dados.\n");
       			}
       			goto fim; //sai do loop for caso o nome seja encontrado
			}
			//se nao achou o nome na ultima posicao do vetor
			if(i==9){
				printf("Nome nao encontrado.\n");
				comprimento_msg = strlen(sem_registro); //comprimento da stream de dados da string "Nao achei."
				
				//envia a mensagem "Nao achei." para o cliente
				if (send(socket_remoto, sem_registro, comprimento_msg, 0) == SOCKET_ERROR){
       				WSACleanup();
        			closesocket(socket_remoto);
        			exibe_msg("Falha ao enviar os dados.\n");
       			}
			}
	  	}
	  }
	  		
	  fim: //ponto de saida do loop for caso o nome tenha sido encontrado
	  printf("Aguardando nova consulta...\n");
	    
   }
   while(strcmp(nome, COMANDO_SAIDA)); // sai quando receber um "#sair" do cliente
   
   printf("Finalizando...\n");
   WSACleanup();
   closesocket(socket_local);
   closesocket(socket_remoto);
  
   system("PAUSE");	
   return 0;
}
