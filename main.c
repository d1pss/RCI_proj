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
#define Id_len 3 // ids are two digit numbers so xx\0 thet is 3 chars

typedef struct _Node_information{
    //TCP info for node to node connection
    char Node_TCP_IP[Max_IP_len];
    char Node_TCP_Port[Max_Port_len];
    int TCP_fd[Number_of_ids];
    int number_of_TCP_chanels;

    //UDP info for node to network connection
    char UDP_Server_IP[Max_IP_len];
    char UDP_Server_Port[Max_Port_len];

    //Node info
    char id[Id_len];
    char net[4];
    int dist[Number_of_ids];
    int succ[Number_of_ids];
    bool state[Number_of_ids];

    //flags
    bool is_in_net;
    bool is_monitoring;
}Node_info;

int main(int argc, char *argv[]){
    
    Check_argv_format(argv, argc);
    
    Node_info* My_node = init_Node(argv, argc);
    if(My_node == NULL) return 1;

    



    int nfds, i, n_con;
    fd_set fdset;

    while(true){
        FD_ZERO(&fdset);
        FD_SET(STDIN_FILENO, &fdset);

        nfds = STDIN_FILENO;

        if(My_node->is_in_net){

            FD_SET(My_node->TCP_fd[atoi(My_node->id)],&fdset);

            nfds = max(nfds, My_node->TCP_fd[atoi(My_node->id)]);
        
            for(i = 0, n_con = 0; i < Number_of_ids; i++){
                if(My_node->TCP_fd[i] != -1){
                    if(i =! atoi(My_node->id)){
                        n_con++;
                        FD_SET(My_node->TCP_fd[i],&fdset);
                        nfds = max(nfds, My_node->TCP_fd[i]);
                    }
                    if(n_con == My_node->number_of_TCP_chanels) break;
                }
            }
        }

        switch (select(nfds+1, &fdset, NULL, NULL, NULL))
        {
        case 0:
            //timeout
            break;

        case -1:
            //error
            break;
        
        default:
            if(FD_ISSET(STDIN_FILENO, &fdset)){
                //recived a terminal command

            } 
            if(My_node->is_in_net){
                if(FD_ISSET(My_node->TCP_fd[atoi(My_node->id)],&fdset)){
                    //A client is trying to connect need to accept


                }

                for(i = 0, n_con = 0; i < Number_of_ids; i++){
                    if(My_node->TCP_fd[i] != -1){
                        if(i =! atoi(My_node->id)){
                            n_con++;
                            if(FD_ISSET(My_node->TCP_fd[i],&fdset)){
                                //the id (i) is sending us a message

                                

                            }
                        }
                        if(n_con == My_node->number_of_TCP_chanels) break;
                    }
                }

            }
            break;
        }
    }

    return 0;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

void print_help(void){
    printf("join (j) net id .   .   .   .   .   . | register node as (id) in the (net)\n\\
            show nodes (n) net  .   .   .   .   . | print nodes registred in (net)\n\\
            leave (l)   .   .   .   .   .   .   . | unregister node\n\\
            exit (x)    .   .   .   .   .   .   . | exit the program\n\\
            add edge (ae) id    .   .   .   .   . | create a TCP chanel between node and destiny (id)\n\\
            remove edge (re) id .   .   .   .   . | closes the TCP chanel between node and destiny (id)\n\\
            show neighbors (sg) .   .   .   .   . | print the ids directly connected to the node\n\\
            announce (a)    .   .   .   .   .   . | announce the node to the other nodes in the network\n\\
            show routing (sr) dest  .   .   .   . | ---falta escrever---\n\\
            start monitor (sm)  .   .   .   .   . | ---falta escrever---\n\\
            end monitor (em)    .   .   .   .   . | ---falta escrever---\n\\
            message (m) dest message    .   .   . | send (message) to node (dest)\n\\
            direct join (dj) net id .   .   .   . | ---falta escrever---\n\\
            direct add edge (dae) id idIP idTCP . | ---falta escrever---\n");
    return;
}

void Check_argv_format(char** argv, int argc){
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
            printf("regIP used in ./OWR IP TCP --> regIP <-- regUDP does not folow the standard IPv4 structure\n");
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
}

bool is_IP_invalid(char* IP) {
    int a, b, c, d;
    int last_index = 0;

    // %n stores the index of the last element into last_index
    if (sscanf(IP, "%d.%d.%d.%d%n", &a, &b, &c, &d, &last_index) != 4) {
        //inalid format
        return true; 
    }

    //if the last_index is no a end of string the IP contains extra chars
    if (IP[last_index] != '\0') {
        return true;
    }

    //check if the numbers are between the porper values
    if (a < 0 || a > 255 || b < 0 || b > 255 || 
        c < 0 || c > 255 || d < 0 || d > 255) {
        return true;
    }

    return false;
}

bool is_Port_invalid(char* Port) {
    char* endptr;
    // Convert string to long, endptr stores the end conversion point
    long p = strtol(Port, &endptr, 10);

    
    // if *endptr is not \0, there was thrash in the sring
    if (Port == endptr || *endptr != '\0') return true;

    //check if it is a valid Port
    if (p < 1 || p > 65535) return true;

    return false;
}

Node_info* init_Node(char** argv, int argc){
    Node_info* My_node = (Node_info*)malloc(sizeof(Node_info));
    if(My_node == NULL){
        printf("ERROR: error alocating memory");
        return NULL;
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

    for(int i = 0; i < Number_of_ids; i++){
        My_node->dist[i] = -1;
        My_node->succ[i] = -1;
        My_node->state[i] = 0;
        My_node->TCP_fd[i] = -1;
    }

    My_node->number_of_TCP_chanels = 0;

    My_node->is_in_net = false;
    My_node->is_monitoring = false;

    return My_node;
}



//return 0 return to main without error
//return 1 return to main to exit the program without errors
//return 2 return to main to with network error
//return 3 return to main to with incorrect input
//return 4 return to main to exit the program with memory allocation error
//return 5 return to main to exit the program with seting up UDP or TCP error
//return 6 return to main to exit the program with not suposed to happend case
int process_command(char *input, Node_info* My_node){
    char arguments[Max_cmd_arguments][Max_cmd_len], cmd[Max_cmd_len];
    int num_args, return_code;
    
    num_args = sscanf(input, "%s %s %s %s", &cmd, &arguments[0], &arguments[1], &arguments[2]);

    if(num_args == 1){
        if(!strcmp(cmd, "l")){
            if(My_node->is_in_net){
                //the node is in the network, so leave
                //cmd_leave();
                
                My_node->is_in_net = false;

                return 0;
            }else{
                //the node is not in the network, so no need to leave
                printf("The node is not in the network, so no need to leave\n");

                return 0;
            }
            

        }else if(!strcmp(cmd, "x")){

            //in case we forget to leave before exiting
            if(My_node->is_in_net){
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

            return cmd_join(arguments, My_node);

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

int cmd_join(char** arguments, Node_info* My_node){
    int return_code;
    bool does_id_exist;

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

    if(My_node->is_in_net){
        //no need to join node is already in th network
        printf("The node is allready in the network\n");
        return 0;
    }

    return_code = get_id_info(&does_id_exist ,arguments[0], arguments[1], My_node);

    if(return_code != 0){
        return return_code;
    }

    if(does_id_exist){
        //error id already exists must use a difrent one
        printf("The id chosen (%s) is in use. Must use a difrent id to add the node to the network %s\n", arguments[1], arguments[0]);
        return 0;
    }

    return_code = add_id_to_net(arguments[0], arguments[1], My_node);

    if(return_code != 0){
        if(return_code == 7){
            return 0;
        }
        return return_code;
    }

    strcpy(My_node->net, arguments[0]);
    strcpy(My_node->id, arguments[1]);
    My_node->is_in_net = true;

    return_code = Create_TCP_Server(My_node);

    return return_code;

}

int Create_TCP_Server(Node_info* My_node){
    int errcode;
    ssize_t n;
    struct addrinfo hints,*res;

    My_node->TCP_fd[atoi(My_node->id)]=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (My_node->TCP_fd[atoi(My_node->id)]==-1) return 5; //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(NULL,My_node->Node_TCP_Port,&hints,&res);
    if((errcode)!=0)/*error*/return 5;

    n=bind(My_node->TCP_fd[atoi(My_node->id)],res->ai_addr,res->ai_addrlen);
    if(n==-1) /*error*/ return 5;

    if(listen(My_node->TCP_fd[atoi(My_node->id)],5)==-1)/*error*/return 5;

    freeaddrinfo(res);

   return 0;
}

int accept_TCP_connection(Node_info* My_node){
    ssize_t n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    int newfd;
    char cli_id[Id_len];

    addrlen=sizeof(addr);
    if((newfd=accept(My_node->TCP_fd[atoi(My_node->id)],(struct sockaddr*)&addr,&addrlen))==-1)
    /*error*/ return 5;

    n=read(newfd,cli_id,3);
    if(n==-1)/*error*/return 5;

    My_node->TCP_fd[atoi(cli_id)] = newfd;

    My_node->number_of_TCP_chanels++;

    return 0;
}

int Close_TCP_Server(Node_info* My_node){
    close(My_node->TCP_fd[atoi(My_node->id)]);

    My_node->TCP_fd[atoi(My_node->id)] = -1;

    return 0;
}

int Create_and_Connect_TCP_client(char* dest_IP, char* dest_Port, char* dest_id, Node_info* My_node){
    int errcode;
    ssize_t n;
    struct addrinfo hints,*res;

    My_node->TCP_fd[atoi(dest_id)]=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (My_node->TCP_fd[atoi(dest_id)]==-1) return 5; //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode=getaddrinfo(dest_IP,dest_Port,&hints,&res);
    if(errcode!=0)/*error*/return 5;

    n=connect(My_node->TCP_fd[atoi(dest_id)],res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/return 5;

    n=write(My_node->TCP_fd[atoi(dest_id)],My_node->id,3);
    if(n==-1)/*error*/return 5;

    My_node->number_of_TCP_chanels++;

    freeaddrinfo(res);

    return 0;
}

int Close_TCP_Client(char* dest_id, Node_info* My_node){
    close(My_node->TCP_fd[atoi(dest_id)]);

    My_node->TCP_fd[atoi(dest_id)] = -1;
    My_node->number_of_TCP_chanels--;

    return 0;
}

int cmd_add_edge(char* dest_id, Node_info* My_node){
    int return_code;
    char dest_ip[Max_IP_len], dest_Port[Max_Port_len];
    bool is_dest_id_in_net;

    if(!My_node->is_in_net){
        printf("Origin node is not in the network use the join comand first\n");
        return 0;
    }

    return_code = get_id_info(&dest_ip, &dest_Port, &is_dest_id_in_net, My_node->net, dest_id, My_node);

    if(return_code != 0){
        return return_code;
    }

    if(!is_dest_id_in_net){
        printf("The destiny id does not exist in network use show nodes to know the ids in network\n");
        return 0;
    }

    return_code = Create_and_Connect_TCP_client(dest_ip, dest_Port, dest_id, My_node);

    if(return_code != 0){
        return return_code;
    }



    return 0;
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

void cmd_leave(Node_info* My_node){
    char message[Max_message_len], *response, op, neigbours[Number_of_ids][Id_len];
    int n_neighbours, i;
    
    //loop para verificar quantos vizinhos tem e quais são...
     for(i = 0, n_con = 0; i < Number_of_ids; i++){
                if(My_node->TCP_fd[i] != -1){
                    if(i =! atoi(My_node->id)){
                        sprintf(neigbours[n_con], "%02d", i);
                        n_con++;
                    }
                    if(n_con == My_node->number_of_TCP_chanels) break;
                }
            }
    //loop para fechar ligação tcp com os vizinhos...
    for(i = 0; i < n_con; i++){
        cmd_remove_edge(neigbours[i], My_node);
    }

    sprintf(message, "REG 100 3 %s %s\n", My_node->net, My_node->id);
    send_message_to_UDP_server(message, &response, Server);
    int items_found = sscanf(response, "%*s %*s %c", &op);
    if (items_found == 1) {
        if (op == '4') {
            // id succesfully left
            free(response);
            Close_TCP_Server(Node_info* My_node);
            return 0;
        }else{
        //error code from network
        free(response);
        return 2 ;
        }
    }
    

}

//return 7 means the net is full
int add_id_to_net(char* net, char* id, Node_info* My_node){
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
            return 7;
        }else{
            //error code from network
            printf("ERROR: error code (%c) from network using this comand %s\n", op, message);

            free(response);
            return 2;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function add_id_to_net) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        free(response);
        return 6;
    }
}


void cmd_remove_edge(char* id_to_remove, Node_info* My_node){
    int return_code;
    if(!My_node->is_in_net){
        //mensagem de erro que o node n existe na net
    }
    if(My_node->TCP_fd[atoi(id_to_remove)] == -1){
        //mensagem de erro que o node n é vizinho
    }


}

int get_neighbours(){

    return;
}

int get_id_info(char** id_IP ,char** id_Port ,bool* get_id_info ,char* net, char* id, Node_info* My_node){
    char message[Max_message_len];
    char* response, op;
    int response_len, return_code;

    sprintf(message, "CONTACT 100 0 %s %s\n", net, id);
   

    return_code = send_message_to_UDP_server(message, &response, &response_len, My_node);

    if(return_code != 0){
        return return_code;
    }
 
    int items_found = sscanf(response, "%*s %*s %c %*s %s %s", &op, id_IP, id_Port);

    if (items_found == 1 || items_found == 3) {
        if (op == '1') {
            // ID exists in net
            *get_id_info = true;

            free(response);
            return 0;
        } else if (op == '2') {
            // ID does not exist in net
            *get_id_info = false;

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
        printf("DEBUG ERROR: (in function add_id_to_net) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
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