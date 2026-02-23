#define _POSIX_C_SOURCE 200112L
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <string.h>

#define DEFAULT_UDP_PORT "59000"
#define DEFAULT_UDP_IP "193.136.138.142"

#define Max_buff_size 320 //100 id x 3 for the 2 digits and the \n + 20 for the first line in the worse case

typedef struct _comand_variables{
    bool is_node_in_net;
    bool is_monitoring;
}Cmd_V;

int main(void){

    while(true){
        
        




    }

}



bool process_command(char *input, Cmd_V* cmd_variables){
    int num_args;
    char cmd[4];
    int input_len = (int)strlen(input);
    char* arguments[3];

    for(int i = 0; i < 3; i++){
        arguments[i] = (char*)malloc(input_len * sizeof(char));
        if(arguments[i] == NULL){
            //failed to alocate mem to heap
            exit(1);
        }

    }
    
    num_args = sscanf(input, "%s %s %s %s", cmd, arguments[0], arguments[1], arguments[2]);

    if(num_args == 1){
        if(!strcmp(cmd, "l")){
            if(cmd_variables->is_node_in_net){
                cmd_leave();
                cmd_variables->is_node_in_net = false;
            }else{
                //the node is not in the network, so no need to leave
            }
            

        }else if(!strcmp(cmd, "x")){

            if(cmd_variables->is_node_in_net){
                //in case we forget to leave before exiting
                cmd_leave();
            }

            //true means exit the program
            return true;

        }else if(!strcmp(cmd, "sg")){

        }else if(!strcmp(cmd, "a")){

        }else if(!strcmp(cmd, "sm")){

        }else if(!strcmp(cmd, "em")){

        }else{
            //error bad input
        }

    }else if(num_args == 2){

        if(!strcmp(cmd, "n")){

        }else if(!strcmp(cmd, "ae")){

        }else if(!strcmp(cmd, "re")){

        }else if(!strcmp(cmd, "sr")){

        }else{
            //error bad input
        }

    }else if(num_args == 3){

        if(!strcmp(cmd, "j")){

            if(atoi(arguments[1]) > 999 || atoi(arguments[1]) < 0 || atoi(arguments[2]) > 99 || atoi(arguments[2]) < 0){
                //error net value should be between 999 and 000 end id between 99 and 00
            }

            

            cmd_variables->is_node_in_net = true;
            join(arguments[1], arguments[2]);

        }else if(!strcmp(cmd, "m")){

        }else if(!strcmp(cmd, "dj")){

        }else{
            //error bad input
        }
        
    }else if(num_args == 4){

        if(!strcmp(cmd, "dae")){

        }else{
            //error bad input
        }

    }else{
        //error not suposed to be here (sscanf error)
    }

    // false to not leave the program
    return false;
    

}

void cmd_leave(){
    //retira todas as arestas ligadas ao no e remove o no da rede
}

void join(char* net, char* id){

    

}

bool is_id_in_net(char* net, char* id){

}

void send_message_to_UDP_server(char* message, char** response, int* sizeof_response, char* UDP_IP, char* UDP_Port){
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr; 
    char buffer[Max_buff_size];

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket

    errcode=getaddrinfo(UDP_IP,UDP_Port,&hints,&res);
    if(errcode!=0) /*error*/ exit(1);

    n=sendto(fd,message,strlen(message),0,res->ai_addr,res->ai_addrlen);
    if(n==-1) /*error*/ exit(1);

    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,Max_buff_size,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1) /*error*/ exit(1);


    freeaddrinfo(res);
    close(fd);
}