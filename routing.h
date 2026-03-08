#ifndef ROUTING_H
#define ROUTING_H

#include "struct_and_constants.h"
#include "server_client_comunication.h"

/***********************************************************************************************
 * @brief Sends a NEIGHBOR identification message to a specific node.
 * * @param neighbor_id_to_send The neighbor node that will receive the message.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Send_NEIGHBOR(int neighbor_id_to_send, Node_info* My_node);

/***********************************************************************************************
 * @brief Sends a COORD message to a neighbor to initiate path coordination.
 * * @param neighbor_id_to_send The neighbor node that will receive the message.
 * @param dest_id The destination ID that entered the coordination state.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Send_COORD(int neighbor_id_to_send, int dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Sends an UNCOORD message to signal the end of a coordination phase.
 * * @param neighbor_id_to_send The neighbor node that will receive the message.
 * @param dest_id The destination ID leaving the coordination state.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Send_UNCOORD(int neighbor_id_to_send, int dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Sends a ROUTE update message containing the distance to a destination.
 * * @param neighbor_id_to_send The neighbor node that will receive the message.
 * @param dest_id The destination node being advertised.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Send_ROUTE(int neighbor_id_to_send, int dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Forwards or sends a CHAT message to the successor of a destination.
 * * @param succ_id The ID of the next-hop neighbor (successor).
 * @param dest_id The ultimate destination ID for the message.
 * @param chat_message The actual text string to be sent.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Send_CHAT(int succ_id, int dest_id, char* chat_message, Node_info* My_node);

/***********************************************************************************************
 * @brief Broadcasts COORD messages for a specific destination to all active neighbors.
 * * @param dest_id The destination ID that requires coordination.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Coord_neighbors(int dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Broadcasts ROUTE updates for a specific destination to all active neighbors.
 * * @param dest_id The destination ID to be advertised.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int Route_neighbors(int dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Process an incoming TCP input fragment and routes it to the specific protocol handler.
 * * @param input The string message received from the fragmented input.
 * @param neigbor_id The ID of the neighbor who sent the message.
 * @param MY_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int process_TCP_message(char* input, int neigbor_id, Node_info* MY_node);

/***********************************************************************************************
 * @brief Handles incoming ROUTE updates and recalculates the path.
 * * @param dest_id The destination mentioned in the message.
 * @param dist_to_dest_id_from_neighbor The distance mentioned in the message.
 * @param neighbor_id The ID of the neighbor providing the route.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int process_ROUTE_message(int dest_id, int dist_to_dest_id_from_neighbor, int neighbor_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Responds to a COORD request by entering coordination or providing an alternative.
 * * @param dest_id The destination requiring coordination.
 * @param neighbor_id The ID of the neighbor requesting the coordination.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int process_COORD_message(int dest_id, int neighbor_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Processes an UNCOORD message and resumes expedition state if all responses arrived.
 * * @param dest_id The destination node for which coordination has ended.
 * @param neighbor_id The neighbor confirming the end of coordination.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int process_UNCOORD_message(int dest_id, int neighbor_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Displays a received chat message or forwards it if this is not the destination.
 * * @param Chat_protocol The full raw CHAT message string.
 * @param dest_id The intended final destination ID.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int process_CHAT_message(char* Chat_protocol, int dest_id, Node_info* My_node);

/***********************************************************************************************
 * @brief Handles the initial NEIGHBOR exchange to identify a new connection.
 * * @param Routing_protocol The raw NEIGHBOR message string.
 * @param newfd The file descriptor of the new TCP connection.
 * @param sender_id Pointer to store the identified ID of the neighbor.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int process_NEIGHBOR_message(char* Routing_protocol, int newfd, int *sender_id, Node_info* My_node);

#endif 
