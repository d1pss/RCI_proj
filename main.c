#include "struct_and_constants.h"
#include "cmd.h"
#include "cmd_aux.h"
#include "server_client_comunication.h"
#include "routing.h"

int main(int argc, char *argv[]){
    
    if(Check_argv_format(argv, argc) == 1){
        return 0;
    }
    
    Node_info* My_node = init_Node(argv, argc);
    if(My_node == NULL) return 0;

    



    int nfds, i, n_con, return_code;
    char TCP_message[TCP_Chat_protocol_len], Terminal_cmd[cmd_len];
    fd_set fdset;

    while(true){
        FD_ZERO(&fdset);
        FD_SET(STDIN_FILENO, &fdset);

        nfds = STDIN_FILENO;

        if(My_node->is_in_net){

            FD_SET(My_node->TCP_fd[My_node->id],&fdset);

            nfds = max(nfds, My_node->TCP_fd[My_node->id]);
        
            for(i = 0, n_con = 0; i < Number_of_ids; i++){
                if(My_node->TCP_fd[i] != -1){
                    if((i =! My_node->id)){
                        n_con++;
                        FD_SET(My_node->TCP_fd[i],&fdset);
                        nfds = max(nfds, My_node->TCP_fd[i]);
                    }
                    if(n_con == My_node->number_of_TCP_channels) break;
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
            printf("ERROR: select error\n");
            return 0;
            break;
        
        default:
            if(FD_ISSET(STDIN_FILENO, &fdset)){
                //recived a terminal command
                if (fgets(Terminal_cmd, cmd_len, stdin) != NULL){

                    return_code = process_command(Terminal_cmd, My_node);
                    if(manage_return_code(return_code) == 1) return 0;

                }
            } 
            if(My_node->is_in_net){
                if(FD_ISSET(My_node->TCP_fd[My_node->id],&fdset)){
                    //A client is trying to connect need to accept
                    return_code = accept_TCP_connection(My_node);
                    if(manage_return_code(return_code) == 1) return 0;

                }

                for(i = 0; i < Number_of_ids; i++){
                    if(My_node->TCP_fd[i] != -1){
                        if((i =! My_node->id)){
                            if(FD_ISSET(My_node->TCP_fd[i],&fdset)){
                                //the id (i) is sending us a message

                                return_code = Recive_message_from_id(TCP_message, TCP_Chat_protocol_len, i, My_node);
                                if(manage_return_code(return_code) == 1) return 0;

                                return_code = process_TCP_message(TCP_message, i, My_node);
                                if(manage_return_code(return_code) == 1) return 0;

                                break;
                            }
                        }
                    }
                }

            }
            break;
        }
    }

    return 0;
}


//return 0 continue program
//return 1 exit program
int manage_return_code(int return_code){
    if(return_code == 0 || return_code == 3){
        return 0;
    }

    if(return_code == 1 || return_code == 4 || return_code == 5 || return_code == 6){
        return 1;
    }

    char response;

    printf("do you wish to proced with the program?\n[y/n]\n");
    scanf("%c", &response);
    if(response == 'y'){
        return 1;
    }else if(response == 'n'){
        return 0;
    }else{
        printf("unknown response exting...\n");
        return 1;
    }

}

