#ifndef CMD_H
#define CMD_H

#include "struct_and_constants.h"
#include "cmd_aux.h"
#include "server_client_comunication.h"


int process_command(char *input, Node_info* My_node);

int cmd_monotoring(bool start, Node_info* My_node);

int cmd_message(char* dest_id_as_char, char* chat_message, Node_info* My_node);

int cmd_announce(Node_info* My_node);

int cmd_show_routing(char* dest_id_as_char, Node_info* My_node);

int cmd_join(char* net, char* id, Node_info* My_node);

int cmd_add_edge(char* dest_id, Node_info* My_node);

int cmd_show_nodes(char* net, Node_info* My_node);

int cmd_leave(Node_info* My_node);

int cmd_remove_edge(char* id_to_remove, Node_info* My_node);

int cmd_direct_join(char* net_as_char, char* id_as_char, Node_info* My_node);

int cmd_direct_add_edge(char* dest_id_as_char, char* idIP, char* idTCP, Node_info* My_node);

#endif