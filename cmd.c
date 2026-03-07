#include "cmd.h"


//return 0 return to main without error
//return 1 return to main to exit the program without errors
//return 2 return to main to with network error
//return 3 return to main to with incorrect input
//return 4 return to main to exit the program with memory allocation error
//return 5 return to main to exit the program with seting up UDP or TCP error
//return 6 return to main to exit the program with not suposed to happend case
int process_command(char *input, Node_info* My_node){
    char arguments[cmd_arguments][cmd_len], cmd[cmd_len], chat_message[TCP_Chat_protocol_len];
    int num_args;

    num_args = sscanf(input, "%s", cmd);

    if(!strcmp(cmd, "m")){
        num_args += sscanf(input, "%*s %s %[^\n]", arguments[0], chat_message);
    }else{
        num_args = sscanf(input, "%s %s %s %s", cmd, arguments[0], arguments[1], arguments[2]);
    }
    

    if(num_args == 1){
        if(!strcmp(cmd, "l")){
           
            return cmd_leave(My_node);

        }else if(!strcmp(cmd, "x")){

            //in case we forget to leave before exiting
            if(My_node->is_in_net){
                //the node is in the network, so leave
                //cmd_leave();
            }

            //return to main to exit the program without errors
            return 1;

        }else if(!strcmp(cmd, "sg")){

        }else if(!strcmp(cmd, "a")){

            return cmd_announce(My_node);

        }else if(!strcmp(cmd, "sm")){

            return cmd_monotoring(true, My_node);

        }else if(!strcmp(cmd, "em")){

            return cmd_monotoring(false, My_node);

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input or the number of arguments for that comand is wrong\n", cmd);
            return 3;
        }

    }else if(num_args == 2){

        if(!strcmp(cmd, "n")){

           return cmd_show_nodes(arguments[0], My_node);

        }else if(!strcmp(cmd, "ae")){

            return cmd_add_edge(arguments[0], My_node);

        }else if(!strcmp(cmd, "re")){

            return cmd_remove_edge(arguments[0], My_node);

        }else if(!strcmp(cmd, "sr")){

            return cmd_show_routing(arguments[0], My_node);

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input or the number of arguments for that comand is wrong\n", cmd);
            return 3;
        }

    }else if(num_args == 3){

        if(!strcmp(cmd, "j")){

            return cmd_join(arguments[0], arguments[1], My_node);

        }else if(!strcmp(cmd, "m")){

            return cmd_message(arguments[0], chat_message, My_node);

        }else if(!strcmp(cmd, "dj")){

            return cmd_direct_join(arguments[0], arguments[1], My_node);

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input or the number of arguments for that comand is wrong\n", cmd);
            return 3;
        }
        
    }else if(num_args == 4){

        if(!strcmp(cmd, "dae")){

            return cmd_direct_add_edge(arguments[0], arguments[1], arguments[2], My_node);

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input or the number of arguments for that comand is wrong\n", cmd);
            return 3;
        }

    }else{
        //unexpected sscanf return
        printf("The comand used (%s) is not a valid input or the number of arguments for that comand is wrong\n", cmd);
        return 3;
    }
    return 0;
}

int cmd_monotoring(bool start, Node_info* My_node){
    if(start){
        if(My_node->is_monitoring){
            printf("Already Monotoring\n");
        }else{
            printf("Started Monotoring\n");
            My_node->is_monitoring = true;
        }
    }else{
        if(My_node->is_monitoring){
            printf("Ended Monotoring\n");
            My_node->is_monitoring = false;
        }else{
            printf("Currently not monotoring\n");
        }
    }
    return 0;
}

int cmd_message(char* dest_id_as_char, char* chat_message, Node_info* My_node){
    if(!is_string_a_number(dest_id_as_char) || atoi(dest_id_as_char) > 99 || atoi(dest_id_as_char) < 0){
        //error id value should be between 99 and 00
        printf("ERROR: input argument id should be a number between 00 and 99\n");
        return 3;
    }

    if (!My_node->is_in_net){
        printf("Our node is not in the network, cant message\n");
        return 0;
    } 

    int dest_id = atoi(dest_id_as_char), return_code;

    if(My_node->succ[dest_id] == -1){
        printf("The Node with id(%02d) is not connected to us id(%02d), cant send message\n", dest_id, My_node->id);
        return 0;
    }

    return_code = Send_CHAT(My_node->succ[dest_id], dest_id, chat_message, My_node);

    return return_code;
}

int cmd_announce(Node_info* My_node){
    if (!My_node->is_in_net){
        printf("Our node is not in the network, cant anounce\n");
        return 0;
    } 

    printf("anounced existence to neighbors\n");
    //Define my route
    My_node->dist[My_node->id] = 0;
    My_node->succ[My_node->id] = -1;
    My_node->state[My_node->id] = 0;

    return Route_neighbors(My_node->id, My_node);
}

int cmd_show_routing(char* dest_id_as_char, Node_info* My_node){
    if(!is_string_a_number(dest_id_as_char) || atoi(dest_id_as_char) > 99 || atoi(dest_id_as_char) < 0){
        //error id value should be between 99 and 00
        printf("ERROR: input argument id should be a number between 00 and 99\n");
        return 3;
    }

    if (!My_node->is_in_net){
        printf("The node is not in the network, cant show route\n");
        return 0;
    } 

    int dest_id = atoi(dest_id_as_char);

    if(My_node->succ[dest_id] == -1){
        printf("The dest_id that was given does not have a way to current node\n");
        return 0;
    }

    if(My_node->state[dest_id] == 0){
        printf("The way to dest_id is cordenated and the succ id is %02d and the distance is %d\n", My_node->succ[dest_id], My_node->dist[dest_id]);
    }else{
        printf("The Node is expedition state\n");
    }

    return 0;
}

int cmd_join(char* net_as_char, char* id_as_char, Node_info* My_node){
    int return_code;
    bool does_id_exist;

    if(!is_string_a_number(net_as_char) || atoi(net_as_char) > 999 || atoi(net_as_char) < 0){
        //error net value should be between 999 and 000
        printf("ERROR: input argument net should be a number between 000 and 999\n");
        return 3;
    }

    if(!is_string_a_number(id_as_char) || atoi(id_as_char) > 99 || atoi(id_as_char) < 0){
        //error id value should be between 99 and 00
        printf("ERROR: input argument id should be a number between 00 and 99\n");
        return 3;
    }

    int id = atoi(id_as_char), net = atoi(net_as_char);

    if(My_node->is_in_net){
        //no need to join node is already in th network
        printf("Our node is allready in the network, no need to join again\n");
        return 0;
    }

    return_code = get_id_info(NULL, NULL, &does_id_exist ,net_as_char, id_as_char, My_node);

    if(return_code != 0){
        return return_code;
    }

    if(does_id_exist){
        //error id already exists must use a difrent one
        printf("The id chosen (%d) is in use. Must use a difrent id to add our node to the network %d\n", id, net);
        return 0;
    }

    return_code = add_id_to_net(net_as_char, id_as_char, My_node);

    if(return_code != 0){
        if(return_code == 7){
            return 0;
        }
        return return_code;
    }

    My_node->net = net;
    My_node->id = id;
    My_node->is_in_net = true;

    return_code = Create_TCP_Server(My_node);

    return return_code;

}

int cmd_add_edge(char* dest_id_as_char, Node_info* My_node){
    int return_code;
    char dest_ip[IP_len], dest_Port[Port_len];
    bool is_dest_id_in_net;

    if(!is_string_a_number(dest_id_as_char) || atoi(dest_id_as_char) > 99 || atoi(dest_id_as_char) < 0){
        //error id value should be between 99 and 00
        printf("ERROR: input argument id should be a number between 00 and 99\n");
        return 3;
    }

    int dest_id = atoi(dest_id_as_char);

    if(!My_node->is_in_net){
        printf("Our node is not in the network, cant add edge\n");
        return 0;
    }

    if(My_node->TCP_fd[dest_id] != -1){
        printf("Our node id(%02d) is already connected to id(%02d)\n", My_node->id, dest_id);
        return 0;
    }

    char net_as_char[Net_len];
    sprintf(net_as_char, "%03d", My_node->net);

    return_code = get_id_info(dest_ip, dest_Port, &is_dest_id_in_net, net_as_char, dest_id_as_char, My_node);
    if(return_code != 0) return return_code;

    if(!is_dest_id_in_net){
        printf("The destiny id does not exist in network use show nodes to know the ids in network\n");
        return 0;
    }

    return_code = Create_and_Connect_TCP_client(dest_ip, dest_Port, dest_id, My_node);

    return return_code;
}

int cmd_show_nodes(char* net_as_char, Node_info* My_node){
    char message[UDP_message_len], response[UDP_response_size], op, op_str[2];
    int return_code, tid, tid_read;

    if(!is_string_a_number(net_as_char) || atoi(net_as_char) > 999 || atoi(net_as_char) < 0){
        //error net value should be between 999 and 000
        printf("ERROR: input argument net should be a number between 000 and 999\n");
        return 3;
    }

    int net = atoi(net_as_char);

    tid = get_unique_tid(My_node);

    sprintf(message, "NODES %03d 0 %s\n", tid, net_as_char);

    return_code = send_message_to_UDP_server(message, response, My_node);
    if(return_code != 0) return return_code;

    int items_found = sscanf(response, "%*s %d %s", &tid_read, op_str);

    if(tid != tid_read){
        printf("Recived message with difrent tid lost data\n");
        return 0;
    }

    if(My_node->debug){
        printf("SEND: %sRECIVE: %s\n", message, response);
    }

    op = op_str[0];

    if (items_found == 2) {
        if (op == '1') {
            // message contains the ids in the network
            return_code = print_ids(response, net);

            return return_code;
        }else{
            //error code from network
            printf("ERROR: error code (%c) from network using this comand %s\n", op, message);

            return 2;
        }
    }else{
        printf("DEBUG ERROR: (in function cmd_show_nodes) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        return 6;
    } 
}

int cmd_leave(Node_info* My_node){
    char message[UDP_message_len], response[UDP_response_size], op;
    int i, n_con;
    
    if(!My_node->is_in_net){
        //the node is not in the network, so no need to leave
        printf("Our node is not in the network, so no need to leave\n");
        
        return 0;
    }


   //loop para fechar ligação tcp com os vizinhos...
    for(i = 0, n_con = 0; i < Number_of_ids; i++){
        if(My_node->TCP_fd[i] != -1){
            if(i != My_node->id){
                Close_TCP_Client(i, My_node);
                n_con++;
            }
            if(n_con == My_node->number_of_TCP_channels) break;
        }
    }
    
    if(My_node->was_direct_added){
        reset_My_node(My_node);
        return 0;
    }

    reset_My_node(My_node);

    sprintf(message, "REG 100 3 %03d %02d\n", My_node->net, My_node->id);
    send_message_to_UDP_server(message, response, My_node);

    int items_found = sscanf(response, "%*s %*s %c", &op);

    if (items_found == 1) {
        if (op == '4') {
            // id succesfully left
            Close_TCP_Server(My_node);
            return 0;
        }else{
        //error code from network
        Close_TCP_Server(My_node);
        printf("ERROR: error code (%c) from network using this comand %s\nIn the location of this error its advised to leave it may have compromised My_node struct\n", op, message);
        return 2 ;
        }
    }
    return 0;

}

int cmd_remove_edge(char* id_to_remove_as_char, Node_info* My_node){
    int i;
    if(!is_string_a_number(id_to_remove_as_char) || atoi(id_to_remove_as_char) > 99 || atoi(id_to_remove_as_char) < 0){
        //error id value should be between 99 and 00
        printf("ERROR: input argument id should be a number between 00 and 99\n");
        return 3;
    }
    if(!My_node->is_in_net){
        printf("The node is not in the network.\n");
        return 0;
    }  

    int id_to_remove = atoi(id_to_remove_as_char);

    if(My_node->succ[id_to_remove] != id_to_remove){
        printf("The id given to remove is not our neighbor.\n");
        return 0;
    }

    Close_TCP_Client(id_to_remove, My_node);

    //check if i have any neighbors left
    if(My_node->number_of_TCP_channels == 0){
        //I have no neighbors no need to enter state 1
        My_node->dist[id_to_remove] = INF;
        My_node->succ[id_to_remove] = -1;
    }else{
        //still have neighbors coord them
        for(i = 0; i < Number_of_ids; i++){
            if(My_node->succ[i] == id_to_remove){
                
                if (My_node->adv_debug) {
                    char dist_to_prt[4];
                    My_node->dist[i] == INF ? (void)strcpy(dist_to_prt, "INF") : (void)sprintf(dist_to_prt, "%02d", My_node->dist[i]);
                    printf("DEBUG [%02d]: dist %s -> INF | succ %02d -> -1 | state 0 -> 1\n", i, dist_to_prt, My_node->succ[i]);
                }

                My_node->dist[i] = INF;
                My_node->succ[i] = -1;
                My_node->state[i] = 1;
                Coord_neighbors(i, My_node);
            }
        }
    }
    
    
    return 0;
}

int cmd_direct_join(char* net_as_char, char* id_as_char, Node_info* My_node){

    if(!is_string_a_number(net_as_char) || atoi(net_as_char) > 999 || atoi(net_as_char) < 0){
        //error net value should be between 999 and 000
        printf("ERROR: input argument net should be a number between 000 and 999\n");
        return 3;
    }

    if(!is_string_a_number(id_as_char) || atoi(id_as_char) > 99 || atoi(id_as_char) < 0){
        //error id value should be between 99 and 00
        printf("ERROR: input argument id should be a number between 00 and 99\n");
        return 3;
    }

    int id = atoi(id_as_char), net = atoi(net_as_char), return_code;

    if(My_node->is_in_net){
        //no need to join node is already in th network
        printf("Our node is allready in the network, no need to join again\n");
        return 0;
    }

    My_node->net = net;
    My_node->id = id;
    My_node->is_in_net = true;
    My_node->was_direct_added = true;

    return_code = Create_TCP_Server(My_node);

    return return_code;


}

int cmd_direct_add_edge(char* dest_id_as_char, char* idIP, char* idTCP, Node_info* My_node){
    if(!is_string_a_number(dest_id_as_char) || atoi(dest_id_as_char) > 99 || atoi(dest_id_as_char) < 0){
        //error id value should be between 99 and 00
        printf("ERROR: input argument id should be a number between 00 and 99\n");
        return 3;
    }

    if(is_IP_invalid(idIP)){
        printf("ERROR: input argument idIP does not folow the standard IPv4 structure\n");
        return 3;
    }

    if(is_Port_invalid(idTCP)){
        printf("ERROR: input argument idTCP does not folow the standard Port structure\n");
        return 3;
    }

    int dest_id = atoi(dest_id_as_char), return_code;

    if(!My_node->is_in_net){
        printf("Our node is not in the network, cant add edge\n");
        return 0;
    }

    if(My_node->TCP_fd[dest_id] != -1){
        printf("Our node id(%02d) is already connected to id(%02d)\n", My_node->id, dest_id);
        return 0;
    }

    return_code = Create_and_Connect_TCP_client(idIP, idTCP, dest_id, My_node);

    return return_code;
}