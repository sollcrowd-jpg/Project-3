#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <ctype.h>

// Constants
#define PORT 8888
#define BACKLOG 10
#define MAXBUFF 1024

// User Node
typedef struct UserNode {
    int socket;
    char username[50];
    struct UserNode *next;
} UserNode;

// Room Node
typedef struct RoomNode {
    char name[50];
    char users[100][50]; // Maximum 100 users per room
    int user_count;
    struct RoomNode *next;
} RoomNode;

// Global Variables
extern pthread_mutex_t rw_lock; // Read/Write lock for synchronization
extern pthread_mutex_t mutex;   // Mutex lock for critical sections
extern UserNode *user_list;     // Linked list of active users
extern RoomNode *room_list;     // Linked list of active rooms
extern const char *server_MOTD; // Message of the Day

// Function Declarations
void *client_receive(void *ptr);                              // Handles client interaction
void add_user(int socket, const char *username);              // Adds a user to the global user list
void rename_user(int socket, const char *new_username);       // Renames a user
void remove_user(int socket);                                 // Removes a user from the global user list
int create_room(const char *room_name);                      // Creates a new chat room
void add_user_to_room(const char *room_name, const char *username); // Adds a user to a specific room
int remove_user_from_room(const char *room_name, const char *username); // Removes a user from a specific room
void list_users(char *buffer, size_t buffer_size);            // Lists all active users
void list_rooms(char *buffer, size_t buffer_size);            // Lists all active rooms
void sigintHandler(int sig_num);                              // Handles SIGINT (Ctrl+C) for graceful shutdown

#endif
