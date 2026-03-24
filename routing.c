#include "routing.h"

int Send_NEIGHBOR(int neighbor_id_to_send, Node_info* My_node){
    char Routing_protocol[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_protocol, "NEIGHBOR %02d\n", My_node->id);

    if(My_node->debug){
        printf("Sending to %02d: %s", neighbor_id_to_send, Routing_protocol);
    }

    return_code = Send_routing_protocol_to_id(Routing_protocol, neighbor_id_to_send, My_node);

    return return_code;
}

int Send_COORD(int neighbor_id_to_send, int dest_id, Node_info* My_node){
    char Routing_cmd[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_cmd, "COORD %02d\n", dest_id);

    if(My_node->is_monitoring || My_node->debug){
        printf("Sending to %02d: %s", neighbor_id_to_send, Routing_cmd);
    }

    return_code = Send_routing_protocol_to_id(Routing_cmd, neighbor_id_to_send, My_node);

    return return_code;
}

int Send_UNCOORD(int neighbor_id_to_send, int dest_id, Node_info* My_node){
    char Routing_cmd[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_cmd, "UNCOORD %02d\n", dest_id);

    if(My_node->is_monitoring || My_node->debug){
        printf("Sending to %02d: %s", neighbor_id_to_send, Routing_cmd);
    }

    return_code = Send_routing_protocol_to_id(Routing_cmd, neighbor_id_to_send, My_node);

    return return_code;
}

int Send_ROUTE(int neighbor_id_to_send, int dest_id, Node_info* My_node){
    char Routing_cmd[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_cmd, "ROUTE %02d %d\n", dest_id, My_node->dist[dest_id]);

    if(My_node->is_monitoring || My_node->debug){
        printf("Sending to %02d: %s", neighbor_id_to_send, Routing_cmd);
    }

    return_code = Send_routing_protocol_to_id(Routing_cmd, neighbor_id_to_send, My_node);

    return return_code;
}

int Send_CHAT(int succ_id, int dest_id, char* chat_message, Node_info* My_node){
    char Chat_protocol[TCP_Chat_protocol_len];
    int return_code;

    sprintf(Chat_protocol, "CHAT %02d %02d %s\n", My_node->id, dest_id, chat_message);

    if(My_node->debug){
        printf("Sending to %02d: %s",succ_id , Chat_protocol);
    }

    return_code = Send_chat_protocol_to_id(Chat_protocol, succ_id, My_node);

    return return_code;
}

int Coord_neighbors(int dest_id, Node_info* My_node){
    int return_code;
    for(int i = 0, n_con = 0; i < Number_of_ids; i++){
        if(My_node->TCP_fd[i] != -1){
            if((i != My_node->id)){
                n_con++;
                return_code = Send_COORD(i, dest_id, My_node);
                if(return_code != 0) return return_code;

                My_node->pending_uncoord[dest_id]++;
            }
            if (n_con == My_node->number_of_TCP_channels) break;
        }
    }
    return 0;
}

int Route_neighbors(int dest_id, Node_info* My_node){
    int return_code;
    for(int i = 0, n_con = 0; i < Number_of_ids; i++){
        if(My_node->TCP_fd[i] != -1){
            if((i != My_node->id)){
                n_con++;
                return_code = Send_ROUTE(i, dest_id, My_node);
                if(return_code != 0) return return_code;
            }
            if (n_con == My_node->number_of_TCP_channels) break;
        }
    }
    return 0;
}

int process_TCP_message(char* input, int neigbor_id, Node_info* My_node){
    char Protocol[TCP_Routing_protocol_len];
    int dist_to_dest_id_from_neighbor, num_args, dest_id;

    num_args = sscanf(input, "%s", Protocol);

    if(num_args == 1){

        if(!strcmp(Protocol, "ROUTE")){

            if(sscanf(input, "%*s %d %d", &dest_id, &dist_to_dest_id_from_neighbor) == 2){

                if(My_node->is_monitoring || My_node->debug){
                    printf("Receiving from %02d: %s", neigbor_id, input);
                }

                return process_ROUTE_message(dest_id, dist_to_dest_id_from_neighbor, neigbor_id, My_node);

            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return ERR_UNEXPECTED;
            }

        }else if(!strcmp(Protocol, "COORD")){

            if(sscanf(input, "%*s %d", &dest_id) == 1){

                if(My_node->is_monitoring || My_node->debug){
                    printf("Receiving from %02d: %s", neigbor_id, input);
                }


                return process_COORD_message(dest_id, neigbor_id, My_node);

            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return ERR_UNEXPECTED;
            }

        }else if(!strcmp(Protocol, "UNCOORD")){

            if(sscanf(input, "%*s %d", &dest_id) == 1){

                if(My_node->is_monitoring || My_node->debug){
                    printf("Receiving from %02d: %s", neigbor_id, input);
                }


                return process_UNCOORD_message(dest_id, neigbor_id, My_node);

            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return ERR_UNEXPECTED;
            }

        }else if(!strcmp(Protocol, "CHAT")){

            if(sscanf(input, "%*s %*d %d", &dest_id) == 1){

                if(My_node->debug){
                    printf("Receiving from %02d: %s", neigbor_id, input);
                }

                return process_CHAT_message(input, dest_id, My_node);

            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return ERR_UNEXPECTED;
            }

        }else{
            //unknown protocol
            printf("DEBUG ERROR: (in function process_routing_message) if we are reading this we recived an unknown protocol (not suposed to do that)\n");
            return ERR_UNEXPECTED;
        }

    }else{
        //unexpected sscanf return
        printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
        return ERR_UNEXPECTED;
    }
    return SUCCESS;
}

int process_ROUTE_message(int dest_id, int dist_to_dest_id_from_neighbor, int neighbor_id, Node_info* My_node){
    if(dest_id == My_node->id){
        //ignore since is a echo of my own route message
        return SUCCESS;
    }

    int new_dist_to_dest_id, return_code;

    //calculate the new distance to dest_id if i go through neighbor_id
    if(dist_to_dest_id_from_neighbor == INF){
        new_dist_to_dest_id = INF;
    }else{
        new_dist_to_dest_id = dist_to_dest_id_from_neighbor + 1;
    }

    if(new_dist_to_dest_id < My_node->dist[dest_id] || neighbor_id == My_node->succ[dest_id]){
        //better way found or my succ found a new way

        if (My_node->adv_debug) {
            char dist_to_prt[4], new_dist_to_prt[12], succ_to_print[4];
            My_node->dist[dest_id] == INF ? (void)strcpy(dist_to_prt, "INF") : (void)sprintf(dist_to_prt, "%02d", My_node->dist[dest_id]);
            new_dist_to_dest_id == INF   ? (void)strcpy(new_dist_to_prt, "INF") : (void)sprintf(new_dist_to_prt, "%02d", new_dist_to_dest_id);
            My_node->succ[dest_id] == -1  ? (void)strcpy(succ_to_print, "-1")  : (void)sprintf(succ_to_print, "%02d", My_node->succ[dest_id]);
            printf("DEBUG [%02d]: dist %s -> %s | succ %s -> %02d\n", dest_id, dist_to_prt, new_dist_to_prt, succ_to_print, neighbor_id);
        }

        //update my route to dest_id to go through neighbor_id
        My_node->dist[dest_id] = new_dist_to_dest_id;
        My_node->succ[dest_id] = neighbor_id;

        //if i am in expedition state to dest_id send this new route to my neighbors
        if((My_node->state[dest_id] == STATE_EXPEDITION)){
            return_code = Route_neighbors(dest_id, My_node);
            if(return_code != SUCCESS) return return_code;

        }//else //need to wait before sending this new route until i am out of coordination state
    }//else //way given is not better that the one we already have
        
    return SUCCESS;
}

int process_COORD_message(int dest_id, int neighbor_id, Node_info* My_node){
    int return_code;

    if(My_node->state[dest_id] == STATE_COORDINATION){

        //i am already in cordination state send UNCOORD to avoid loops
        return_code = Send_UNCOORD(neighbor_id, dest_id, My_node);
        if(return_code != SUCCESS) return return_code;

    }else{
        
        //if i depend on neighbor_id to get to dest_id i lose my route to dest_id
        if(My_node->succ[dest_id] == neighbor_id){

            if (My_node->adv_debug) {
            char dist_to_prt[4], succ_to_print[4];
            My_node->dist[dest_id] == INF ? (void)strcpy(dist_to_prt, "INF") : (void)sprintf(dist_to_prt, "%02d", My_node->dist[dest_id]);
            My_node->succ[dest_id] == NO_SUCCESSOR  ? (void)strcpy(succ_to_print, "-1")  : (void)sprintf(succ_to_print, "%02d", My_node->succ[dest_id]);
            printf("DEBUG [%02d]: dist %s -> INF | succ %s -> -1 | state 0 -> 1\n", dest_id, dist_to_prt, succ_to_print);
            }

            //lost route updating routing table and entering coordination state
            My_node->dist[dest_id] = INF;
            My_node->succ[dest_id] = NO_SUCCESSOR;
            My_node->state[dest_id] = STATE_COORDINATION;
            My_node->succ_coord[dest_id] = neighbor_id;

            //coordinate neighbors to inform them that i lost my route to dest_id
            return_code = Coord_neighbors(dest_id, My_node);
            if(return_code != SUCCESS) return return_code;

        }else{
            //i dont depend on neighbor_id to get to dest_id sending him my route to dest_id to help him update his routing table

            return_code = Send_ROUTE(neighbor_id, dest_id, My_node);
            if(return_code != SUCCESS) return return_code;

            return_code = Send_UNCOORD(neighbor_id, dest_id, My_node);
            if(return_code != SUCCESS) return return_code;

        }
    }
    return SUCCESS;
}

int process_UNCOORD_message(int dest_id, int neighbor_id, Node_info* My_node){
    int return_code;

    //recived UNCOORD decrease the counter of pending UNCOORD from neighbors to dest_id
    My_node->pending_uncoord[dest_id]--;

    //if all UNCOORD have arrived i can exit coordination state
    if(My_node->pending_uncoord[dest_id] == 0){

        if (My_node->adv_debug) {
            printf("DEBUG [%02d]: state 1 -> 0\n", dest_id);
        }

        //exit coordination state
        My_node->state[dest_id] = STATE_EXPEDITION;
        
        //if i found a new way while i was in coordination state send it to all my neighbors
        if(My_node->dist[dest_id] != INF){
            //we got a route to dest_id, so spreed it
            return_code = Route_neighbors(dest_id, My_node);
            if(return_code != SUCCESS) return return_code;
        }//else //we didnt get a route to dest_id while we were in coordination state so we have nothing to spreed

        //if i have a successor that coordined me send him UNCOORD to inform him that i am out of coordination state
        if(My_node->succ_coord[dest_id] != NO_SUCCESSOR){
            return_code = Send_UNCOORD(My_node->succ_coord[dest_id], dest_id, My_node);
            if(return_code != SUCCESS) return return_code;
            My_node->succ_coord[dest_id] = NO_SUCCESSOR;
        }

    }//else //still whating on all coords to respond

    return SUCCESS;
}

int process_CHAT_message(char* Chat_protocol, int dest_id, Node_info* My_node){
    int return_code, origin_id;
    char chat_message[TCP_Chat_protocol_len];

    //if the message reached its destination print it
    if(My_node->id == dest_id){

        if(sscanf(Chat_protocol, "CHAT %d %*d %[^\n]", &origin_id, chat_message) == 2){

            printf("Recived message from %d\n%s\n", origin_id, chat_message);

            return SUCCESS;
        }else{
            //unexpected sscanf return
            printf("DEBUG ERROR: (in function process_CHAT_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
            return ERR_UNEXPECTED;
        }
    }

    //if my successor to dest_id is unreachable or in coordination state i cannot forward the message so i lose it
    if(My_node->state[dest_id] == STATE_COORDINATION || My_node->succ[dest_id] == NO_SUCCESSOR){
        printf("Cannot forward chat_message: Destination %d is unreachable or coordinating.\n", dest_id);
        return SUCCESS; // lose message
    }

    //else forward the message to my successor to dest_id
    return_code = Send_chat_protocol_to_id(Chat_protocol, My_node->succ[dest_id], My_node);
    if(return_code != SUCCESS) return return_code;

    return SUCCESS;
}

int process_NEIGHBOR_message(char* Routing_protocol, int newfd, int *sender_id, Node_info* My_node){
    char Neigbor_check[TCP_Routing_protocol_len];
    int return_code;

    if(sscanf(Routing_protocol, "%s %d", Neigbor_check, sender_id) != 2){
        printf("DEBUG ERROR: (in function process_NEIGHBOR_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
        return ERR_UNEXPECTED;
    }

    //check if the message starts with NEIGHBOR (this can only happen if the first message we receive is not a NEIGHBOR message)
    if(strcmp(Neigbor_check, "NEIGHBOR") != 0){
        printf("DEBUG ERROR: (in function process_NEIGHBOR_message) if we are reading this the message does not start with NEIGHBOR (not suposed to do that)\n");
        return ERR_UNEXPECTED;
    }

    if(My_node->debug){
        printf("Receiving from %02d: %s", (*sender_id), Routing_protocol);
    }

    //store the fd in the (id) position of the TCP_fd array
    My_node->TCP_fd[(*sender_id)] = newfd;

    //we now have "valid" TCP channel (it was already valid in the sense that we already had a TCP connection with the neighbor but we were waiting for him to identify himself )
    My_node->number_of_TCP_channels++;

    //share with the new neighbor all the routes i have in my routing table so he can update his routing table with this new information
    for(int i = 0; i < Number_of_ids; i++){
        if((My_node->dist[i] < INF && My_node->state[i] == STATE_EXPEDITION)){
            return_code = Send_ROUTE((*sender_id), i, My_node);
            if(return_code != SUCCESS) return return_code;
        }
    }

    return SUCCESS;

}