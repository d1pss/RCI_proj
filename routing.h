#ifndef ROUTING_H
#define ROUTING_H

#include "struct_and_constants.h"
#include "server_client_comunication.h"

int Send_NEIGHBOR(int neighbor_id_to_send, Node_info* My_node);

int Send_COORD(int neighbor_id_to_send, int dest_id, Node_info* My_node);

int Send_UNCOORD(int neighbor_id_to_send, int dest_id, Node_info* My_node);

int Send_ROUTE(int neighbor_id_to_send, int dest_id, Node_info* My_node);

int Send_CHAT(int succ_id, int dest_id, char* chat_message, Node_info* My_node);

//the except id can go as -1 as to not exept any id
int Coord_neighbors(int dest_id, int except_id, Node_info* My_node);

//the except id can go as -1 as to not exept any id
int Route_neighbors(int dest_id, int except_id, Node_info* My_node);

int process_TCP_message(char* input, int neigbor_id, Node_info* MY_node);

int process_ROUTE_message(int dest_id, int dist_to_dest_id_from_neighbor, int neighbor_id, Node_info* My_node);

int process_COORD_message(int dest_id, int neighbor_id, Node_info* My_node);

int process_UNCOORD_message(int dest_id, int neighbor_id, Node_info* My_node);

int process_CHAT_message(char* Chat_protocol, int dest_id, Node_info* My_node);

int process_NEIGHBOR_message(char* Routing_protocol, int newfd, Node_info* My_node);

#endif 
