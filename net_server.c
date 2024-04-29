
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 2001
#define BUFFER_SIZE 1024
#define COURT_HEIGHT 20
#define COURT_WIDTH 40
#define PADDLE_HEIGHT 3
#define PADDLE_WIDTH 1
#define BALL_SPEED 1

// Define structures to represent game state
typedef struct {
    int paddle_position;
    int ball_position;
    int ball_velocity;
    int score;
} GameState;

// Function to update game state based on paddle movement
void update_game_state(GameState *game_state, int new_paddle_position) {
    game_state->paddle_position = new_paddle_position;
}

// Function to update ball position
void update_ball_position(GameState *game_state) {
    game_state->ball_position += game_state->ball_velocity;
}

// Function to handle ball collisions
void handle_collisions(GameState *game_state) {
    // Handle top and bottom wall collisions
    if (game_state->ball_position < 0 || game_state->ball_position >= COURT_HEIGHT) {
        game_state->ball_velocity = -game_state->ball_velocity;
    }

    // Handle paddle collisions
    if (game_state->ball_position == game_state->paddle_position) {
        // Increase score
        game_state->score++;
        
        // Reverse ball velocity
        game_state->ball_velocity = -game_state->ball_velocity;
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 1) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connection...\n");

    // Accept incoming connection
    client_len = sizeof(client_addr);
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted\n");

    // Initialize game state
    GameState game_state;
    game_state.paddle_position = COURT_HEIGHT / 2;
    game_state.ball_position = COURT_HEIGHT / 2;
    game_state.ball_velocity = BALL_SPEED;
    game_state.score = 0;

    // Main game loop
    while (1) {
        // Receive paddle position from client
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) {
            printf("Client disconnected\n");
            break;
        } else {
            int new_paddle_position = atoi(buffer);
            update_game_state(&game_state, new_paddle_position);
        }

        // Update ball position
        update_ball_position(&game_state);

        // Handle collisions
        handle_collisions(&game_state);

        // Send updated game state to client
        snprintf(buffer, BUFFER_SIZE, "%d,%d,%d", game_state.paddle_position, game_state.ball_position, game_state.score);
        if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
            perror("Error sending message");
            exit(EXIT_FAILURE);
        }
    }

    // Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
