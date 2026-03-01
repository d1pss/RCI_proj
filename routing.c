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

int Coord_neighbors(int dest_id, Node_info* My_node){
    int return_code;
    for(int i = 0, n_con = 0; i < Number_of_ids; i++){
        if(My_node->TCP_fd[i] != -1){
            if((i =! atoi(My_node->id))){
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

int process_routing_message(char* input, Node_info* MY_node){
    char Protocol[TCP_Routing_protocol_len];

    sscanf(input, "%s", Protocol)
}

int process_ROUTE_message(){

}

int process_COORD_message(){

}