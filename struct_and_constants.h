#ifndef STRUCT_AND_CONSTANTS_H
#define STRUCT_AND_CONSTANTS_H

#include <stdbool.h>

#define DEFAULT_UDP_PORT "59000" //Default UDP server Port
#define DEFAULT_UDP_IP "193.136.138.142" //Default UDP server IPv4

#define Max_response_size 320 //100 id x 3 for the 2 digits and the \n + 20 for the first line in the worse case
#define Max_message_len 40 //in the worse case using REG we can have 38 chars so 40 is a safe lenght
#define Max_cmd_len 32 //in the worse case using (dae id idIP idTCP) we can have 30 chars so 32 is a safe lenght
#define Max_cmd_arguments 3 //in the worse case using (dae id idIP idTCP) we have 3 arguments
#define Max_IP_len 16 // in the worse case we have xxx.xxx.xxx.xxx\0 that is 16 chars
#define Max_Port_len 6 // in the worse case we have xxxxx\0 that is 6 chars
#define Number_of_ids 100 // we have ids between 00 and 99, so 100 in total
#define Id_len 3 // ids are two digit numbers so xx\0 thet is 3 chars

typedef struct _Node_information{
    //TCP info for node to node connection
    char Node_TCP_IP[Max_IP_len];
    char Node_TCP_Port[Max_Port_len];
    int TCP_fd[Number_of_ids];
    int number_of_TCP_chanels;

    //UDP info for node to network connection
    char UDP_Server_IP[Max_IP_len];
    char UDP_Server_Port[Max_Port_len];

    //Node info
    char id[Id_len];
    char net[4];
    int dist[Number_of_ids];
    int succ[Number_of_ids];
    bool state[Number_of_ids];

    //flags
    bool is_in_net;
    bool is_monitoring;
}Node_info;


#endif