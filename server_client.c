#include "server.h"

// Function to handle client interactions
void *client_receive(void *ptr) {
    int client_socket = *(int *)ptr;
    char buffer[MAXBUFF], cmd[MAXBUFF], username[50];
    char *args[80];

    // Assign a default guest username
    sprintf(username, "guest%d", client_socket);

    // Add the user to the global user list and the default room (Lobby)
    pthread_mutex_lock(&rw_lock);
    add_user(client_socket, username);
    add_user_to_room("Lobby", username);
    pthread_mutex_unlock(&rw_lock);

    // Send the welcome message
    send(client_socket, server_MOTD, strlen(server_MOTD), 0);

    while (1) {
        int received = read(client_socket, buffer, MAXBUFF);
        if (received <= 0) {
            // If the client disconnects, remove them and close the socket
            pthread_mutex_lock(&rw_lock);
            remove_user(client_socket);
            pthread_mutex_unlock(&rw_lock);
            close(client_socket);
            break;
        }

        buffer[received] = '\0';
        strcpy(cmd, buffer);

        // Tokenize the command
        args[0] = strtok(cmd, " \n");
        int i = 0;
        while (args[i] != NULL) {
            args[++i] = strtok(NULL, " \n");
        }

        if (args[0] == NULL) {
            send(client_socket, "Invalid command.\n", 18, 0);
            continue;
        }

        if (strcmp(args[0], "login") == 0 && args[1]) {
            pthread_mutex_lock(&rw_lock);
            rename_user(client_socket, args[1]);
            pthread_mutex_unlock(&rw_lock);
            send(client_socket, "Logged in.\n", 11, 0);
        } else if (strcmp(args[0], "create") == 0 && args[1]) {
            pthread_mutex_lock(&rw_lock);
            if (create_room(args[1])) {
                send(client_socket, "Room created.\n", 14, 0);
            } else {
                send(client_socket, "Room already exists.\n", 22, 0);
            }
            pthread_mutex_unlock(&rw_lock);
        } else if (strcmp(args[0], "join") == 0 && args[1]) {
            pthread_mutex_lock(&rw_lock);
            add_user_to_room(args[1], username);
            pthread_mutex_unlock(&rw_lock);
            send(client_socket, "Room joined.\n", 13, 0);
        } else if (strcmp(args[0], "leave") == 0 && args[1]) {
            pthread_mutex_lock(&rw_lock);
            if (remove_user_from_room(args[1], username)) {
                send(client_socket, "Left the room.\n", 15, 0);
            } else {
                send(client_socket, "Room not found or you are not in it.\n", 36, 0);
            }
            pthread_mutex_unlock(&rw_lock);
        } else if (strcmp(args[0], "rooms") == 0) {
            pthread_mutex_lock(&rw_lock);
            list_rooms(buffer, sizeof(buffer));
            pthread_mutex_unlock(&rw_lock);
            send(client_socket, buffer, strlen(buffer), 0);
        } else if (strcmp(args[0], "users") == 0) {
            pthread_mutex_lock(&rw_lock);
            list_users(buffer, sizeof(buffer));
            pthread_mutex_unlock(&rw_lock);
            send(client_socket, buffer, strlen(buffer), 0);
        } else if (strcmp(args[0], "exit") == 0) {
            pthread_mutex_lock(&rw_lock);
            remove_user(client_socket);
            pthread_mutex_unlock(&rw_lock);
            close(client_socket);
            break;
        } else {
            send(client_socket, "Unknown command.\n", 17, 0);
        }
    }

    return NULL;
}
