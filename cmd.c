#include "cmd.h"


//return 0 return to main without error
//return 1 return to main to exit the program without errors
//return 2 return to main to with network error
//return 3 return to main to with incorrect input
//return 4 return to main to exit the program with memory allocation error
//return 5 return to main to exit the program with seting up UDP or TCP error
//return 6 return to main to exit the program with not suposed to happend case
int process_command(char *input, Node_info* My_node){
    char arguments[cmd_arguments][cmd_len], cmd[cmd_len];
    int num_args, return_code;
    
    num_args = sscanf(input, "%s %s %s %s", cmd, arguments[0], arguments[1], arguments[2]);

    if(num_args == 1){
        if(!strcmp(cmd, "l")){
            if(My_node->is_in_net){
                //the node is in the network, so leave
                //cmd_leave();
                
                My_node->is_in_net = false;

                return 0;
            }else{
                //the node is not in the network, so no need to leave
                printf("The node is not in the network, so no need to leave\n");

                return 0;
            }
            

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

        }else if(!strcmp(cmd, "sm")){

        }else if(!strcmp(cmd, "em")){

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input or the number of arguments for that comand is wrong\n", cmd);
            return 3;
        }

    }else if(num_args == 2){

        if(!strcmp(cmd, "n")){

            if(!is_string_a_number(arguments[0]) || atoi(arguments[0]) > 999 || atoi(arguments[0]) < 0){
                //error net value should be between 999 and 000
                printf("ERROR: input argument net should be a number between 000 and 999\n");
                return 3;
            }

            return_code = cmd_show_nodes(arguments[0], My_node);

            return return_code;

        }else if(!strcmp(cmd, "ae")){

        }else if(!strcmp(cmd, "re")){

        }else if(!strcmp(cmd, "sr")){

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input or the number of arguments for that comand is wrong\n", cmd);
            return 3;
        }

    }else if(num_args == 3){

        if(!strcmp(cmd, "j")){

            return cmd_join(arguments[0], arguments[1], My_node);

        }else if(!strcmp(cmd, "m")){

        }else if(!strcmp(cmd, "dj")){

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input or the number of arguments for that comand is wrong\n", cmd);
            return 3;
        }
        
    }else if(num_args == 4){

        if(!strcmp(cmd, "dae")){

        }else{
            //error bad input
            printf("The comand used (%s) is not a valid input or the number of arguments for that comand is wrong\n", cmd);
            return 3;
        }

    }else{
        //unexpected sscanf return
        printf("DEBUG ERROR: (in function process_command) if we are reading this the sscanf returned an unexpected number (not suposed to do that)\n");
        return 6;
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
        printf("The node is allready in the network\n");
        return 0;
    }

    return_code = get_id_info(NULL, NULL, &does_id_exist ,net, id, My_node);

    if(return_code != 0){
        return return_code;
    }

    if(does_id_exist){
        //error id already exists must use a difrent one
        printf("The id chosen (%d) is in use. Must use a difrent id to add the node to the network %d\n", id, net);
        return 0;
    }

    return_code = add_id_to_net(net, id, My_node);

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
        printf("Origin node is not in the network use the join comand first\n");
        return 0;
    }

    return_code = get_id_info(dest_ip, dest_Port, &is_dest_id_in_net, My_node->net, dest_id, My_node);

    if(return_code != 0){
        return return_code;
    }

    if(!is_dest_id_in_net){
        printf("The destiny id does not exist in network use show nodes to know the ids in network\n");
        return 0;
    }

    return_code = Create_and_Connect_TCP_client(dest_ip, dest_Port, dest_id, My_node);

    if(return_code != 0){
        return return_code;
    }



    return 0;
}

int cmd_show_nodes(char* net_as_char, Node_info* My_node){
    char message[UDP_message_len], response[UDP_response_size], op;
    int return_code;

    if(!is_string_a_number(net_as_char) || atoi(net_as_char) > 999 || atoi(net_as_char) < 0){
        //error net value should be between 999 and 000
        printf("ERROR: input argument net should be a number between 000 and 999\n");
        return 3;
    }

    int net = atoi(net_as_char);

    sprintf(message, "NODES 100 0 %d\n", net);

    return_code = send_message_to_UDP_server(message, response, My_node);

    if(return_code != 0){
        return return_code;
    }

    int items_found = sscanf(response, "%*s %*s %c", &op);

    if (items_found == 1) {
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

void cmd_leave(Node_info* My_node){
   /* char message[Max_message_len], response[Max_response_size], op, neigbours[Number_of_ids][Id_len];
    int n_neighbours, i;
    
   //loop para verificar quantos vizinhos tem e quais são...
     for(i = 0, n_con = 0; i < Number_of_ids; i++){
                if(My_node->TCP_fd[i] != -1){
                    if(i =! atoi(My_node->id)){
                        sprintf(neigbours[n_con], "%02d", i);
                        n_con++;
                    }
                    if(n_con == My_node->number_of_TCP_chanels) break;
                }
            }
    //loop para fechar ligação tcp com os vizinhos...
    for(i = 0; i < n_con; i++){
        cmd_remove_edge(neigbours[i], My_node);
    }

    sprintf(message, "REG 100 3 %s %s\n", My_node->net, My_node->id);
    send_message_to_UDP_server(message, response, Server);
    int items_found = sscanf(response, "%*s %*s %c", &op);
    if (items_found == 1) {
        if (op == '4') {
            // id succesfully left
            Close_TCP_Server(Node_info* My_node);
            return 0;
        }else{
        //error code from network
        return 2 ;
        }
    }
    */

}

int cmd_remove_edge(char* id_to_remove_as_char, Node_info* My_node){
    //int return_code;

    if(!is_string_a_number(id_to_remove_as_char) || atoi(id_to_remove_as_char) > 99 || atoi(id_to_remove_as_char) < 0){
        //error id value should be between 99 and 00
        printf("ERROR: input argument id should be a number between 00 and 99\n");
        return 3;
    }

    int id_to_remove = atoi(id_to_remove_as_char);


    if(!My_node->is_in_net){
        //mensagem de erro que o node n existe na net
    }

    if(My_node->TCP_fd[id_to_remove] == -1){
        //mensagem de erro que o node n é vizinho
    }

    return 0;
}

