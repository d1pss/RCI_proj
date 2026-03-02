#ifndef CMD_AUX_H
#define CMD_AUX_H

#include "struct_and_constants.h"
#include "server_client_comunication.h"

int print_ids(char* response, int net);

int add_id_to_net(int net, int id, Node_info* My_node);

int get_id_info(char* id_IP ,char* id_Port ,bool* get_id_info ,int net, int id, Node_info* My_node);

int max(int a, int b);

void print_help(void);

int Check_argv_format(char** argv, int argc);

bool is_IP_invalid(char* IP);

bool is_Port_invalid(char* Port);

Node_info* init_Node(char** argv, int argc);

bool is_string_a_number(char* string);

char* int_id_to_string(int id);

char* int_net_to_string(int net);


#endif