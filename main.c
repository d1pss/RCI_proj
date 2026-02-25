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
#include <sys/select.h>

#define DEFAULT_UDP_PORT "59000"
#define DEFAULT_UDP_IP "193.136.138.142"

#define Max_buff_size 320 //100 id x 3 for the 2 digits and the \n + 20 for the first line in the worse case
#define Max_message_len 40 //in the worse case using REG we can have 38 chars so 40 is a safe lenght
#define Max_cmd_len 32 //in the worse case using (dae id idIP idTCP) we can have 30 chars so 32 is a safe lenght
#define Max_cmd_arguments 3 //in the worse case using (dae id idIP idTCP) we have 3 arguments
#define Max_IP_len 16 // in the worse case we have xxx.xxx.xxx.xxx\0 that is 16 chars
#define Max_Port_len 6 // in the worse case we have xxxxx\0 that is 6 chars
#define Number_of_ids 100 // we have ids between 00 and 99, so 100 in total

typedef struct _Node_information{
    //TCP info for node to node connection
    char Node_TCP_IP[Max_IP_len];
    char Node_TCP_Port[Max_Port_len];

    //UDP info for node to network connection
    char UDP_Server_IP[Max_IP_len];
    char UDP_Server_Port[Max_Port_len];

    //flags
    bool is_node_in_net;
    bool is_monitoring;
}Node_info;

int main(int argc, char *argv[]){
    

    if(argc == 5){
        if(is_IP_invalid(argv[1])){
            printf("IP used in ./OWR --> IP <-- TCP regIP regUDP does not folow the standard IPv4 structure\n");
            return 0;
        }
        if(is_Port_invalid(argv[2])){
            printf("TCP Port used in ./OWR IP --> TCP <-- regIP regUDP does not folow the standard Port structure\n");
            return 0;
        }
        if(is_IP_invalid(argv[3])){
            printf("IP used in ./OWR IP TCP --> regIP <-- regUDP does not folow the standard IPv4 structure\n");
            return 0;
        }
        if(is_Port_invalid(argv[4])){
            printf("UDP Port used in ./OWR IP TCP regIP --> regUDP <-- does not folow the standard Port structure\n");
            return 0;
        }
    }else if(argc == 3){
        if(is_IP_invalid(argv[1])){
            printf("IP used in ./OWR --> IP <-- TCP does not folow the standard IPv4 structure\n");
            return 0;
        }
        if(is_Port_invalid(argv[2])){
            printf("TCP Port used in ./OWR IP --> TCP <-- does not folow the standard Port structure\n");
            return 0;
        }
    }else{
        printf("Comand format is incorrect.\nUsage: ./OWR IP TCP regIP regUDP\n");
        return 0;
    }
    
    Node_info* My_node = init_Node(argv, argc);

    



    int nfds;
    fd_set fdset;

    while(true){
        FD_ZERO(&fdset);
        FD_SET(STDIN_FILENO, &fdset);
        

        switch (select(nfds, &fdset, NULL, NULL, NULL))
        {
        case 0:
            //timeout
            break;

        case -1:
            //error
            break;
        
        default:
            if(FD_ISSET(STDIN_FILENO, &fdset)){


            } 
            
            break;
        }



    }

    return 0;
}

bool is_IP_invalid(char* IP) {
    int a, b, c, d;
    int last_index = 0;

    // %n stores the index of the last element into
    if (sscanf(IP, "%d.%d.%d.%d%n", &a, &b, &c, &d, &last_index) != 4) {
        return true; 
    }

    // Se o número de caracteres lidos pelo sscanf for diferente do 
    // tamanho total da string, significa que sobrou "lixo" no final.
    if (IP[last_index] != '\0') {
        return true;
    }

    if (a < 0 || a > 255 || b < 0 || b > 255 || 
        c < 0 || c > 255 || d < 0 || d > 255) {
        return true;
    }

    return false;
}

bool is_Port_invalid(char* Port) {
    char* endptr;
    // Converte string para long, endptr aponta onde a conversão parou
    long p = strtol(Port, &endptr, 10);

    // Se endptr ainda aponta para Port, não era um número.
    // Se *endptr não for \0, havia lixo (ex: "8080abc")
    if (Port == endptr || *endptr != '\0') return true;

    if (p < 1 || p > 65535) return true;

    return false;
}

Node_info* init_Node(char** argv, int argc){
    Node_info* My_node = (Node_info*)malloc(sizeof(Node_info));
    if(My_node == NULL){
        printf("ERROR: error alocating memory");
        return 1;
    }

    strcpy(My_node->Node_TCP_IP, argv[1]);
    strcpy(My_node->Node_TCP_Port, argv[2]);

    if(argc == 5){
        strcpy(My_node->UDP_Server_IP, argv[3]);
        strcpy(My_node->UDP_Server_Port, argv[4]);
    }else{
        strcpy(My_node->UDP_Server_IP, DEFAULT_UDP_IP);
        strcpy(My_node->UDP_Server_Port, DEFAULT_UDP_PORT);
    }

    My_node->is_node_in_net = false;
    My_node->is_monitoring = false;

    return My_node;
}



//return 0 return to main without error
//return 1 return to main to exit the program without errors
//return 2 return to main to with network error
//return 3 return to main to with incorrect input
//return 4 return to main to exit the program with memory allocation error
//return 5 return to main to exit the program with connection via UDP or TCP error
//return 6 return to main to exit the program with not suposed to happend case
int process_command(char *input, Node_info* My_node){
    char arguments[Max_cmd_arguments][Max_cmd_len], cmd[Max_cmd_len];
    int num_args, return_code;
    
    bool does_id_exist;
    
    num_args = sscanf(input, "%s %s %s %s", &cmd, &arguments[0], &arguments[1], &arguments[2]);

    if(num_args == 1){
        if(!strcmp(cmd, "l")){
            if(My_node->is_node_in_net){
                //the node is in the network, so leave
                //cmd_leave();

                My_node->is_node_in_net = false;

                return 0;
            }else{
                //the node is not in the network, so no need to leave
                printf("The node is not in the network, so no need to leave\n");

                return 0;
            }
            

        }else if(!strcmp(cmd, "x")){

            //in case we forget to leave before exiting
            if(My_node->is_node_in_net){
                //the node is in the network, so leave
                //cmd_leave();
            }

            //return to main to exit the program without errors
            return 1;

        }else if(!strcmp(cmd, "sg")){

        }else if(!strcmp(cmd, "a")){

        }else if(!strcmp(cmd, "sm")){

        }else if(!strcmp(cmd, "em")){

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input\n", cmd);
            return 3;
        }

    }else if(num_args == 2){

        if(!strcmp(cmd, "n")){

            if(atoi(arguments[0]) > 999 || atoi(arguments[0]) < 0){
                //error net value should be between 999 and 000
                printf("ERROR: input argument net should be between 000 and 999\n");
                return 3;
            }

            return_code = cmd_show_nodes(arguments[0], My_node);

            return return_code;

        }else if(!strcmp(cmd, "ae")){

        }else if(!strcmp(cmd, "re")){

        }else if(!strcmp(cmd, "sr")){

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input\n", cmd);
            return 3;
        }

    }else if(num_args == 3){

        if(!strcmp(cmd, "j")){

            if(atoi(arguments[0]) > 999 || atoi(arguments[0]) < 0){
                //error net value should be between 999 and 000
                printf("ERROR: input argument net should be between 000 and 999\n");
                return 3;
            }

            if(atoi(arguments[1]) > 99 || atoi(arguments[1]) < 0){
                //error id value should be between 99 and 00
                printf("ERROR: input argument id should be between 00 and 99\n");
                return 3;
            }

            if(My_node->is_node_in_net){
                //no need to join node is already in th network


            }

            return_code = is_id_in_net(&does_id_exist ,arguments[0], arguments[1], My_node);

            if(return_code != 0){
                return return_code;
            }

            if(does_id_exist){
                //error id already exists must use a difrent one
                printf("The id chosen (%s) is in use. Must use a difrent id to add the node to the network %s\n", arguments[1], arguments[0]);
                return 0;
            }

            My_node->is_node_in_net = true;

            return_code = cmd_join(arguments[0], arguments[1], My_node);

            return return_code;

        }else if(!strcmp(cmd, "m")){

        }else if(!strcmp(cmd, "dj")){

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input\n", cmd);
            return 3;
        }
        
    }else if(num_args == 4){

        if(!strcmp(cmd, "dae")){

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input\n", cmd);
            return 3;
        }

    }else{
        //unexpected sscanf return
        printf("DEBUG ERROR: (in function process_command) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
        return 6;
    }
}

int cmd_show_nodes(char* net, Node_info* My_node){
    char message[Max_message_len], *response, op;
    int response_len, return_code;

    sprintf(message, "NODES 100 0 %s\n", net);

    return_code = send_message_to_UDP_server(message, &response, &response_len, My_node);

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
            return 0;
        }else{
            // there is no nodes in the network
            printf("There are no Nodes in the network %s to show.\n", net);
            return 0;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function print_ids) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        free(response);
        return 6;
    }
}

void cmd_leave(char* net, char* id, UDP_S* Server){
    char message[Max_message_len], *response, op;
    int sizeof_response, n_neighbours;
    char* new_IP, *new_Port;
    get_new_ip_and_Port(&new_IP, &new_Port);
    //está associado a OWR IP TCP regIP regUDP

    get_neighbours();

    for(int i = 0; i < n_neighbours; i++){
        cmd_remove_edge(char* net, char* id, UDP_S* Server);
    }

   

    sprintf(message, "REG 100 3 %s %s %s %s\n", net, id, new_IP, new_Port);
    send_message_to_UDP_server(message, &response, &sizeof_response, Server);
    int items_found = sscanf(response, "%*s %*s %c", &op);

    
    if (items_found == 1) {
        if (op == '4') {
            // id succesfully left
            free(response);
            return;
        
        }else{
            //error code from network
        }
    }

    free(response);
    return;

}

int cmd_join(char* net, char* id, Node_info* My_node){
    char message[Max_message_len], *response, op;
    int response_len, return_code;

    //get_new_ip_and_Port(&new_IP, &new_Port);
    //está associado a OWR IP TCP regIP regUDP
    
    sprintf(message, "REG 100 0 %s %s %s %s\n", net, id, My_node->Node_TCP_IP, My_node->Node_TCP_Port);

    return_code = send_message_to_UDP_server(message, &response, &response_len, My_node);

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
            printf("id was not registred in the network because its full\n");

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
        free(response);
        return 6;
    }
}


void cmd_remove_edge(char* net, char* id, UDP_S* Server){

}

int get_neighbours(){

    return;
}

int is_id_in_net(bool* is_id_in_net ,char* net, char* id, Node_info* My_node){
    char message[Max_message_len];
    char* response, op;
    int response_len, return_code;

    sprintf(message, "CONTACT 100 0 %s %s\n", net, id);
   

    return_code = send_message_to_UDP_server(message, &response, &response_len, My_node);

    if(return_code != 0){
        return return_code;
    }
 
    int items_found = sscanf(response, "%*s %*s %c", &op);

    if (items_found == 1) {
        if (op == '1') {
            // ID already exists
            *is_id_in_net = true;

            free(response);
            return 0;
        } else if (op == '2') {
            // ID does not exist
            *is_id_in_net = false;

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
        free(response);
        return 6;
    }
}


//response needs to be freed outside the function (if it has error no need to free response)
int send_message_to_UDP_server(char* message, char** response, int* response_len, Node_info* My_node){
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

    errcode=getaddrinfo(My_node->UDP_Server_IP, My_node->Node_TCP_Port, &hints, &res);
    if(errcode!=0) /*error*/ return 5;

    n=sendto(fd,message,strlen(message),0,res->ai_addr,res->ai_addrlen);
    if(n==-1) /*error*/ return 5;


    //do select here


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
        printf("ERROR: error alocating memory");
        return 4;
    }
    *response_len = n;
    strcpy(*response, buffer);

    freeaddrinfo(res);
    close(fd);

    return 0;
}