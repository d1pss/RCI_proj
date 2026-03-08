#ifndef CMD_H
#define CMD_H

#include "struct_and_constants.h"
#include "cmd_aux.h"
#include "server_client_comunication.h"

/***********************************************************************************************
 * @brief Parses the user input string and routes it to the appropriate command handler.
 * @param input Raw string entered by the user in the terminal.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, 1 to exit program, 3 for incorrect input, others for errors.
 ***********************************************************************************************/
int process_command(char *input, Node_info* My_node);

/***********************************************************************************************
 * @brief Enables or disables the monitoring of protocol message exchanges.
 * @param start Boolean flag: true to start monitoring, false to end it.
 * @param My_node Pointer to the global node information structure.
 * @return (int) Always 0.
 ***********************************************************************************************/
int cmd_monotoring(bool start, Node_info* My_node);

/***********************************************************************************************
 * @brief Prepares and sends a CHAT message to a specific destination node via the successor.
 * @param dest_id_as_char String containing the destination ID.
 * @param chat_message String containing the message text to be delivered.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success,3 for invalid input.
 ***********************************************************************************************/
int cmd_message(char* dest_id_as_char, char* chat_message, Node_info* My_node);

/***********************************************************************************************
 * @brief Announces the node's existence to the network by broadcasting a distance 0 route.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int cmd_announce(Node_info* My_node);

/***********************************************************************************************
 * @brief Displays the current routing table entry (state, successor, distance) for a destination.
 * @param dest_id_as_char String containing the destination ID to look up.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, 3 for invalid input.
 ***********************************************************************************************/
int cmd_show_routing(char* dest_id_as_char, Node_info* My_node);

/***********************************************************************************************
 * @brief Handles the standard registration in a network via the central UDP server.
 * @param net_as_char String containing the network.
 * @param id_as_char String containing the requested node ID.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, 3 for invalid input, others for errors.
 ***********************************************************************************************/
int cmd_join(char* net_as_char, char* id_as_char, Node_info* My_node);

/***********************************************************************************************
 * @brief Looks up a node's contact info via UDP and establishes a new direct TCP edge.
 * @param dest_id String containing the ID of the node to connect to.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, 3 for invalid input, others for errors.
 ***********************************************************************************************/
int cmd_add_edge(char* dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Queries the central UDP server for a list of all nodes registered in a specific network.
 * @param net_as_char String containing the network ID to query.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, 3 for invalid input, others for errors.
 ***********************************************************************************************/
int cmd_show_nodes(char* net_as_char, Node_info* My_node);

/***********************************************************************************************
 * @brief Gracefully leaves the network, closing all TCP edges and unregistering from UDP server.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others for errors.
 ***********************************************************************************************/
int cmd_leave(Node_info* My_node);

/***********************************************************************************************
 * @brief Closes a specific TCP edge and triggers the coordination protocol for lost routes.
 * @param id_to_remove_as_char String containing the neighbor ID to disconnect.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, 3 for invalid input.
 ***********************************************************************************************/
int cmd_remove_edge(char* id_to_remove_as_char, Node_info* My_node);

/***********************************************************************************************
 * @brief Joins a network locally without registering the node in the central UDP server.
 * @param net_as_char String containing the network.
 * @param id_as_char String containing the node ID.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, 3 for invalid input, others for errors.
 ***********************************************************************************************/
int cmd_direct_join(char* net_as_char, char* id_as_char, Node_info* My_node);

/***********************************************************************************************
 * @brief Manually establishes a TCP edge using direct IP and Port info instead of a UDP lookup.
 * @param dest_id_as_char String containing the ID of the target node.
 * @param idIP The IPv4 address of the target node.
 * @param idTCP The TCP port of the target node.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, 3 for invalid input, others for errors.
 ***********************************************************************************************/
int cmd_direct_add_edge(char* dest_id_as_char, char* idIP, char* idTCP, Node_info* My_node);

#endif