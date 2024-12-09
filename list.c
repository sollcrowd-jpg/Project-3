#include "server.h"

// Add a user to the user list
void add_user(int socket, const char *username) {
    UserNode *new_user = (UserNode *)malloc(sizeof(UserNode));
    if (!new_user) return;
    strcpy(new_user->username, username);
    new_user->socket = socket;

    new_user->next = user_list;
    user_list = new_user;
}

// Rename a user
void rename_user(int socket, const char *new_username) {
    UserNode *current = user_list;
    while (current) {
        if (current->socket == socket) {
            strcpy(current->username, new_username);
            break;
        }
        current = current->next;
    }
}

// Remove a user from the user list
void remove_user(int socket) {
    UserNode *current = user_list, *prev = NULL;

    while (current) {
        if (current->socket == socket) {
            if (prev) {
                prev->next = current->next;
            } else {
                user_list = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
}

// Create a new room
int create_room(const char *room_name) {
    RoomNode *room = room_list;
    while (room) {
        if (strcmp(room->name, room_name) == 0) {
            return 0; // Room already exists
        }
        room = room->next;
    }

    RoomNode *new_room = (RoomNode *)malloc(sizeof(RoomNode));
    if (!new_room) return 0;
    strcpy(new_room->name, room_name);
    new_room->user_count = 0;
    new_room->next = room_list;
    room_list = new_room;
    return 1;
}

// Add a user to a room
void add_user_to_room(const char *room_name, const char *username) {
    RoomNode *room = room_list;

    while (room) {
        if (strcmp(room->name, room_name) == 0) {
            for (int i = 0; i < room->user_count; i++) {
                if (strcmp(room->users[i], username) == 0) {
                    return; // User already in the room
                }
            }

            strcpy(room->users[room->user_count++], username);
            return;
        }
        room = room->next;
    }

    create_room(room_name);
    add_user_to_room(room_name, username);
}

// Remove a user from a room
int remove_user_from_room(const char *room_name, const char *username) {
    RoomNode *room = room_list;

    while (room) {
        if (strcmp(room->name, room_name) == 0) {
            for (int i = 0; i < room->user_count; i++) {
                if (strcmp(room->users[i], username) == 0) {
                    for (int j = i; j < room->user_count - 1; j++) {
                        strcpy(room->users[j], room->users[j + 1]);
                    }
                    room->user_count--;
                    return 1;
                }
            }
        }
        room = room->next;
    }
    return 0; // Room not found
}

// List all users
void list_users(char *buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "Active users:\n");
    UserNode *current = user_list;
    while (current) {
        strncat(buffer, current->username, buffer_size - strlen(buffer) - 1);
        strncat(buffer, "\n", buffer_size - strlen(buffer) - 1);
        current = current->next;
    }
}

// List all rooms
void list_rooms(char *buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "Available rooms:\n");
    RoomNode *current = room_list;
    while (current) {
        strncat(buffer, current->name, buffer_size - strlen(buffer) - 1);
        strncat(buffer, "\n", buffer_size - strlen(buffer) - 1);
        current = current->next;
    }
}
