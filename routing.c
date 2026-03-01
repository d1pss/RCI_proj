#include "routing.h"

int Send_COORD(int neighbor_id_to_send, int dest_id, Node_info* My_node){
    char Routing_cmd[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_cmd, "COORD %d\n", dest_id);

    return_code = Send_message_to_id(Routing_cmd, neighbor_id_to_send, My_node);

    return return_code;
}

int Send_UNCOORD(int neighbor_id_to_send, int dest_id, Node_info* My_node){
    char Routing_cmd[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_cmd, "UNCOORD %d\n", dest_id);

    return_code = Send_message_to_id(Routing_cmd, neighbor_id_to_send, My_node);

    return return_code;
}

int Send_ROUTE(int neighbor_id_to_send, int dest_id, Node_info* My_node){
    char Routing_cmd[TCP_Routing_protocol_len];
    int return_code;

    sprintf(Routing_cmd, "ROUTE %d %d\n", dest_id, My_node->dist[dest_id]);

    return_code = Send_message_to_id(Routing_cmd, neighbor_id_to_send, My_node);

    return return_code;
}

int Coord_neighbors(int dest_id, int exept_id, Node_info* My_node){
    int return_code;
    for(int i = 0, n_con = 0; i < Number_of_ids; i++){
        if(My_node->TCP_fd[i] != -1){
            if((i =! atoi(My_node->id)) && (i != exept_id)){
                n_con++;
                return_code = Send_COORD(i, dest_id, My_node);
                if(return_code != 0){
                    return return_code;
                }
            }
            if(n_con == My_node->number_of_TCP_chanels) break;
        }
    }
    return 0;
}

int Route_neighbors(int dest_id, int exept_id, Node_info* My_node){
    int return_code;
    for(int i = 0, n_con = 0; i < Number_of_ids; i++){
        if(My_node->TCP_fd[i] != -1){
            if((i =! atoi(My_node->id)) && (i != exept_id)){
                n_con++;
                return_code = Send_ROUTE(i, dest_id, My_node);
                if(return_code != 0){
                    return return_code;
                }
            }
            if(n_con == My_node->number_of_TCP_chanels) break;
        }
    }
    return 0;
}

int process_TCP_message(char* input, int neigbor_id, Node_info* MY_node){
    char Protocol[TCP_Routing_protocol_len], chat_message[TCP_Chat_protocol_len];
    int dist_to_dest_id_from_neighbor, num_args, origin_id, dest_id;

    num_args = sscanf(input, "%s", Protocol);

    if(num_args == 1){

        if(!strcmp(Protocol, "ROUTE")){

            if(sscanf(input, "%*s %d %d", dest_id, dist_to_dest_id_from_neighbor) == 2){



                

            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return 6;
            }

        }else if(!strcmp(Protocol, "COORD")){

            if(sscanf(input, "%*s %d", dest_id) == 1){





            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return 6;
            }

        }else if(!strcmp(Protocol, "UNCOORD")){

             if(sscanf(input, "%*s %d", dest_id) == 1){





            }else{
                //unexpected sscanf return
                printf("DEBUG ERROR: (in function process_routing_message) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
                return 6;
            }

        }else if(!strcmp(Protocol, "CHAT")){

             if(sscanf(input, "%*s %d %d %s", origin_id, dest_id, chat_message) == 3){





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
}

int process_ROUTE_message(int dest_id, int dist_to_dest_id_from_neighbor, int neighbor_id, Node_info* My_node){
    if(dest_id == My_node->id){
        return 0;
    }

    int new_dist_to_dest_id = dist_to_dest_id_from_neighbor + 1, return_code;

    if(new_dist_to_dest_id < My_node->dist[dest_id]){
        //better way found
        My_node->dist[dest_id] = new_dist_to_dest_id;
        My_node->succ[dest_id] = neighbor_id;

        if((My_node->state[dest_id] == 0)){
            //send new route to neigbors exept neighbor_id
            return_code = Route_neighbors(dest_id, neighbor_id, My_node);
            if(return_code != 0){
                return return_code;
            }

        }//else //need to wait before sending this new route
    }//else //way given is not better that the one we already have
        
    return 0;
}

int process_COORD_message(int dest_id, int neighbor_id, Node_info* My_node){
    
}

int process_UNCOORD_message(){

}

int process_CHAT_message(){

}