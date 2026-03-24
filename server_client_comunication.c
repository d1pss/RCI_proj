#include "server_client_comunication.h"


/********************************************************************************* -----TCP----- *********************************************************************************/
int Create_TCP_Server(Node_info* My_node){
    int errcode;
    ssize_t n;
    struct addrinfo hints,*res;

    My_node->TCP_fd[My_node->id]=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (My_node->TCP_fd[My_node->id] == UNUSED_FD){
        printf("TCP error (in Create_TCP_Server) socket\n");
        return ERR_SOCKET_IO;
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(NULL,My_node->Node_TCP_Port,&hints,&res);
    if((errcode)!=0){
        printf("TCP error (in Create_TCP_Server) getaddrinfo\n");
        return ERR_SOCKET_IO;
    }
    n=bind(My_node->TCP_fd[My_node->id],res->ai_addr,res->ai_addrlen);
    if(n==-1){
        printf("TCP error (in Create_TCP_Server) bind\n");
        return ERR_SOCKET_IO;
    }

    if(listen(My_node->TCP_fd[My_node->id], Number_of_ids) == -1){
        printf("TCP error (in Create_TCP_Server) listen\n");
        return ERR_SOCKET_IO;
    }

    freeaddrinfo(res);

    if(My_node->debug){
        printf("Created TCP server for id %02d\n", My_node->id);
    }

   return SUCCESS;
}

int accept_TCP_connection(Node_info* My_node){
    socklen_t addrlen;
    struct sockaddr_in addr;
    int newfd;

    addrlen=sizeof(addr);
    newfd = accept(My_node->TCP_fd[My_node->id],(struct sockaddr*)&addr,&addrlen);
    if(newfd == UNUSED_FD){
        printf("TCP error (in accept_TCP_connection) accept\n");
        return ERR_SOCKET_IO;
    }  

    My_node->TCP_pending_fd[My_node->number_pending_fd] = newfd;
    My_node->number_pending_fd++;

    if(My_node->debug){
        printf("accepted TCP connection in id %02d\n", My_node->id);
    }

    return SUCCESS;
}

int Create_and_Connect_TCP_client(char* dest_IP, char* dest_Port, int dest_id, Node_info* My_node){
    int return_code;
    struct addrinfo hints,*res;  

    My_node->TCP_fd[dest_id]=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (My_node->TCP_fd[dest_id] == UNUSED_FD){
        printf("TCP error (in Create_and_Connect_TCP_client) socket\n");
        return ERR_SOCKET_IO;
    }  

    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    if(getaddrinfo(dest_IP, dest_Port, &hints, &res) != 0){
        printf("TCP error (in Create_and_Connect_TCP_client) getaddrinfo\n");
        close(My_node->TCP_fd[dest_id]);
        My_node->TCP_fd[dest_id] = UNUSED_FD;
        return ERR_SOCKET_IO;
    }


    if(connect(My_node->TCP_fd[dest_id], res->ai_addr, res->ai_addrlen) == -1) {
        printf("TCP error (in Create_and_Connect_TCP_client) connect"); 
        freeaddrinfo(res);
        close(My_node->TCP_fd[dest_id]);
        My_node->TCP_fd[dest_id] = UNUSED_FD;
        return ERR_SOCKET_IO;
    }

    freeaddrinfo(res);

    if(My_node->debug){
        printf("Created and Connected TCP client in %02d to %02d\n", My_node->id, dest_id);
    }

    return_code = Send_NEIGHBOR(dest_id, My_node);
    if(return_code != SUCCESS){
        close(My_node->TCP_fd[dest_id]);
        return return_code;
    } 

    My_node->number_of_TCP_channels++;

    for(int i = 0; i < Number_of_ids; i++){
        if((My_node->dist[i] < INF && My_node->state[i] == STATE_EXPEDITION)){
            return_code = Send_ROUTE(dest_id, i, My_node);
            if(return_code != SUCCESS) return return_code;
        }
    }

    return SUCCESS;
}

int Close_TCP_Client(int dest_id, Node_info* My_node){
    close(My_node->TCP_fd[dest_id]);

    My_node->TCP_fd[dest_id] = UNUSED_FD;
    My_node->number_of_TCP_channels--;

    return SUCCESS;
}

int Close_TCP_Server(Node_info* My_node){
    close(My_node->TCP_fd[My_node->id]);

    My_node->TCP_fd[My_node->id] = UNUSED_FD;

    return SUCCESS;
}

int Send_routing_protocol_to_id(char* routing_protocol, int dest_id, Node_info* My_node){

    ssize_t n=write(My_node->TCP_fd[dest_id],routing_protocol, strlen(routing_protocol));
    if(n==-1){
        printf("TCP error (in Send_routing_protocol_to_id) write\n");
        return ERR_SOCKET_IO;
    }  
    return SUCCESS;
}

int Send_chat_protocol_to_id(char* chat_protocol, int dest_id, Node_info* My_node){
    return Send_routing_protocol_to_id(chat_protocol, dest_id, My_node);
}

int Recive_message_from_fd(char* message, int sender_fd, Node_info* My_node){
    ssize_t n=read(sender_fd, message, TCP_buffer_len);
    if(n==-1){
        printf("TCP error (in Recive_message_from_fd) read\n");
        return ERR_SOCKET_IO;
    }  

    if(n == 0){
        //TCP connection was closed
        return STATUS_SPECIFIC;
    }

    if(n < TCP_buffer_len){
        message[n] = '\0';
    }else{
        //lost info not suposed to happen if it happends we need bigger buffer
        printf("Buffer Overflow: Received data exceeded %d bytes. Information may be truncated. Increase TCP_buffer_len.\n", TCP_buffer_len);
        return ERR_UNEXPECTED;
    }

    return SUCCESS;

}


/********************************************************************************* -----UDP----- *********************************************************************************/

int send_message_to_UDP_server(char* message, char* response, Node_info* My_node){
    int fd, errcode, return_code;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr; 

    fd = socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd == UNUSED_FD){
        printf("UDP error\n");
        return ERR_SOCKET_IO;
    }  

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket

    errcode = getaddrinfo(My_node->UDP_Server_IP, My_node->UDP_Server_Port, &hints, &res);
    if(errcode != SUCCESS){
        printf("UDP error\n");
        return ERR_SOCKET_IO;
    }  

    n = sendto(fd,message,strlen(message),0,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        printf("UDP error\n");
        return ERR_SOCKET_IO;
    }  

    return_code = select_timeout(fd);
    if(return_code != SUCCESS){
        return return_code;
    } 

    addrlen=sizeof(addr);
    n=recvfrom(fd,response,UDP_response_size,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1){
        printf("UDP error\n");
        return ERR_SOCKET_IO;
    }  

    if(n < UDP_response_size){
        response[n] = '\0';
    }else{
        //lost info not suposed to happen if it happends we need bigger buffer
        printf("Buffer Overflow: Received data exceeded %d bytes. Information may be truncated. Increase UDP_response_size.\n", UDP_response_size);
        return ERR_UNEXPECTED;
    }

    freeaddrinfo(res);
    close(fd);

    return SUCCESS;
}

int select_timeout(int fd){
    fd_set fdset;
    struct timeval tv;

    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);

    //Define timeout as UDP_TIMEOUT_SECONDS seconds
    tv.tv_sec = UDP_TIMEOUT_SECONDS; 
    tv.tv_usec = 0;

    int sret = select(fd + 1, &fdset, NULL, NULL, &tv);

    if (sret == 0) {
        // TIMEOUT
        printf("ERROR: The response took to long to arive or the server is not responding\n");
        close(fd);
        return SUCCESS; 
    } else if (sret == -1) {
        // ERROR in select
        printf("ERROR (UDP select)\n"); 
        close(fd);
        return ERR_UNEXPECTED;
    }

    return SUCCESS;
}