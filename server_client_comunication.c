#include "server_client_comunication.h"


/********************************************************************************* -----TCP----- *********************************************************************************/
int Create_TCP_Server(Node_info* My_node){
    int errcode;
    ssize_t n;
    struct addrinfo hints,*res;

    My_node->TCP_fd[My_node->id]=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (My_node->TCP_fd[My_node->id]==-1) return 5; //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(NULL,My_node->Node_TCP_Port,&hints,&res);
    if((errcode)!=0)/*error*/return 5;

    n=bind(My_node->TCP_fd[My_node->id],res->ai_addr,res->ai_addrlen);
    if(n==-1) /*error*/ return 5;

    if(listen(My_node->TCP_fd[My_node->id], Number_of_ids)==-1)/*error*/return 5;

    freeaddrinfo(res);

   return 0;
}

int accept_TCP_connection(Node_info* My_node){
    ssize_t n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    int newfd, cli_id = 0, return_code;
    char Routing_protocol[TCP_Routing_protocol_len];

    addrlen=sizeof(addr);
    if((newfd=accept(My_node->TCP_fd[My_node->id],(struct sockaddr*)&addr,&addrlen))==-1)
    /*error*/ return 5;

    My_node->TCP_pending_fd[My_node->number_pending_fd] = newfd;
    My_node->number_pending_fd++;

    return 0;
}

int Create_and_Connect_TCP_client(char* dest_IP, char* dest_Port, int dest_id, Node_info* My_node){
    int errcode, return_code;
    ssize_t n;
    struct addrinfo hints,*res;
    char Routing_protocol[TCP_Routing_protocol_len];

    My_node->TCP_fd[dest_id]=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (My_node->TCP_fd[dest_id]==-1) return 5; //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode=getaddrinfo(dest_IP,dest_Port,&hints,&res);
    if(errcode!=0)/*error*/return 5;

    n=connect(My_node->TCP_fd[dest_id],res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/return 5;

    freeaddrinfo(res);

    return_code = Send_NEIGHBOR(dest_id, My_node);
    if(return_code != 0){
        close(My_node->TCP_fd[dest_id]);
        return return_code;
    } 

    My_node->number_of_TCP_channels++;

    return_code = Send_ROUTE(dest_id, My_node->id, My_node);
    if(return_code != 0) return return_code;

    return 0;
}

int Close_TCP_Client(int dest_id, Node_info* My_node){
    close(My_node->TCP_fd[dest_id]);

    My_node->TCP_fd[dest_id] = -1;
    My_node->number_of_TCP_channels--;

    return 0;
}

int Close_TCP_Server(Node_info* My_node){
    close(My_node->TCP_fd[My_node->id]);

    My_node->TCP_fd[My_node->id] = -1;

    return 0;
}

int Send_routing_protocol_to_id(char* routing_protocol, int dest_id, Node_info* My_node){

    ssize_t n=write(My_node->TCP_fd[dest_id],routing_protocol, strlen(routing_protocol));
    if(n==-1)/*error*/return 5;

    return 0;
}

int Send_chat_protocol_to_id(char* chat_protocol, int dest_id, Node_info* My_node){
    return Send_routing_protocol_to_id(chat_protocol, dest_id, My_node);
}

int Recive_message_from_fd(char* message, int sender_fd, Node_info* My_node){
    ssize_t n=read(sender_fd, message, TCP_Chat_protocol_len);
    if(n==-1)/*error*/return 5;

    if(n < TCP_Chat_protocol_len){
        message[n] = '\0';
    }else{
        //lost info not suposed to happen if it happends we need bigger buffer
        printf("SE ISTO IMPRIMIO PERCISAMOS DE UM BUFFER MAIOR\n"); //debug retirar no fim
    }

    return 0;

}


/********************************************************************************* -----UDP----- *********************************************************************************/

int send_message_to_UDP_server(char* message, char* response, Node_info* My_node){
    int fd, errcode, return_code;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr; 

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/return 5;

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket

    errcode=getaddrinfo(My_node->UDP_Server_IP, My_node->UDP_Server_Port, &hints, &res);
    if(errcode!=0) /*error*/ return 5;

    n=sendto(fd,message,strlen(message),0,res->ai_addr,res->ai_addrlen);
    if(n==-1) /*error*/ return 5;

    return_code = select_timeout(fd);
    if(return_code != 0){
        if(return_code == 7){
            return 0;
        }
        return return_code;
    } 

    addrlen=sizeof(addr);
    n=recvfrom(fd,response,UDP_response_size,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1) /*error*/ return 5;

    if(n < UDP_response_size){
        response[n] = '\0';
    }else{
        //lost info not suposed to happen if it happends we need bigger buffer
        printf("SE ISTO IMPRIMIO PERCISAMOS DE UM BUFFER MAIOR\n"); //debug retirar no fim
    }

    freeaddrinfo(res);
    close(fd);

    return 0;
}

// return 7 if timeout
int select_timeout(int fd){
    fd_set fdset;
    struct timeval tv;

    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);

    //Define timeout
    tv.tv_sec = 5; 
    tv.tv_usec = 0;

    int sret = select(fd + 1, &fdset, NULL, NULL, &tv);

    if (sret == 0) {
        // TIMEOUT
        printf("Erro: The response took to long to arive or got lost\n");
        close(fd);
        return 7; 
    } else if (sret == -1) {
        // ERRO in select
        printf("ERROR: select error\n"); 
        close(fd);
        return 5;
    }

    return 0;
}