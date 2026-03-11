#include "cmd_aux.h"

/********************************************************************************* -----cmd aux funcs----- *********************************************************************************/

int print_ids(char* response, int net){

    int first_id_index = 0, response_len = strlen(response);
    char id_to_print[3] = {'0', '0', '\0'};

    for(int i = 0; i < response_len; i++){
        if(response[i] == '\n'){
            first_id_index = i + 1;
            break;
        }
    }
    if(first_id_index != 0){
        if(first_id_index != response_len){
            printf("List of Nodes in network %d:\n", net);
            for(int i = first_id_index; i < response_len; i += 3){
                id_to_print[0] = response[i];
                id_to_print[1] = response[i+1];
                printf("%s\n", id_to_print);
            }
            return SUCCESS;
        }else{
            // there is no nodes in the network
            printf("There are no Nodes in the network %d to show.\n", net);
            return SUCCESS;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function print_ids) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        
        return ERR_UNEXPECTED;
    }
}

int add_id_to_net(char* net, char* id, Node_info* My_node){
    char message[UDP_message_len], response[UDP_response_size], op_str[2], op;
    int return_code, tid, tid_read;

    tid = get_unique_tid(My_node);

    sprintf(message, "REG %03d 0 %s %s %s %s\n", tid, net, id, My_node->Node_TCP_IP, My_node->Node_TCP_Port);

    return_code = send_message_to_UDP_server(message, response, My_node);

    if(return_code != SUCCESS){
        return return_code;
    }

    int items_found = sscanf(response, "%*s %d %s", &tid_read, op_str);

    if(tid != tid_read){
        printf("Recived message with difrent tid lost data\n");
        return SUCCESS;
    }

    op = op_str[0];

    if(My_node->debug){
        printf("SEND: %sRECIVE: %s\n", message, response);
    }

    if (items_found == 2) {
        if (op == '1') {
            // id sucessfuly registred in network
            return SUCCESS;
        } else if (op == '2') {
            // Network is full
            printf("Server Exchange Error -> Registration Failed: The node server database is full. Cannot join network [%s].\n", net);
            return STATUS_SPECIFIC;
        }else{
            //error code from network
            printf("Server Exchange Error -> Registration Failed: Server returned error code [op=%c] for command [%s].\n", op, message);
            printf("This error could also mean that The id chosen is already registered in network. Please try a difrent identifier.\n");
            return ERR_NET_LOGIC;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function add_id_to_net) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        
        return ERR_UNEXPECTED;
    }
}

int get_id_info(char* id_IP ,char* id_Port ,bool* get_id_info ,char* net ,char* id , Node_info* My_node){
    char message[UDP_message_len], response[UDP_response_size], op, op_str[2];
    int return_code, items_found, tid, tid_read;

    tid = get_unique_tid(My_node);

    sprintf(message, "CONTACT %03d 0 %s %s\n", tid, net, id);
   

    return_code = send_message_to_UDP_server(message, response, My_node);

    if(return_code != SUCCESS){
        return return_code;
    }

    if(id_IP == NULL || id_Port == NULL){
        items_found = sscanf(response, "%*s %d %s", &tid_read, op_str);
    }else{
        items_found = sscanf(response, "%*s %d %s %*s %*s %s %s", &tid_read, op_str, id_IP, id_Port);
    }

    if(tid != tid_read){
        printf("Server Exchange Error -> Transaction Mismatch: Received TID does not match the sent TID. Data discarded to prevent corruption.\n");
        return SUCCESS;
    }

    if(My_node->debug){
        printf("SEND: %sRECIVE: %s\n", message, response);
    }

    op = op_str[0];

    if (items_found == 2 || items_found == 4) {
        if (op == '1') {
            // ID exists in net
            *get_id_info = true;
            return SUCCESS;

        } else if (op == '2') {
            // ID does not exist in net
            *get_id_info = false;
            return SUCCESS;

        }else{
            //error code from network
            printf("Server Exchange Error -> Operation Rejected: Server returned error code [op=%c] for command [%s].\n", op, message);
            return ERR_NET_LOGIC;
        }
    }else{
        //response is not as expected
        printf("DEBUG ERROR: (in function get_id_info) if we are reading this the server sent a bad formated response (not suposed to do that)\n");
        
        return ERR_UNEXPECTED;
    }
}

int print_help(void){
    printf("|-------------------------------------------------------------------------------------|\n"
           "|join (j) net id . . . . . . . . . . | Register node as (id) in the network (net)     |\n"
           "|show nodes (n) net. . . . . . . . . | List all nodes registered in network (net)     |\n"
           "|leave (l) . . . . . . . . . . . . . | Unregister node and close all active edges     |\n"
           "|exit (x). . . . . . . . . . . . . . | Safely leave the network and exit program      |\n"
           "|add edge (ae) id. . . . . . . . . . | Create a TCP session with registered node (id) |\n"
           "|remove edge (re) id . . . . . . . . | Close the TCP session with neighbor node (id)  |\n"
           "|show neighbors (sn) . . . . . . . . | List IDs of all directly connected neighbors   |\n"
           "|announce (a). . . . . . . . . . . . | Advertise node existence to the entire network |\n"
           "|show routing (sr) dest. . . . . . . | Show routing state/path to (dest)              |\n"
           "|start monitor (sm). . . . . . . . . | Enable monitoring of protocol message exchange |\n"
           "|end monitor (em). . . . . . . . . . | Disable monitoring of protocol message exchange|\n"
           "|message (m) dest msg. . . . . . . . | Send a chat message to destination node (dest) |\n"
           "|direct join (dj) net id . . . . . . | Join network (net) as (id) without registration|\n"
           "|direct add edge (dae) id ip port. . | Create a TCP session using specific IP and Port|\n"
           "|-------------------------------------------------------------------------------------|\n");
    return SUCCESS;
}


/********************************************************************************* -----check that is valid funcs----- *********************************************************************************/


bool is_IP_invalid(char* IP){
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

bool is_Port_invalid(char* Port){
    char* endptr;
    // Convert string to long, endptr stores the end conversion point
    long p = strtol(Port, &endptr, 10);

    
    // if *endptr is not \0, there was thrash in the sring
    if (Port == endptr || *endptr != '\0') return true;

    //check if it is a valid Port
    if (p < 1 || p > 65535) return true;

    return false;
}

bool is_string_a_number(char* string){
    char* endptr;
    // Convert string to long, endptr stores the end conversion point
    (void)strtol(string, &endptr, 10);

    // if *endptr is not \0, there was thrash in the sring
    if (string == endptr || *endptr != '\0') return false;



    return true;
}

int get_unique_tid(Node_info* My_node){
    if(My_node->unique_tid == 1000){
        My_node->unique_tid = 0;
    }
    My_node->unique_tid++;
    return My_node->unique_tid - 1;
}


/********************************************************************************* -----Main aux funcs----- *********************************************************************************/


int max(int a, int b){
    return a > b ? a : b;
}

int Check_argv_format(char** argv, int argc){
    if(argc == 5){
        if(is_IP_invalid(argv[1])){
            printf("IP used in ./OWR --> IP <-- TCP regIP regUDP does not folow the standard IPv4 structure\n");
            return EXIT_OK;
        }
        if(is_Port_invalid(argv[2])){
            printf("TCP Port used in ./OWR IP --> TCP <-- regIP regUDP does not folow the standard Port structure\n");
            return EXIT_OK;
        }
        if(is_IP_invalid(argv[3])){
            printf("regIP used in ./OWR IP TCP --> regIP <-- regUDP does not folow the standard IPv4 structure\n");
            return EXIT_OK;
        }
        if(is_Port_invalid(argv[4])){
            printf("UDP Port used in ./OWR IP TCP regIP --> regUDP <-- does not folow the standard Port structure\n");
            return EXIT_OK;
        }
    }else if(argc == 3){
        if(is_IP_invalid(argv[1])){
            printf("IP used in ./OWR --> IP <-- TCP does not folow the standard IPv4 structure\n");
            return EXIT_OK;
        }
        if(is_Port_invalid(argv[2])){
            printf("TCP Port used in ./OWR IP --> TCP <-- does not folow the standard Port structure\n");
            return EXIT_OK;
        }
    }else{
        printf("Comand format is incorrect.\nUsage: ./OWR IP TCP regIP regUDP\n");
        return EXIT_OK;
    }

    return SUCCESS;
}

Node_info* init_Node(char** argv, int argc){
    Node_info* My_node = (Node_info*)malloc(sizeof(Node_info));
    if(My_node == NULL){
        printf("ERROR: error alocating memory\n");
        return NULL;
    }

    //debug variables
    My_node->debug = true;
    My_node->adv_debug = true;

    strcpy(My_node->Node_TCP_IP, argv[1]);
    strcpy(My_node->Node_TCP_Port, argv[2]);

    if(argc == 5){
        strcpy(My_node->UDP_Server_IP, argv[3]);
        strcpy(My_node->UDP_Server_Port, argv[4]);
    }else{
        strcpy(My_node->UDP_Server_IP, DEFAULT_UDP_IP);
        strcpy(My_node->UDP_Server_Port, DEFAULT_UDP_PORT);
    }

    reset_My_node(My_node);

    return My_node;
}

void reset_My_node(Node_info* My_node){
    for(int i = 0; i < Number_of_ids; i++){
        My_node->dist[i] = INF;
        My_node->succ[i] = -1;
        My_node->state[i] = 0;
        My_node->succ_coord[i] = -1;
        My_node->TCP_fd[i] = -1;
        My_node->TCP_pending_fd[i] = -1;
        My_node->pending_uncoord[i] = 0;
    }

    My_node->number_pending_fd = 0;
    My_node->number_of_TCP_channels = 0;

    My_node->is_in_net = false;
    My_node->is_monitoring = false;
    My_node->was_direct_added = false;
    My_node->unique_tid = 0;

    return;
}

int manage_return_code(int return_code, Node_info* My_node){
    if(return_code == SUCCESS || return_code == ERR_INPUT){
        return SUCCESS;
    }

    if(return_code == EXIT_OK || return_code == ERR_MEMORY || return_code == ERR_SOCKET_IO || return_code == ERR_UNEXPECTED){
        if(My_node->debug){
            printf("exiting whith error code %d\n", return_code);
        }
        if(My_node->is_in_net){
            (void)cmd_leave(My_node);
        }
        return EXIT_OK;
    }

    char response;

    printf("do you wish to proced with the program?\n[y/n]\n");
    if(scanf("%c", &response) == 1){
        if(response == 'n'){
            return EXIT_OK;
        }else if(response == 'y'){
            return SUCCESS;
        }else{
            printf("unknown response exting...\n");
            return EXIT_OK;
        }
    }
    printf("unknown response exting...\n");
    return EXIT_OK;

}

void remove_pending_fd(int index_to_remove, Node_info* My_node){
    if(index_to_remove + 1 == Number_of_ids || My_node->TCP_pending_fd[index_to_remove + 1] == -1){
        My_node->TCP_pending_fd[index_to_remove] = -1;
        return;
    }else{
        My_node->TCP_pending_fd[index_to_remove] = My_node->TCP_pending_fd[index_to_remove + 1];
        return remove_pending_fd(index_to_remove + 1, My_node);
    } 
}

int fragment_buffer(char* buffer, char*** frag_buffer, int* n_frags){
    if(buffer == NULL || buffer[0] == '\0'){
        printf("DEBUG ERROR: (in function fragment_buffer) buffer is empy or points to NULL");
        return ERR_UNEXPECTED;
    }

    (*n_frags) = 0;

    int buffer_len = (int)strlen(buffer), curr_frag_strt_indx = 0, curr_frag_len = 0, curr_frag = 0;

    for(int i = 0; i < buffer_len; i++){
        if(buffer[i] == '\n'){
            (*n_frags)++;
        }
    }

    (*frag_buffer) = (char**)malloc((*n_frags) * sizeof(char*));
    if((*frag_buffer) == NULL) return ERR_MEMORY;

    for(int i = 0; i < buffer_len; i++){
        curr_frag_len++;
        if(buffer[i] == '\n'){

            (*frag_buffer)[curr_frag] = (char*)malloc((curr_frag_len + 1) * sizeof(char));
            if((*frag_buffer)[curr_frag] == NULL) return ERR_MEMORY;

            for(int k = 0; k < curr_frag_len; k++){
                (*frag_buffer)[curr_frag][k] = buffer[curr_frag_strt_indx + k];
            }

            (*frag_buffer)[curr_frag][curr_frag_len] = '\0';

            curr_frag_len = 0;
            curr_frag_strt_indx = i + 1;
            curr_frag++;
        }
    }
    return SUCCESS;
}

void free_frag_buffer(char** buffer, int n_frags){
    for(int i = 0; i < n_frags; i++){
        free(buffer[i]);
    }
    free(buffer);
}