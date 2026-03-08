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
        return SUCCESS;
    }

    int new_dist_to_dest_id, return_code;

    if(dist_to_dest_id_from_neighbor == INF){
        new_dist_to_dest_id = INF;
    }else{
        new_dist_to_dest_id = dist_to_dest_id_from_neighbor + 1;
    }

    if(new_dist_to_dest_id < My_node->dist[dest_id] || neighbor_id == My_node->succ[dest_id]){
        //better way found or my succ found a new wayr

        if (My_node->adv_debug) {
            char dist_to_prt[4], new_dist_to_prt[12], succ_to_print[4];
            My_node->dist[dest_id] == INF ? (void)strcpy(dist_to_prt, "INF") : (void)sprintf(dist_to_prt, "%02d", My_node->dist[dest_id]);
            new_dist_to_dest_id == INF   ? (void)strcpy(new_dist_to_prt, "INF") : (void)sprintf(new_dist_to_prt, "%02d", new_dist_to_dest_id);
            My_node->succ[dest_id] == -1  ? (void)strcpy(succ_to_print, "-1")  : (void)sprintf(succ_to_print, "%02d", My_node->succ[dest_id]);
            printf("DEBUG [%02d]: dist %s -> %s | succ %s -> %02d\n", dest_id, dist_to_prt, new_dist_to_prt, succ_to_print, neighbor_id);
        }

        My_node->dist[dest_id] = new_dist_to_dest_id;
        My_node->succ[dest_id] = neighbor_id;

        if((My_node->state[dest_id] == STATE_EXPEDITION)){
            //send new route to neigbors (exept neighbor_id this feture is not in use)
            return_code = Route_neighbors(dest_id, My_node);
            if(return_code != SUCCESS) return return_code;

        }//else //need to wait before sending this new route
    }//else //way given is not better that the one we already have
        
    return SUCCESS;
}

int process_COORD_message(int dest_id, int neighbor_id, Node_info* My_node){
    int return_code;

    if(My_node->state[dest_id] == STATE_COORDINATION){

        return_code = Send_UNCOORD(neighbor_id, dest_id, My_node);
        if(return_code != SUCCESS) return return_code;

    }else{

        if(My_node->succ[dest_id] == neighbor_id){
            //i lost my route to dest_id

            if (My_node->adv_debug) {
            char dist_to_prt[4], succ_to_print[4];
            My_node->dist[dest_id] == INF ? (void)strcpy(dist_to_prt, "INF") : (void)sprintf(dist_to_prt, "%02d", My_node->dist[dest_id]);
            My_node->succ[dest_id] == NO_SUCCESSOR  ? (void)strcpy(succ_to_print, "-1")  : (void)sprintf(succ_to_print, "%02d", My_node->succ[dest_id]);
            printf("DEBUG [%02d]: dist %s -> INF | succ %s -> -1 | state 0 -> 1\n", dest_id, dist_to_prt, succ_to_print);
            }

            My_node->dist[dest_id] = INF;
            My_node->succ[dest_id] = NO_SUCCESSOR;
            My_node->state[dest_id] = STATE_COORDINATION;
            My_node->succ_coord[dest_id] = neighbor_id;

            return_code = Coord_neighbors(dest_id, My_node);
            if(return_code != SUCCESS) return return_code;

        }else{
            //i dont depend on neighbor_id to get to dest_id, so send Route to neighbor_id

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

    My_node->pending_uncoord[dest_id]--;

    if(My_node->pending_uncoord[dest_id] == 0){
        //all coords have responded

        if (My_node->adv_debug) {
            printf("DEBUG [%02d]: state 1 -> 0\n", dest_id);
        }

        My_node->state[dest_id] = STATE_EXPEDITION;
        
        if(My_node->dist[dest_id] != INF){
            //we got a route to dest_id, so spreed it
            return_code = Route_neighbors(dest_id, My_node);
            if(return_code != SUCCESS) return return_code;
        }//else //we didnt get a route to dest_id there is no way available to dest_id

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

    if(My_node->id == dest_id){
        //the message reached its destination
        if(sscanf(Chat_protocol, "CHAT %d %*d %[^\n]", &origin_id, chat_message) == 2){

            printf("Recived message from %d\n%s\n", origin_id, chat_message);

            return SUCCESS;
        }else{
            //unexpected sscanf return
            printf("DEBUG ERROR: (in function process_CHAT_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
            return ERR_UNEXPECTED;
        }
    }

    if(My_node->state[dest_id] == STATE_COORDINATION || My_node->succ[dest_id] == NO_SUCCESSOR){
        printf("Cannot forward chat_message: Destination %d is unreachable or coordinating.\n", dest_id);
        return SUCCESS; // lose message
    }

    return_code = Send_chat_protocol_to_id(Chat_protocol, My_node->succ[dest_id], My_node);
    if(return_code != SUCCESS) return return_code;

    return SUCCESS;
}

int process_NEIGHBOR_message(char* Routing_protocol, int newfd, int *sender_id, Node_info* My_node){
    int return_code;

    if(sscanf(Routing_protocol, "%*s %d", sender_id) != 1){
        printf("DEBUG ERROR: (in function process_NEIGHBOR_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
        return ERR_UNEXPECTED;
    }

    if(My_node->debug){
        printf("Receiving from %02d: %s", (*sender_id), Routing_protocol);
    }

    My_node->TCP_fd[(*sender_id)] = newfd;

    My_node->number_of_TCP_channels++;

    for(int i = 0; i < Number_of_ids; i++){
        if((My_node->dist[i] < INF && My_node->state[i] == STATE_EXPEDITION)){
            return_code = Send_ROUTE((*sender_id), i, My_node);
            if(return_code != SUCCESS) return return_code;
        }
    }

    return SUCCESS;

}