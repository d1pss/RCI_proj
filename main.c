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
#include "cmd.h"
#include "cmd_aux.h"
#include "TCP.h"

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





