#ifndef SER_CLI_COM_H
#define SER_CLI_COM_H


#include "struct_and_constants.h"
#include "routing.h"
#include "cmd_aux.h"
#include "cmd.h"


/********************************************************************************* -----TCP----- *********************************************************************************/

/***********************************************************************************************
 * @brief Initializes a TCP server socket and starts listening for incoming connections.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Create_TCP_Server(Node_info* My_node);

/***********************************************************************************************
 * @brief Accepts an incoming TCP connection request and stores the descriptor in the pending list.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int accept_TCP_connection(Node_info* My_node);

/***********************************************************************************************
 * @brief Creates a TCP client socket, connects to a remote node, and sends the initial NEIGHBOR and ROUTE messages.
 * @param dest_IP The IPv4 address of the target node.
 * @param dest_Port The TCP port of the target node.
 * @param dest_id The ID of the target node.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Create_and_Connect_TCP_client(char* dest_IP, char* dest_Port, int dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Closes a TCP connection with a specific neighbor and updates the node state.
 * @param dest_id The ID of the neighbor to disconnect.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Close_TCP_Client(int dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Closes the main TCP listening server socket.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Close_TCP_Server(Node_info* My_node);

/**
 * @brief Writes a routing protocol message (ROUTE, COORD, UNCOORD) to a specific neighbor's socket.
 * @param routing_protocol The raw protocol string to be sent.
 * @param dest_id The ID of the neighbor receiving the message.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Send_routing_protocol_to_id(char* routing_protocol, int dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Writes a CHAT protocol message to a specific neighbor's socket.
 * @param chat_protocol The raw chat message string.
 * @param dest_id The ID of the neighbor (successor) receiving the message.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Send_chat_protocol_to_id(char* chat_protocol, int dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Reads a message from a specific file descriptor and null-terminates the string.
 * * Handles detection of closed connection.
 * @param message Buffer to store the received string.
 * @param sender_fd The fd to read from.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, 7 if the connection was closed by the peer, others are errors.
 ***********************************************************************************************/
int Recive_message_from_fd(char* message, int sender_fd, Node_info* My_node);

/********************************************************************************* -----UDP----- *********************************************************************************/

/***********************************************************************************************
 * @brief Sends a request to the central UDP registry server and waits for a response.
 * * Creates a socket, sends the message, and uses select_timeout to wait for data.
 * @param message The raw UDP request (REG, NODES, CONTACT).
 * @param response Buffer to store the server's response.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int send_message_to_UDP_server(char* message, char* response, Node_info* My_node);

/***********************************************************************************************
 * @brief Monitors a UDP socket for a specific period to prevent the program from blocking indefinitely.
 * * Implements a 5-second timeout using the select() system call.
 * @param fd The UDP socket file descriptor to monitor.
 * @return (int) 0 on success, 2 if a timeout occurs, others are errors.
 ***********************************************************************************************/
int select_timeout(int fd);




#endif