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
#define PADDLE_WIDTH 2
#define BALL_RADIUS 1

// Function to clear the screen
void clear_screen() {
    printf("\033[H\033[J");
}

// Function to draw the split court model
void draw_court(int paddle1_pos, int paddle2_pos, int ball_pos, int score1, int score2) {
    // Clear the screen
    clear_screen();

    // Draw player one's court
    for (int i = 0; i < COURT_HEIGHT; i++) {
        for (int j = 0; j < COURT_WIDTH / 2; j++) {
            // Draw left paddle
            if (j == 0 && i >= paddle1_pos && i < paddle1_pos + PADDLE_HEIGHT) {
                printf("|");
            }
            // Draw ball
            else if (j == ball_pos && i == ball_pos) {
                printf("O");
            }
            else {
                printf(" ");
            }
        }
        // Draw player one's score
        if (i == 0) {
            printf("  Player 1: %d", score1);
        }
        printf("\n");
    }

    // Draw net
    for (int i = 0; i < COURT_HEIGHT; i++) {
        printf("|");
        printf("\n");
    }

    // Draw player two's court
    for (int i = 0; i < COURT_HEIGHT; i++) {
        for (int j = 0; j < COURT_WIDTH / 2; j++) {
            // Draw right paddle
            if (j == COURT_WIDTH / 2 - PADDLE_WIDTH && i >= paddle2_pos && i < paddle2_pos + PADDLE_HEIGHT) {
                printf("|");
            }
            // Draw ball
            else if (j == COURT_WIDTH / 2 - BALL_RADIUS - ball_pos && i == ball_pos) {
                printf("O");
            }
            else {
                printf(" ");
            }
        }
        // Draw player two's score
        if (i == 0) {
            printf("  Player 2: %d", score2);
        }
        printf("\n");
    }
}

// Function to extract game state information from the received message
void extract_game_state(char *message, int *paddle_position, int *ball_position, int *score) {
    char *token = strtok(message, ",");
    *paddle_position = atoi(token);
    token = strtok(NULL, ",");
    *ball_position = atoi(token);
    token = strtok(NULL, ",");
    *score = atoi(token);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(PORT);

    // Connect to server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    // Game loop
    while (1) {
        // Get input for paddle movement (replace this with actual input mechanism)
        int paddle_position;
        printf("Enter paddle position: ");
        scanf("%d", &paddle_position);

        // Send paddle position to server
        snprintf(buffer, BUFFER_SIZE, "%d", paddle_position);
        if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
            perror("Error sending message");
            exit(EXIT_FAILURE);
        }

        // Receive updated game state from server
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) {
            printf("Server disconnected\n");
            break;
        } else {
            buffer[bytes_received] = '\0';
            int received_paddle_position, ball_position, score;
            extract_game_state(buffer, &received_paddle_position, &ball_position, &score);
            draw_court(received_paddle_position, paddle_position, ball_position, score, 0);
        }

        // Add a delay here (optional) to control the speed of the game
        // sleep(1);
    }

    // Close socket
    close(client_fd);

    return 0;
}
