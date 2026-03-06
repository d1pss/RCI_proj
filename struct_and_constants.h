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

#define DEFAULT_UDP_PORT "59000" //Default UDP server Port
#define DEFAULT_UDP_IP "193.136.138.142" //Default UDP server IPv4

#define UDP_response_size 320 //100 id x 3 for the 2 digits and the \n + 20 for the first line in the worse case
#define UDP_message_len 40 //in the worse case using REG we can have 38 chars so 40 is a safe lenght
#define TCP_Routing_protocol_len 24 //in the worse case using (ROUTE dest n\n\0) we have 14 chars
#define TCP_Chat_protocol_len 140  // in the worse case using (CHAT origin dest chat\n) we can have 139 chars
#define TCP_buffer_len 2048 // in case we recieve various messages from one id in the same read we need a big buffer to store it all
#define cmd_len 32 //in the worse case using (dae id idIP idTCP) we can have 30 chars so 32 is a safe lenght
#define cmd_arguments 3 //in the worse case using (dae id idIP idTCP) we have 3 arguments
#define IP_len 16 // in the worse case we have xxx.xxx.xxx.xxx\0 that is 16 chars
#define Port_len 6 // in the worse case we have xxxxx\0 that is 6 chars
#define Number_of_ids 100 // we have ids between 00 and 99, so 100 in total
#define Id_len 3 // ids are two digit numbers so xx\0 that is 3 chars
#define Net_len 4 // nets are three digit numbers so xxx\0 that is 4 chars
#define INF 256 // 99 is the max dist possible so 256 can act as infinity


typedef struct _Node_information{
    //TCP info for node to node connection
    char Node_TCP_IP[IP_len];
    char Node_TCP_Port[Port_len];
    int TCP_fd[Number_of_ids];
    int number_of_TCP_channels;
    int TCP_pending_fd[Number_of_ids];
    int number_pending_fd;

    //UDP info for node to network connection
    char UDP_Server_IP[IP_len];
    char UDP_Server_Port[Port_len];

    //Node info
    int id;
    int net;
    int dist[Number_of_ids];
    int succ[Number_of_ids];
    bool state[Number_of_ids];
    int succ_coord[Number_of_ids];
    int pending_uncoord[Number_of_ids];
    int unique_tid;

    //flags
    bool is_in_net;
    bool is_monitoring;
    bool debug;
    bool adv_debug;
}Node_info;


#endif