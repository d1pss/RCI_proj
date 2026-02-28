#include "cmd_aux.h"

int print_ids(char* response, int response_len, char* net){

    int first_id_index = 0;
    char id_to_print[3] = {'0', '0', '\0'};

    for(int i = 0; i < response_len; i++){
        if(response[i] == '\n'){
            first_id_index = i + 1;
        }
    }
    if(first_id_index != 0){
        if(first_id_index != response_len){
            printf("List of Nodes in network %s:\n", net);
            for(int i = first_id_index; i < response_len; i+3){
                id_to_print[0] = response[i];
                id_to_print[0] = response[i+1];
                printf("%s\n", id_to_print);
            }
            return 0;
        }else{
            // there is no nodes in the network
            printf("There are no Nodes in the network %s to show.\n", net);
            return 0;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function print_ids) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        free(response);
        return 6;
    }
}

//return 7 means the net is full
int add_id_to_net(char* net, char* id, Node_info* My_node){
    char message[Max_message_len], *response, op;
    int response_len, return_code;

    //get_new_ip_and_Port(&new_IP, &new_Port);
    //está associado a OWR IP TCP regIP regUDP
    
    sprintf(message, "REG 100 0 %s %s %s %s\n", net, id, My_node->Node_TCP_IP, My_node->Node_TCP_Port);

    return_code = send_message_to_UDP_server(message, &response, &response_len, My_node);

    if(return_code != 0){
        return return_code;
    }

    int items_found = sscanf(response, "%*s %*s %c", &op);

    if (items_found == 1) {
        if (op == '1') {
            // id sucessfuly registred in network
            printf("id was sucessfuly registred in network\n");

            free(response);
            return 0;
        } else if (op == '2') {
            // Network is full
            printf("id was not registred in the network because its full\n");

            free(response);
            return 7;
        }else{
            //error code from network
            printf("ERROR: error code (%c) from network using this comand %s\n", op, message);

            free(response);
            return 2;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function add_id_to_net) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        free(response);
        return 6;
    }
}

int get_id_info(char** id_IP ,char** id_Port ,bool* get_id_info ,char* net, char* id, Node_info* My_node){
    char message[Max_message_len];
    char* response, op;
    int response_len, return_code, items_found;

    sprintf(message, "CONTACT 100 0 %s %s\n", net, id);
   

    return_code = send_message_to_UDP_server(message, &response, &response_len, My_node);

    if(return_code != 0){
        return return_code;
    }

    if(id_IP == NULL || id_Port == NULL){
        items_found = sscanf(response, "%*s %*s %c %*s", &op);
    }else{
        items_found = sscanf(response, "%*s %*s %c %*s %s %s", &op, id_IP, id_Port);
    }

    if (items_found == 1 || items_found == 3) {
        if (op == '1') {
            // ID exists in net
            *get_id_info = true;

            free(response);
            return 0;
        } else if (op == '2') {
            // ID does not exist in net
            *get_id_info = false;

            free(response);
            return 0;
        }else{
            //error code from network
            printf("ERROR: error code (%c) from network using this comand %s\n", op, message);

            free(response);
            return 2;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function add_id_to_net) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        free(response);
        return 6;
    }
}
