#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#define PORT 8080
#define MAX 100

void findInfile(char buffer[],char *output, int isDomain){
	FILE *fp = fopen("domaininfo.txt","r");
	int i;
	i=0;
	char domain[100],ip[100];

	while(fgets(domain,100,fp)!=NULL){
		int index = 0;
		while(index<strlen(domain) && domain[index]!=' ')index++;
		int i=0;
		for(i = index+1;domain[i]!='\n';i++){
			ip[i-index-1] = domain[i];
		}
		domain[index] = '\0';
		ip[i] = '\0';		
		if(isDomain == 1 && strcmp(domain,buffer)==0){
			strcpy(output,ip);
			fclose(fp);
			return;

		}
		if(isDomain == 0 && strcmp(ip,buffer)==0){
		
			strcpy(output,domain);
			fclose(fp);
			return;
		}
	}
	
	if(isDomain==0) strcpy(output,"Requested IP address not found");
	else strcpy(output,"Requested domain name not found");
        fclose(fp);
}

void dns(int server_socket){
	char buffer[MAX];
	struct sockaddr_in client_address;
	int i ;

	char domain[MAX], ip[MAX];
	
	while(1){
		
		char output[MAX];
		int isDomain  = 1;
		
		bzero(buffer,MAX);
		bzero(&client_address,sizeof(client_address));
		
		int len = sizeof(client_address);
		
		recvfrom(server_socket,buffer,MAX,0,(struct sockaddr*)&client_address,&len);

		buffer[strlen(buffer)-1] = '\0';
		printf("\nRequest from Client : %s\n",buffer);
		
		if(strncmp(buffer,"exit",4)==0){
			printf("\nServer has successfully exited the network\n");
			return;
		
		}
		if(buffer[0]>='0' && buffer[0]<='9')isDomain = 0;
		
		
		findInfile(buffer,(char*)output,isDomain);
		
		sendto(server_socket,output,strlen(output),0,(struct sockaddr*)&client_address,len);
		
	}
}

int main(){

	//Initialize socket descriptor
	int server_socket;
	
	//Create a UDP socket
	server_socket = socket(PF_INET,SOCK_DGRAM,0);
	
	if(server_socket<0){
		printf("\nError creating socket ...");
		exit(1);
	}
	else{
		printf("\nSocket created successfully ...\n");
	}
	
	

	//Initializing sockddr_in structure before binding
	struct sockaddr_in server_address;
	bzero(&server_address,sizeof(server_address));
	
	server_address.sin_family = PF_INET;           //Protocol family for IPV4 communications
	server_address.sin_port = htons(PORT);         //Port number 8080 converted from host to network byte ordering
	server_address.sin_addr.s_addr = INADDR_ANY;
	
	//Binding socket to port
	if(bind(server_socket,(struct sockaddr*)&server_address, sizeof(server_address))== 0){
		printf("\nSocket binded successfully ...\n");

	} 
	else{
		printf("\nUnable to bind server... An error occured\n");
		exit(1);		
	}
	
	//Calling function that converts domain name to ip and vice versa
	
	dns(server_socket);
	
	//Close the server socket
	close(server_socket);
}
