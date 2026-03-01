#ifndef STRUCT_AND_CONSTANTS_H
#define STRUCT_AND_CONSTANTS_H

#include <stdbool.h>

#define DEFAULT_UDP_PORT "59000" //Default UDP server Port
#define DEFAULT_UDP_IP "193.136.138.142" //Default UDP server IPv4

#define UDP_response_size 320 //100 id x 3 for the 2 digits and the \n + 20 for the first line in the worse case
#define UDP_message_len 40 //in the worse case using REG we can have 38 chars so 40 is a safe lenght
#define TCP_Routing_protocol_len 12 //in the worse case using (NEIGHBOR id\n) we hase 12 chars
#define TCP_Chat_protocol_len 140  // in the worse case using (CHAT origin dest chat\n) we can have 139 chars
#define cmd_len 32 //in the worse case using (dae id idIP idTCP) we can have 30 chars so 32 is a safe lenght
#define cmd_arguments 3 //in the worse case using (dae id idIP idTCP) we have 3 arguments
#define IP_len 16 // in the worse case we have xxx.xxx.xxx.xxx\0 that is 16 chars
#define Port_len 6 // in the worse case we have xxxxx\0 that is 6 chars
#define Number_of_ids 100 // we have ids between 00 and 99, so 100 in total
#define Id_len 3 // ids are two digit numbers so xx\0 that is 3 chars
#define Net_len 4 // nets are three digit numbers so xxx\0 that is 4 chars

typedef struct _Node_information{
    //TCP info for node to node connection
    char Node_TCP_IP[IP_len];
    char Node_TCP_Port[Port_len];
    int TCP_fd[Number_of_ids];
    int number_of_TCP_chanels;

    //UDP info for node to network connection
    char UDP_Server_IP[IP_len];
    char UDP_Server_Port[Port_len];

    //Node info
    char id[Id_len];
    char net[Net_len];
    int dist[Number_of_ids];
    int succ[Number_of_ids];
    bool state[Number_of_ids];

    //flags
    bool is_in_net;
    bool is_monitoring;
}Node_info;


#endif