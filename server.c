#include "server.h"

// Global variables
pthread_mutex_t rw_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
UserNode *user_list = NULL;
RoomNode *room_list = NULL;
const char *server_MOTD = "Welcome to the Chat Server.\n";

// Initialize the default "Lobby" room
void initialize_default_room() {
    pthread_mutex_lock(&rw_lock);
    create_room("Lobby");
    pthread_mutex_unlock(&rw_lock);
}

// Signal handler for SIGINT (Ctrl+C)
void sigintHandler(int sig_num) {
    printf("\nServer shutting down...\n");
    cleanup_resources();
    exit(0);
}

// Cleanup resources (user list and rooms)
void cleanup_resources() {
    pthread_mutex_lock(&rw_lock);
    UserNode *user = user_list;
    while (user) {
        send(user->socket, "Server shutting down.\n", 22, 0);
        close(user->socket);
        UserNode *temp = user;
        user = user->next;
        free(temp);
    }

    RoomNode *room = room_list;
    while (room) {
        RoomNode *temp = room;
        room = room->next;
        free(temp);
    }
    pthread_mutex_unlock(&rw_lock);
}

// Main server function
int main() {
    signal(SIGINT, sigintHandler); // Handle SIGINT

    // Initialize the default room
    initialize_default_room();

    // Create and configure the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT),
    };

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, BACKLOG) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Listening on port %d...\n", PORT);

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, client_receive, &client_socket);
    }

    return 0;
}
