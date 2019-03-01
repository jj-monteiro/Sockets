#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <winsock.h>
#include <socks.h>

#define TAMANHO_BUFFER 128
#define COMANDO_SAIDA "#sair"

int socket_remoto = 0;
int comprimento_msg = 0;

unsigned short porta_remota = 0;
char ip_remoto[32];

char mensagem[TAMANHO_BUFFER];

struct sockaddr_in endereco_remoto; // armazena endereco do servidor

// estrutura WSADATA que recebe os detalhes da implementacao do WinSock do Windows
// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/ns-winsock-wsadata
WSADATA wsa_data;

// funcao para exibir mensagens de erro e terminar o programa
void exibe_msg(char *msg){
   printf(msg);
   system("PAUSE");
   exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
   // inicia o WinSock versao 2.2
   if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
      exibe_msg("Inicializacao do WinSock falhou.\n");


   printf("TRABALHO 03 - SISTEMAS DISTRIBUIDOS - FSMA\n");
   printf("JOAO FIDALGO - 201202017\n");
   printf("SISTEMA CLIENTE-SERVIDOR PARA CONSULTA DE NOMES\n");
   printf("Modulo cliente...\n\n\n");
   
   printf("IP do servidor: ");
   scanf("%s", ip_remoto);
   fflush(stdin);

   printf("Porta do servidor: ");
   scanf("%d", &porta_remota);
   fflush(stdin); //limpa o buffer de entrada
 
   // AF_INET: IPv4
   // SOCK_STREAM: usa conexao TCP
   // IPPROTO_TCP: usa conexao TCP
   // https://docs.microsoft.com/en-us/windows/desktop/api/Winsock2/nf-winsock2-socket
   socket_remoto = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (socket_remoto == INVALID_SOCKET){
      WSACleanup();
      exibe_msg("Criacao do socket falhou.\n");
   }

   // preenchendo o endereco_remoto (servidor)
   memset(&endereco_remoto, 0, sizeof(endereco_remoto));
   
   // configura o tipo da familia do endereco ip
   endereco_remoto.sin_family = AF_INET;
   
   // configura o endereco ip endereco ip
   // htonl: converte um long de Little Endian (x86) para Big Endian (network)
   endereco_remoto.sin_addr.s_addr = inet_addr(ip_remoto);
   
   // configura a porta local utilizada pelo socket
   // htonl: converte um short de Little Endian (x86) para Big Endian (network)
   endereco_remoto.sin_port = htons(porta_remota);


   printf("Conectando ao servidor %s...\n", ip_remoto);
   //cria a conexao com o socket especificado em socket_remoto
   if (connect(socket_remoto, (struct sockaddr *) &endereco_remoto, sizeof(endereco_remoto)) == SOCKET_ERROR){
      WSACleanup();
      exibe_msg("Conexao falhou.\n");
   }

   printf("Digite o mensagem desejado:\n");
   do{
      // limpa o buffer
      memset(&mensagem, 0, TAMANHO_BUFFER);

      printf("\n-> ");
      gets(mensagem);
      fflush(stdin); //limpa o buffer de entrada
	  
	  //testa se o nome é igual o comando de saída do programa
	  //se for igual o resultado do strcmp é 0, e o programa sai do do...while
	  if(strcmp(mensagem, COMANDO_SAIDA)){
	      comprimento_msg = strlen(mensagem); //tamanho em bytes do nome
	
	      // envia o nome para o servidor
	      if (send(socket_remoto, mensagem, comprimento_msg, 0) == SOCKET_ERROR){
	         WSACleanup();
	         closesocket(socket_remoto);
	         exibe_msg("Falha ao enviar os dados.\n");
	      }
	      
	      // limpa o buffer onde ficara a idade vinda do servidor
	      memset(&mensagem, 0, TAMANHO_BUFFER);
	      
	      // espera resposta do servidor e armazena em mensagem
	      if(recv(socket_remoto, mensagem, TAMANHO_BUFFER, 0) == SOCKET_ERROR){
		  	exibe_msg("Falha no recebimento dos dados.\n");
	  	  } else {
	  	  	printf("Idade: %s\n", mensagem); // exibe a idade retornada do servidor
		  }
	  }
   }
   while(strcmp(mensagem, COMANDO_SAIDA)); // sai quando enviar um "#sair" para o servidor
  
   printf("Finalizando\n");
   WSACleanup();
   closesocket(socket_remoto);

   system("PAUSE");
   return 0;
}
