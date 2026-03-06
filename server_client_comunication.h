#ifndef SER_CLI_COM_H
#define SER_CLI_COM_H


#include "struct_and_constants.h"
#include "routing.h"
#include "cmd_aux.h"
#include "cmd.h"


/********************************************************************************* -----TCP----- *********************************************************************************/

int Create_TCP_Server(Node_info* My_node);

int accept_TCP_connection(Node_info* My_node);

int Create_and_Connect_TCP_client(char* dest_IP, char* dest_Port, int dest_id, Node_info* My_node);

int Close_TCP_Client(int dest_id, Node_info* My_node);

int Close_TCP_Server(Node_info* My_node);

int Send_routing_protocol_to_id(char* routing_protocol, int dest_id, Node_info* My_node);

int Send_chat_protocol_to_id(char* chat_protocol, int dest_id, Node_info* My_node);

int Recive_message_from_fd(char* message, int sender_index, int sender_id, int sender_fd, Node_info* My_node);

/********************************************************************************* -----UDP----- *********************************************************************************/

int send_message_to_UDP_server(char* message, char* response, Node_info* My_node);

// return 7 if timeout
int select_timeout(int fd);




#endif