#include "cmd_aux.h"

int print_ids(char* response, char* net){

    int first_id_index = 0, response_len = strlen(response);
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
        
        return 6;
    }
}

//return 7 means the net is full
int add_id_to_net(char* net, char* id, Node_info* My_node){
    char message[Max_message_len], response[Max_response_size], op;
    int return_code;

    //get_new_ip_and_Port(&new_IP, &new_Port);
    //está associado a OWR IP TCP regIP regUDP
    
    sprintf(message, "REG 100 0 %s %s %s %s\n", net, id, My_node->Node_TCP_IP, My_node->Node_TCP_Port);

    return_code = send_message_to_UDP_server(message, response, My_node);

    if(return_code != 0){
        return return_code;
    }

    int items_found = sscanf(response, "%*s %*s %c", &op);

    if (items_found == 1) {
        if (op == '1') {
            // id sucessfuly registred in network
            printf("id was sucessfuly registred in network\n");

            
            return 0;
        } else if (op == '2') {
            // Network is full
            printf("id was not registred in the network because its full\n");

            
            return 7;
        }else{
            //error code from network
            printf("ERROR: error code (%c) from network using this comand %s\n", op, message);

           
            return 2;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function add_id_to_net) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        
        return 6;
    }
}

int get_id_info(char** id_IP ,char** id_Port ,bool* get_id_info ,char* net, char* id, Node_info* My_node){
    char message[Max_message_len];
    char response[Max_response_size], op;
    int return_code, items_found;

    sprintf(message, "CONTACT 100 0 %s %s\n", net, id);
   

    return_code = send_message_to_UDP_server(message, response, My_node);

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

            
            return 0;
        } else if (op == '2') {
            // ID does not exist in net
            *get_id_info = false;

           
            return 0;
        }else{
            //error code from network
            printf("ERROR: error code (%c) from network using this comand %s\n", op, message);

            
            return 2;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function add_id_to_net) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        
        return 6;
    }
}


int max(int a, int b)
{
    return a > b ? a : b;
}

void print_help(void){
    printf("join (j) net id .   .   .   .   .   . | register node as (id) in the (net)\n\\
            show nodes (n) net  .   .   .   .   . | print nodes registred in (net)\n\\
            leave (l)   .   .   .   .   .   .   . | unregister node\n\\
            exit (x)    .   .   .   .   .   .   . | exit the program\n\\
            add edge (ae) id    .   .   .   .   . | create a TCP chanel between node and destiny (id)\n\\
            remove edge (re) id .   .   .   .   . | closes the TCP chanel between node and destiny (id)\n\\
            show neighbors (sg) .   .   .   .   . | print the ids directly connected to the node\n\\
            announce (a)    .   .   .   .   .   . | announce the node to the other nodes in the network\n\\
            show routing (sr) dest  .   .   .   . | ---falta escrever---\n\\
            start monitor (sm)  .   .   .   .   . | ---falta escrever---\n\\
            end monitor (em)    .   .   .   .   . | ---falta escrever---\n\\
            message (m) dest message    .   .   . | send (message) to node (dest)\n\\
            direct join (dj) net id .   .   .   . | ---falta escrever---\n\\
            direct add edge (dae) id idIP idTCP . | ---falta escrever---\n");
    return;
}

void Check_argv_format(char** argv, int argc){
    if(argc == 5){
        if(is_IP_invalid(argv[1])){
            printf("IP used in ./OWR --> IP <-- TCP regIP regUDP does not folow the standard IPv4 structure\n");
            return 0;
        }
        if(is_Port_invalid(argv[2])){
            printf("TCP Port used in ./OWR IP --> TCP <-- regIP regUDP does not folow the standard Port structure\n");
            return 0;
        }
        if(is_IP_invalid(argv[3])){
            printf("regIP used in ./OWR IP TCP --> regIP <-- regUDP does not folow the standard IPv4 structure\n");
            return 0;
        }
        if(is_Port_invalid(argv[4])){
            printf("UDP Port used in ./OWR IP TCP regIP --> regUDP <-- does not folow the standard Port structure\n");
            return 0;
        }
    }else if(argc == 3){
        if(is_IP_invalid(argv[1])){
            printf("IP used in ./OWR --> IP <-- TCP does not folow the standard IPv4 structure\n");
            return 0;
        }
        if(is_Port_invalid(argv[2])){
            printf("TCP Port used in ./OWR IP --> TCP <-- does not folow the standard Port structure\n");
            return 0;
        }
    }else{
        printf("Comand format is incorrect.\nUsage: ./OWR IP TCP regIP regUDP\n");
        return 0;
    }
}

bool is_IP_invalid(char* IP) {
    int a, b, c, d;
    int last_index = 0;

    // %n stores the index of the last element into last_index
    if (sscanf(IP, "%d.%d.%d.%d%n", &a, &b, &c, &d, &last_index) != 4) {
        //inalid format
        return true; 
    }

    //if the last_index is no a end of string the IP contains extra chars
    if (IP[last_index] != '\0') {
        return true;
    }

    //check if the numbers are between the porper values
    if (a < 0 || a > 255 || b < 0 || b > 255 || 
        c < 0 || c > 255 || d < 0 || d > 255) {
        return true;
    }

    return false;
}

bool is_Port_invalid(char* Port) {
    char* endptr;
    // Convert string to long, endptr stores the end conversion point
    long p = strtol(Port, &endptr, 10);

    
    // if *endptr is not \0, there was thrash in the sring
    if (Port == endptr || *endptr != '\0') return true;

    //check if it is a valid Port
    if (p < 1 || p > 65535) return true;

    return false;
}

Node_info* init_Node(char** argv, int argc){
    Node_info* My_node = (Node_info*)malloc(sizeof(Node_info));
    if(My_node == NULL){
        printf("ERROR: error alocating memory");
        return NULL;
    }

    strcpy(My_node->Node_TCP_IP, argv[1]);
    strcpy(My_node->Node_TCP_Port, argv[2]);

    if(argc == 5){
        strcpy(My_node->UDP_Server_IP, argv[3]);
        strcpy(My_node->UDP_Server_Port, argv[4]);
    }else{
        strcpy(My_node->UDP_Server_IP, DEFAULT_UDP_IP);
        strcpy(My_node->UDP_Server_Port, DEFAULT_UDP_PORT);
    }

    for(int i = 0; i < Number_of_ids; i++){
        My_node->dist[i] = -1;
        My_node->succ[i] = -1;
        My_node->state[i] = 0;
        My_node->TCP_fd[i] = -1;
    }

    My_node->number_of_TCP_chanels = 0;

    My_node->is_in_net = false;
    My_node->is_monitoring = false;

    return My_node;
}

bool is_string_a_number(char* string){
    char* endptr;
    // Convert string to long, endptr stores the end conversion point
    long p = strtol(string, &endptr, 10);

    // if *endptr is not \0, there was thrash in the sring
    if (string == endptr || *endptr != '\0') return false;

    return true;
}