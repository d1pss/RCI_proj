#include "routing.h"

int Send_NEIGHBOR(int neighbor_id_to_send, Node_info* My_node){
    char Routing_protocol[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_protocol, "NEIGHBOR %02d\n", My_node->id);

    return_code = Send_routing_protocol_to_id(Routing_protocol, neighbor_id_to_send, My_node);

    return return_code;
}

int Send_COORD(int neighbor_id_to_send, int dest_id, Node_info* My_node){
    char Routing_cmd[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_cmd, "COORD %02d\n", dest_id);

    return_code = Send_routing_protocol_to_id(Routing_cmd, neighbor_id_to_send, My_node);

    return return_code;
}

int Send_UNCOORD(int neighbor_id_to_send, int dest_id, Node_info* My_node){
    char Routing_cmd[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_cmd, "UNCOORD %02d\n", dest_id);

    return_code = Send_routing_protocol_to_id(Routing_cmd, neighbor_id_to_send, My_node);

    return return_code;
}

int Send_ROUTE(int neighbor_id_to_send, int dest_id, Node_info* My_node){
    char Routing_cmd[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_cmd, "ROUTE %02d %d\n", dest_id, My_node->dist[dest_id]);

    return_code = Send_routing_protocol_to_id(Routing_cmd, neighbor_id_to_send, My_node);

    return return_code;
}

int Send_CHAT(int succ_id, int dest_id, char* chat_message, Node_info* My_node){
    char Chat_protocol[TCP_Chat_protocol_len];
    int return_code;

    sprintf(Chat_protocol, "CHAT %02d %02d %[^\n]\n", My_node->id, dest_id, chat_message);

    return_code = Send_chat_protocol_to_id(Chat_protocol, succ_id, My_node);

    return return_code;
}

//the except id can go as -1 as to not exept any id
int Coord_neighbors(int dest_id, int except_id, Node_info* My_node){
    int return_code;
    for(int i = 0, n_con = 0; i < Number_of_ids; i++){
        if(My_node->TCP_fd[i] != -1){
            if((i != My_node->id) && (i != except_id)){
                n_con++;
                return_code = Send_COORD(i, dest_id, My_node);
                if(return_code != 0) return return_code;

                My_node->pending_uncoord[dest_id]++;
            }
            if (n_con == My_node->number_of_TCP_channels - (except_id != -1)) break;
        }
    }
    return 0;
}

//the except id can go as -1 as to not exept any id
int Route_neighbors(int dest_id, int except_id, Node_info* My_node){
    int return_code;
    for(int i = 0, n_con = 0; i < Number_of_ids; i++){
        if(My_node->TCP_fd[i] != -1){
            if((i != My_node->id) && (i != except_id)){
                n_con++;
                return_code = Send_ROUTE(i, dest_id, My_node);
                if(return_code != 0) return return_code;
            }
            if (n_con == My_node->number_of_TCP_channels - (except_id != -1)) break;
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

                return process_ROUTE_message(dest_id, dist_to_dest_id_from_neighbor, neigbor_id, My_node);

            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return 6;
            }

        }else if(!strcmp(Protocol, "COORD")){

            if(sscanf(input, "%*s %d", &dest_id) == 1){

                return process_COORD_message(dest_id, neigbor_id, My_node);

            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return 6;
            }

        }else if(!strcmp(Protocol, "UNCOORD")){

            if(sscanf(input, "%*s %d", &dest_id) == 1){

                return process_UNCOORD_message(dest_id, neigbor_id, My_node);

            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return 6;
            }

        }else if(!strcmp(Protocol, "CHAT")){

            if(sscanf(input, "%*s %*d %d", &dest_id) == 1){

                return process_CHAT_message(input, dest_id, My_node);

            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return 6;
            }

        }else{
            //unknown protocol
            printf("DEBUG ERROR: (in function process_routing_message) if we are reading this we recived an unknown protocol (not suposed to do that)\n");
            return 6;
        }

    }else{
        //unexpected sscanf return
        printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
        return 6;
    }
    return 0;
}

int process_ROUTE_message(int dest_id, int dist_to_dest_id_from_neighbor, int neighbor_id, Node_info* My_node){
    if(dest_id == My_node->id){
        return 0;
    }

    int new_dist_to_dest_id = dist_to_dest_id_from_neighbor + 1, return_code;

    if(new_dist_to_dest_id < My_node->dist[dest_id] || neighbor_id == My_node->succ[dest_id]){
        //better way found or my succ found a new way
        My_node->dist[dest_id] = new_dist_to_dest_id;
        My_node->succ[dest_id] = neighbor_id;

        if((My_node->state[dest_id] == 0)){
            //send new route to neigbors exept neighbor_id
            return_code = Route_neighbors(dest_id, neighbor_id, My_node);
            if(return_code != 0) return return_code;

        }//else //need to wait before sending this new route
    }//else //way given is not better that the one we already have
        
    return 0;
}

int process_COORD_message(int dest_id, int neighbor_id, Node_info* My_node){
    int return_code;

    if(My_node->state[dest_id] == 1){

        return_code = Send_UNCOORD(neighbor_id, dest_id, My_node);
        if(return_code != 0) return return_code;

    }else{

        if(My_node->succ[dest_id] == neighbor_id){
            //i lost my route to dest_id

            My_node->dist[dest_id] = INF;
            My_node->succ[dest_id] = -1;
            My_node->state[dest_id] = 1;
            My_node->succ_coord[dest_id] = neighbor_id;

            return_code = Coord_neighbors(dest_id, neighbor_id, My_node);
            if(return_code != 0) return return_code;

        }else{
            //i dont depend on neighbor_id to get to dest_id, so send Route to neighbor_id

            return_code = Send_ROUTE(neighbor_id, dest_id, My_node);
            if(return_code != 0) return return_code;

            return_code = Send_UNCOORD(neighbor_id, dest_id, My_node);
            if(return_code != 0) return return_code;

        }
    }
    return 0;
}

int process_UNCOORD_message(int dest_id, int neighbor_id, Node_info* My_node){
    int return_code;

    My_node->pending_uncoord[dest_id]--;

    if(My_node->pending_uncoord[dest_id] == 0){
        //all coords have responded

        My_node->state[dest_id] = 0;
        
        if(My_node->dist[dest_id] != INF){
            //we got a route to dest_id, so spreed it
            return_code = Route_neighbors(dest_id, -1, My_node);
            if(return_code != 0) return return_code;
        }//else //we didnt get a route to dest_id there is no way available to dest_id

        if(My_node->succ_coord[dest_id] != -1){
            return_code = Send_UNCOORD(My_node->succ_coord[dest_id], dest_id, My_node);
            if(return_code != 0) return return_code;
            My_node->succ_coord[dest_id] = -1;
        }

    }//else //still whating on all coords to respond

    return 0;
}

int process_CHAT_message(char* Chat_protocol, int dest_id, Node_info* My_node){
    int return_code, origin_id;
    char chat_message[TCP_Chat_protocol_len];

    if(My_node->id == dest_id){
        //the message reached its destination
        if(sscanf(Chat_protocol, "CHAT %d %*d %[^\n]", &origin_id, chat_message) == 2){

            printf("Recived message from %d\n%s\n", origin_id, chat_message);

            return 0;
        }else{
            //unexpected sscanf return
            printf("DEBUG ERROR: (in function process_CHAT_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
            return 6;
        }
    }

    if(My_node->state[dest_id] == 1 || My_node->succ[dest_id] == -1){
        printf("Cannot forward chat_message: Destination %d is unreachable or coordinating.\n", dest_id);
        return 0; // lose message
    }

    return_code = Send_chat_protocol_to_id(Chat_protocol, My_node->succ[dest_id], My_node);
    if(return_code != 0) return return_code;

    return 0;
}

int process_NEIGHBOR_message(char* Routing_protocol, int newfd, Node_info* My_node){
    int neigbor_id, return_code;

    if(sscanf(Routing_protocol, "%*s %d", &neigbor_id) != 1){
        return 6;
    }

    My_node->TCP_fd[neigbor_id] = newfd;

    My_node->number_of_TCP_channels++;

    return_code = Send_ROUTE(neigbor_id, My_node->id, My_node);
    if(return_code != 0) return return_code;

    for(int i = 0; i < Number_of_ids; i++){
        if(My_node->succ[i] != -1 && My_node->state[i] == 0){
            return_code = Send_ROUTE(neigbor_id, i, My_node);
            if(return_code != 0) return return_code;
        }
    }

    return 0;

}