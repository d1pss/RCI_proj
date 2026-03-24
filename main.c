#include "struct_and_constants.h"
#include "cmd.h"
#include "cmd_aux.h"
#include "server_client_comunication.h"
#include "routing.h"

/***********************************************************************************************
 * @brief Main entry point for the OWR Node application.
 *
 * Implements an event-driven loop using select() to multiplex between:
 * 
 * - 1. User input (Standard Input).
 * 
 * - 2. New incoming TCP connection requests (Server Socket).
 * 
 * - 3. Pending TCP connections awaiting identification (NEIGHBOR message).
 * 
 * - 4. Active neighbor connections (Routing and Chat protocols).
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns 0 on successful termination.
 ***********************************************************************************************/
int main(int argc, char *argv[]){
    
    //Validate command-line arguments and initialize the node's state
    if(Check_argv_format(argv, argc) == EXIT_OK){
        return SUCCESS;
    }
    
    //Initialize the Node_info structure with TCP/UDP configurations and default values
    Node_info* My_node = init_Node(argv, argc);
    if(My_node == NULL) return SUCCESS;

    //Main loop variables
    int nfds, i, j, n_con, return_code, n_of_frag_inputs, NEIGHBOR_id;
    char TCP_buffer[TCP_buffer_len], Terminal_cmd[cmd_len], **fragmented_input;
    fd_set fdset;

    //Main loop
    while(true){
        //Prepare the file descriptor set for select()
        FD_ZERO(&fdset);

        //Always monitor standard input for user commands
        FD_SET(STDIN_FILENO, &fdset);
        nfds = STDIN_FILENO;


        if(My_node->is_in_net){

            //Monitor the TCP server socket for new incoming connections
            FD_SET(My_node->TCP_fd[My_node->id],&fdset);
            nfds = max(nfds, My_node->TCP_fd[My_node->id]);

            //Monitor pending TCP connections awaiting NEIGHBOR identification
            for(i = 0; i < My_node->number_pending_fd; i++){
                if(My_node->TCP_pending_fd[i] == UNUSED_FD) break;
                FD_SET(My_node->TCP_pending_fd[i],&fdset);
                nfds = max(nfds, My_node->TCP_pending_fd[i]);
            }
            
            //Monitor active TCP connections with neighbors for incoming messages
            for(i = 0, n_con = 0; i < Number_of_ids; i++){
                if(My_node->TCP_fd[i] != UNUSED_FD && (i != My_node->id)){
                    n_con++;
                    FD_SET(My_node->TCP_fd[i],&fdset);
                    nfds = max(nfds, My_node->TCP_fd[i]);
                    if(n_con == My_node->number_of_TCP_channels) break;
                }
            }
        }

        //Wait for any of the monitored file descriptors to become active
        switch (select(nfds+1, &fdset, NULL, NULL, NULL))
        {
        case 0:
            //timeout (should not happen since we have no timeout set)
            break;

        case UNUSED_FD:
            //error
            printf("ERROR (main loop select)\n");
            if(manage_return_code(ERR_UNEXPECTED, My_node) == EXIT_OK) return SUCCESS;
            break;
        
        default:
            if(FD_ISSET(STDIN_FILENO, &fdset)){
                //the user is sending a command
                if (fgets(Terminal_cmd, cmd_len, stdin) != NULL){

                    return_code = process_command(Terminal_cmd, My_node);
                    if(manage_return_code(return_code, My_node) == EXIT_OK) return SUCCESS;

                }
            } 
            
            if(My_node->is_in_net){

                for(i = 0; i < Number_of_ids; i++){
                    if(My_node->TCP_fd[i] != UNUSED_FD && (i != My_node->id)){
                        if(FD_ISSET(My_node->TCP_fd[i],&fdset)){
                            //reciving ROUTE, COORD, UNCOORD or CHAT message from neighbor i

                            return_code = Recive_message_from_fd(TCP_buffer, My_node->TCP_fd[i], My_node);

                            //TCP connection was closed by the other side
                            if(return_code == STATUS_SPECIFIC){
                                char sender_id_as_char[Id_len];
                                sprintf(sender_id_as_char, "%02d", i);
                                return_code = cmd_remove_edge(sender_id_as_char, My_node);
                                if(manage_return_code(return_code, My_node) == EXIT_OK) return SUCCESS;
                                continue;
                            }

                            if(manage_return_code(return_code, My_node) == EXIT_OK) return SUCCESS;

                            //fragmenting the buffer in case we recived more than one message at once
                            return_code = fragment_buffer(TCP_buffer, &fragmented_input, &n_of_frag_inputs);
                            if(manage_return_code(return_code, My_node) == EXIT_OK) return SUCCESS;

                            //process each message in the buffer
                            for(j = 0; j < n_of_frag_inputs; j++){
                                return_code = process_TCP_message(fragmented_input[j], i, My_node);
                                if(manage_return_code(return_code, My_node) == EXIT_OK) return SUCCESS;
                            }
                            
                            free_frag_buffer(fragmented_input, n_of_frag_inputs);

                        }
                        
                    }
                }

                for(i = 0; i < My_node->number_pending_fd; i++){
                    if(My_node->TCP_pending_fd[i] == UNUSED_FD) break;
                    if(FD_ISSET(My_node->TCP_pending_fd[i],&fdset)){
                        //reciving the NEIGHBOR message from a pending connection to identify the neighbor and move the fd to the active list

                        return_code = Recive_message_from_fd(TCP_buffer, My_node->TCP_pending_fd[i], My_node);

                        //TCP connection was closed by the other side
                        if(return_code == STATUS_SPECIFIC){
                            close(My_node->TCP_pending_fd[i]);
                            remove_pending_fd(i, My_node);
                            My_node->number_pending_fd--;
                            if(My_node->TCP_pending_fd[i] != UNUSED_FD){
                                i--;
                            }
                            continue;
                        }

                        if(manage_return_code(return_code, My_node) == EXIT_OK) return SUCCESS;

                        //fragmenting the buffer in case we recived more than one message at once
                        return_code = fragment_buffer(TCP_buffer, &fragmented_input, &n_of_frag_inputs);
                        if(manage_return_code(return_code, My_node) == EXIT_OK) return SUCCESS;

                        //the first message in the buffer has to be the NEIGHBOR message because it is first message the other side sends after connecting
                        return_code = process_NEIGHBOR_message(fragmented_input[0], My_node->TCP_pending_fd[i], &NEIGHBOR_id, My_node);
                        if(manage_return_code(return_code, My_node) == EXIT_OK) return SUCCESS;
                        
                        //after processing the NEIGHBOR message we can remove the fd from the pending list
                        remove_pending_fd(i, My_node);
                        My_node->number_pending_fd--;
                        if(My_node->TCP_pending_fd[i] != UNUSED_FD){
                            i--;
                        }
                        
                        //process the rest of the messages in the buffer as normal messages from that neighbor
                        for(j = 1; j < n_of_frag_inputs; j++){
                            return_code = process_TCP_message(fragmented_input[j], NEIGHBOR_id, My_node);
                            if(manage_return_code(return_code, My_node) == EXIT_OK) return SUCCESS;
                        }
                        
                        free_frag_buffer(fragmented_input, n_of_frag_inputs);
                    }
                }

                if(FD_ISSET(My_node->TCP_fd[My_node->id],&fdset)){
                    //A client is trying to connect need to accept the connection 
                    //and add the new fd to the pending list to wait for the NEIGHBOR message that will identify the neighbor
                    return_code = accept_TCP_connection(My_node);
                    if(manage_return_code(return_code, My_node) == EXIT_OK) return SUCCESS;

                }

            }
            break;
        }
    }

    free(My_node);

    return 0;
}