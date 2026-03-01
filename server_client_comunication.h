#ifndef SER_CLI_COM_H
#define SER_CLI_COM_H

#define _POSIX_C_SOURCE 200112L
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <string.h>

#include "struct_and_constants.h"


/********************************************************************************* -----TCP----- *********************************************************************************/

int Create_TCP_Server(Node_info* My_node);

int accept_TCP_connection(Node_info* My_node);

int Create_and_Connect_TCP_client(char* dest_IP, char* dest_Port, char* dest_id, Node_info* My_node);

int Close_TCP_Client(char* dest_id, Node_info* My_node);

int Close_TCP_Server(Node_info* My_node);

int Send_routing_protocol_to_id(char* routing_protocol, int dest_id, Node_info* My_node);

int Send_chat_protocol_to_id(char* chat_protocol, int dest_id, Node_info* My_node);

int Recive_message_from_id(char* message, ssize_t Max_len, int dest_id, Node_info* My_node);

int Recive_routing_protocol_from_id(char* routing_protocol, int dest_id, Node_info* My_node);

int Recive_chat_protocol_from_id(char* chat_protocol, int dest_id, Node_info* My_node);


/********************************************************************************* -----UDP----- *********************************************************************************/

int send_message_to_UDP_server(char* message, char* response, Node_info* My_node);




#endif