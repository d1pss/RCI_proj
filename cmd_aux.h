#ifndef CMD_AUX_H
#define CMD_AUX_H

#include "struct_and_constants.h"
#include "server_client_comunication.h"

/********************************************************************************* -----cmd aux funcs----- *********************************************************************************/

/***********************************************************************************************
 * @brief Parses the UDP server response and prints the list of IDs registered in a network.
 * @param response The raw string response from the NODES command.
 * @param net The network identifier associated with the list.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int print_ids(char* response, int net);

/***********************************************************************************************
 * @brief Sends a REG message to the UDP server to register a node ID in a specific network.
 * @param net String containing the network ID.
 * @param id String containing the node ID to register.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, 7 if the network is full, others are errors.
 ***********************************************************************************************/
int add_id_to_net(char* net, char* id, Node_info* My_node);

/***********************************************************************************************
 * @brief Retrieves contact information (IP/Port) for a specific node from the UDP server.
 * @param id_IP Buffer to store the retrieved IPv4 address.
 * @param id_Port Buffer to store the retrieved TCP port.
 * @param get_id_info Boolean pointer set to true if the ID exists, false otherwise.
 * @param net String containing the network ID.
 * @param id String containing the node ID to look up.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int get_id_info(char* id_IP, char* id_Port, bool* get_id_info, char* net, char* id, Node_info* My_node);

/***********************************************************************************************
 * @brief Prints the available terminal commands and their descriptions to the user.
 * @return (int) Always 0.
 ***********************************************************************************************/
int print_help(void);

/***********************************************************************************************
 * @brief Generates a unique Transaction ID (TID) for UDP server communications.
 * * TID values range from 000 to 999 and wrap around.
 * @param My_node Pointer to the global node information structure.
 * @return (int) The generated unique transaction ID.
 ***********************************************************************************************/
int get_unique_tid(Node_info* My_node);


/********************************************************************************* -----check that is valid funcs----- *********************************************************************************/


/***********************************************************************************************
 * @brief Checks if a string represents a valid IPv4 address (0.0.0.0 to 255.255.255.255).
 * @param IP String containing the IP address to validate.
 * @return (bool) true if the IP is invalid, false if it is a valid IPv4 address.
 ***********************************************************************************************/
bool is_IP_invalid(char* IP);

/***********************************************************************************************
 * @brief Checks if a string represents a valid TCP/UDP port (1 to 65535).
 * @param Port String containing the port to validate.
 * @return (bool) true if the port is invalid, false if it is within a valid range.
 ***********************************************************************************************/
bool is_Port_invalid(char* Port);

/***********************************************************************************************
 * @brief Checks if a given string consists entirely of numeric characters.
 * @param string The string to be evaluated.
 * @return (bool) true if the string is a number, false otherwise.
 ***********************************************************************************************/
bool is_string_a_number(char* string);


/********************************************************************************* -----Main aux funcs----- *********************************************************************************/


/***********************************************************************************************
 * @brief Returns the maximum of two integers.
 * @param a First integer.
 * @param b Second integer.
 * @return (int) The greater of the two values.
 ***********************************************************************************************/
int max(int a, int b);

/***********************************************************************************************
 * @brief Validates the format and values of the command-line arguments (IPs and Ports).
 * @param argv Array of command-line argument strings.
 * @param argc Number of command-line arguments.
 * @return (int) 0 if valid, 1 if any argument is malformed or out of range.
 ***********************************************************************************************/
int Check_argv_format(char** argv, int argc);

/***********************************************************************************************
 * @brief Allocates memory for the Node_info structure and initializes it with boot arguments.
 * @param argv Validated command-line arguments.
 * @param argc Number of arguments.
 * @return (Node_info*) Pointer to the allocated structure, or NULL on memory failure.
 ***********************************************************************************************/
Node_info* init_Node(char** argv, int argc);

/***********************************************************************************************
 * @brief Resets the node's internal state (routing tables, flags, and file descriptors).
 * @param My_node Pointer to the global node information structure.
 ***********************************************************************************************/
void reset_My_node(Node_info* My_node);

/***********************************************************************************************
 * @brief Evaluates function returns and decides if the program should continue or exit.
 * @param return_code The code returned by a previously executed function.
 * @param My_node Pointer to the global node information structure.
 * @return (int) 0 to continue execution, 1 to initiate program shutdown.
 ***********************************************************************************************/
int manage_return_code(int return_code, Node_info* My_node);

/***********************************************************************************************
 * @brief Removes a file descriptor from the pending list and shifts remaining elements.
 * @param index_to_remove The index in the TCP_pending_fd array to be cleared.
 * @param My_node Pointer to the global node information structure.
 ***********************************************************************************************/
void remove_pending_fd(int index_to_remove, Node_info* My_node);

/***********************************************************************************************
 * @brief Splits a buffer containing multiple messages (separated by \\n) into a string array.
 * @param buffer The raw input string containing multiple protocol messages.
 * @param frag_buffer Pointer to the array of strings that will store the fragments.
 * @param n_frags Pointer to an integer to store the total number of fragments found.
 * @return (int) 0 on success, others are errors.
 ***********************************************************************************************/
int fragment_buffer(char* buffer, char*** frag_buffer, int* n_frags);

/***********************************************************************************************
 * @brief Frees the memory allocated for the fragmented buffer array.
 * @param buffer The array of strings to be freed.
 * @param n_frags The number of strings within the array.
 ***********************************************************************************************/
void free_frag_buffer(char** buffer, int n_frags);

#endif