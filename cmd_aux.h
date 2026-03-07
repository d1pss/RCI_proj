#ifndef CMD_AUX_H
#define CMD_AUX_H

#include "struct_and_constants.h"
#include "server_client_comunication.h"

int print_ids(char* response, int net);

int add_id_to_net(char* net, char* id, Node_info* My_node);

int get_id_info(char* id_IP ,char* id_Port ,bool* get_id_info ,char* net, char* id, Node_info* My_node);

int max(int a, int b);

void print_help(void);

int Check_argv_format(char** argv, int argc);

bool is_IP_invalid(char* IP);

bool is_Port_invalid(char* Port);

Node_info* init_Node(char** argv, int argc);

void reset_My_node(Node_info* My_node);

bool is_string_a_number(char* string);

int manage_return_code(int return_code, Node_info* My_node);

void remove_pending_fd(int index_to_remove, Node_info* My_node);

int get_unique_tid(Node_info* My_node);

int fragment_buffer(char* buffer, char*** frag_buffer, int* n_frags);

void free_frag_buffer(char** buffer, int n_frags);

#endif