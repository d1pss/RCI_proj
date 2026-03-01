#ifndef CMD_H
#define CMD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "struct_and_constants.h"
#include "cmd_aux.h"
#include "server_client_comunication.h"


int process_command(char *input, Node_info* My_node);

int cmd_join(char* net, char* id, Node_info* My_node);

int cmd_add_edge(char* dest_id, Node_info* My_node);

int cmd_show_nodes(char* net, Node_info* My_node);

void cmd_leave(Node_info* My_node);

void cmd_remove_edge(char* id_to_remove, Node_info* My_node);



#endif