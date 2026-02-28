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

#include "struct_and_constants.h"

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

bool is_string_a_number(char* string){
    char* endptr;
    // Convert string to long, endptr stores the end conversion point
    long p = strtol(string, &endptr, 10);

    // if *endptr is not \0, there was thrash in the sring
    if (string == endptr || *endptr != '\0') return false;

    return true;
}



