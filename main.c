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
#define Max_message_len 40 //in the worse case using REG we can have 38 chars so 40 is a safe number

typedef struct _comand_variables{
    bool is_node_in_net;
    bool is_monitoring;
}Cmd_V;

typedef struct _UDP_Server
{
    char IP[16];
    char Port[6];
}UDP_S;


int send_message_to_UDP_server(char* message, char** response, int* response_len, UDP_S* Server);

bool is_id_in_net(char* net, char* id, UDP_S* Server);

int main(void){


    while(true){
        
      



    }

    return 0;
}


//return 0 return to main without error
//return 1 return to main to exit the program without errors
//return 2 return to main to exit the program with network error
//return 3 return to main to exit the program with incorrect input
//return 4 return to main to exit the program with memory allocation error
//return 5 return to main to exit the program with connection via UDP or TCP error
//return 6 return to main to exit the program with not suposed to happend case
int process_command(char *input, Cmd_V* cmd_variables, UDP_S* Server){
    int num_args, return_code;
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

            //return to main to exit the program without errors
            return 1;

        }else if(!strcmp(cmd, "sg")){

        }else if(!strcmp(cmd, "a")){

        }else if(!strcmp(cmd, "sm")){

        }else if(!strcmp(cmd, "em")){

        }else{
            //error bad input
        }

    }else if(num_args == 2){

        if(!strcmp(cmd, "n")){

            if(atoi(arguments[0]) > 999 || atoi(arguments[0]) < 0){
                //error net value should be between 999 and 000
                printf("ERROR: input argument net should be between 000 and 999\n");
            }

            return_code = cmd_show_nodes(arguments[0], Server);

            if(return_code == 1){
                //return to main to exit the program with network error
                return 2;
            }

            //return to main without error
            return 0;

        }else if(!strcmp(cmd, "ae")){

        }else if(!strcmp(cmd, "re")){

        }else if(!strcmp(cmd, "sr")){

        }else{
            //error bad input
        }

    }else if(num_args == 3){

        if(!strcmp(cmd, "j")){

            if(atoi(arguments[0]) > 999 || atoi(arguments[0]) < 0 || atoi(arguments[1]) > 99 || atoi(arguments[1]) < 0){
                //error net value should be between 999 and 000 end id between 99 and 00
            }

            if(is_id_in_net(arguments[0], arguments[1], Server)){
                //error id already exists must use a difrent one
            }

            cmd_variables->is_node_in_net = true;

            cmd_join(arguments[1], arguments[2], Server);

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

int cmd_show_nodes(char* net, UDP_S* Server){
    char message[Max_message_len], *response, op;
    int response_len, return_code;

    sprintf(message, "NODES 100 0 %s\n", net);

    return_code = send_message_to_UDP_server(message, &response, &response_len, Server);

    if(return_code != 0){
        return return_code;
    }

    int items_found = sscanf(response, "%*s %*s %c", &op);

    if (items_found == 1) {
        if (op == '1') {
            // message contains the ids in the network
            return_code = print_ids(response, response_len, net);

            free(response);
            return return_code;
        }else{
            //error code from network
            printf("ERROR: error code (%c) from network using this comand %s\n", op, message);

            free(response);
            return 2;
        }
    }else{
        printf("DEBUG ERROR: (in function cmd_show_nodes) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        return 6;
    } 
}

int print_ids(char* response, int response_len, char* net){

    int first_id_index = 0;
    char id_to_print[3] = {'0', '0', '\0'};

    for(int i = 0; i < response_len; i++){
        if(response[i] == '\n'){
            first_id_index = i + 1;
        }
    }
    if(first_id_index != 0){
        if(first_id_index != response_len){
            printf("List of Nodes in network %s:\n", net);
            for(int i = first_id_index; i < response_len; i+3){
                id_to_print[0] = response[i];
                id_to_print[0] = response[i+1];
                printf("%s\n", id_to_print);
            }
        }else{
            // there is no nodes in the network
            printf("There are no Nodes in the network %s to show.\n", net);
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function print_ids) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        return 6;
    }
    
    return 0;
}

int cmd_leave(){
    //retira todas as arestas ligadas ao no e remove o no da rede
}

int cmd_join(char* net, char* id, UDP_S* Server){
    char message[Max_message_len], *response, op;
    int response_len, return_code;

    char* new_IP, *new_Port;

    //get_new_ip_and_Port(&new_IP, &new_Port);
    
    sprintf(message, "REG 100 0 %s %s %s %s\n", net, id, new_IP, new_Port);

    return_code = send_message_to_UDP_server(message, &response, &response_len, Server);

    if(return_code != 0){
        return return_code;
    }

    int items_found = sscanf(response, "%*s %*s %c", &op);

    if (items_found == 1) {
        if (op == '1') {
            // id sucessfuly registred in network
            printf("id was sucessfuly registred in network\n");

            free(response);
            return 0;
        } else if (op == '2') {
            // Network is full
            free(response);
            return 0;
        }else{
            //error code from network
            printf("ERROR: error code (%c) from network using this comand %s\n", op, message);

            free(response);
            return 2;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function cmd_join) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        return 6;
    }
}


bool is_id_in_net(char* net, char* id, UDP_S* Server){
    char message[Max_message_len];
    char* response, op;
    int response_len;

    sprintf(message, "CONTACT 100 0 %s %s\n", net, id);
   

    send_message_to_UDP_server(message, &response, &response_len, Server);
 
    int items_found = sscanf(response, "%*s %*s %c", &op);

    if (items_found == 1) {
        if (op == '1') {
            // ID already exists
            free(response);
            return true;
        } else if (op == '2') {
            // ID does not exist
            free(response);
            return false;
        }else{
            //error code from network
        }
    }

    free(response);

}


//response needs to be freed outside the function (if it has error no need to free response)
int send_message_to_UDP_server(char* message, char** response, int* response_len, UDP_S* Server){
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr; 
    char buffer[Max_buff_size];

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/return 5;

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket

    errcode=getaddrinfo(Server->IP,Server->Port,&hints,&res);
    if(errcode!=0) /*error*/ return 5;

    n=sendto(fd,message,strlen(message),0,res->ai_addr,res->ai_addrlen);
    if(n==-1) /*error*/ return 5;

    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,Max_buff_size,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1) /*error*/ return 5;

    if(n < Max_buff_size){
        buffer[n] = '\0';
    }else{
        //lost info not suposed to happen if it happends we need bigger buffer
        printf("SE ISTO IMPRIMIO PERCISAMOS DE UM BUFFER MAIOR\n"); //debug retirar no fim
    }

    *response = (char*)malloc((n+1)*sizeof(char));
    if(*response == NULL){
        //error alocating memory
        return 4;
    }
    *response_len = n;
    strcpy(*response, buffer);

    freeaddrinfo(res);
    close(fd);

    return 0;
}