#ifndef STRUCT_AND_CONSTANTS_H
#define STRUCT_AND_CONSTANTS_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <string.h>
#include <sys/select.h>

/* --- Connectivity Defaults (UDP Server) --- */
#define DEFAULT_UDP_IP "193.136.138.142" // Default UDP server IPv4 address
#define DEFAULT_UDP_PORT "59000"         // Default UDP server Port

/* --- Network and ID Constraints --- */
#define Number_of_ids 100    // Total possible IDs (00 to 99)
#define INF 256              // Infinity value (max possible distance is 99)

/* --- String and Buffer Lengths --- */
#define IP_len 16            // IPv4 string length (xxx.xxx.xxx.xxx\0)
#define Port_len 6           // Port string length (xxxxx\0)
#define Id_len 3             // Node ID string length (xx\0)
#define Net_len 4            // Network ID string length (xxx\0)
#define cmd_len 140          // Maximum terminal command length
#define cmd_arguments 3      // Max arguments for commands (e.g., dae id IP Port)

/* --- Protocol Message Limits --- */
#define UDP_message_len 40           // Max length for outgoing UDP messages (e.g., REG)
#define UDP_response_size 320        // Max length for incoming UDP responses (e.g., NODES)
#define TCP_Routing_protocol_len 24  // Max length for ROUTE/COORD/UNCOORD messages
#define TCP_Chat_protocol_len 140    // Max length for CHAT messages
#define TCP_buffer_len 2048          // Buffer for multiple TCP messages in one read

/* --- Internal Node States and Symbols --- */
#define UNUSED_FD -1         // Symbolizes that the socket has not been allocated or is closed
#define NO_SUCCESSOR -1      // Indicates there is no next-hop neighbor for a destination
#define STATE_EXPEDITION 0   // Route is stable and the node can forward messages
#define STATE_COORDINATION 1 // Route is lost and the node is looking for an alternative

/**********************************************************************************************/
//PROGRAM RETURN CODES REFERENCE                                                     
#define SUCCESS 0         //- Function completed as expected.
#define EXIT_OK 1         //- User initiated exit (x) or fatal error cleanup completed.
#define ERR_NET_LOGIC 2   //- UDP Timeouts or server op code unknown.
#define ERR_INPUT 3      //- Incorrect command syntax or out-of-range arguments.
#define ERR_MEMORY 4      //- Critical failure in memory allocation (malloc).
#define ERR_SOCKET_IO 5   //- Critical I/O failure (socket, bind, connect, listen, read, write).
#define ERR_UNEXPECTED 6  //- Internal logic violation (e.g., sscanf mismatch).
#define STATUS_SPECIFIC 7   //- Function-dependent status.
//Note: Errors 1, 4, 5, and 6 will trigger an automatic 'leave' before exiting.
/**********************************************************************************************/

/***********************************************************************************************
 * @brief Central structure to manage the node's global state and network context.
 * * This structure acts as the main control for the node, storing TCP/UDP network 
 * configurations, active/pending fds, and the OWR routing table. 
 ***********************************************************************************************/
typedef struct _Node_information{
    // TCP info for node-to-node connection
    char Node_TCP_IP[IP_len];        /* Node's own IPv4 address for TCP server. */
    char Node_TCP_Port[Port_len];    /* Port where the node listens for TCP connections. */
    int TCP_fd[Number_of_ids];       /* Array of active TCP sockets indexed by neighbor ID. */
    int number_of_TCP_channels;      /* Current count of active direct neighbors (edges). */
    int TCP_pending_fd[Number_of_ids]; /* Temporary storage for accepted sockets awaiting NEIGHBOR ID. */
    int number_pending_fd;           /* Current count of sockets in the pending identification state. */

    // UDP info for node-to-network connection
    char UDP_Server_IP[IP_len];      /* IP address of the central node registry server. */
    char UDP_Server_Port[Port_len];  /* Port of the central node registry server. */
    int unique_tid;                 /* Internal counter to generate unique UDP Transaction IDs. */

    // Node info
    int id;                          /* Unique identifier (00-99) of this node in the network. */
    int net;                         /* Network identifier (000-999) the node belongs to. */
    int dist[Number_of_ids];         /* Distance vector storing shortest path to all known nodes. */
    int succ[Number_of_ids];         /* Successor (next-hop) ID for each destination. */
    bool state[Number_of_ids];       /* 0 for EXPEDITION state, 1 for COORDINATION state. */
    int succ_coord[Number_of_ids];   /* Successor that triggered a COORDINATION message. */
    int pending_uncoord[Number_of_ids]; /* Counter of neighbors yet to respond with UNCOORD. */

    // Flags
    bool is_in_net;                  /* Status flag indicating if the node is registered/active. */
    bool is_monitoring;              /* Flag to enable/disable protocol message logging. */
    bool was_direct_added;           /* Flag to distinguish between 'join' and 'direct join'. */
    bool debug;                      /* Toggle for basic debug messages. */
    bool adv_debug;                  /* Toggle for advanced routing and state debug logs. */
} Node_info;


#endif