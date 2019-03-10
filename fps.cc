//#include <vector>
#include <iostream>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>

static const int WIDTH = 120;
static const int HEIGHT = 40;

float playerX = 8.0f;
float playerY = 8.0f;
float playerA = 0.0f;

int mapHeight = 16;
int mapWidth = 16;

float fov = 3.14159 / 4;
float depth = 16.0f;

// mvaddch(2 + tetrominoY, WIDTH + 7 + tetrominoX, ' ');

int main() {
    char *screen = new char[WIDTH * HEIGHT];

    std::string map;

    map += "################";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "################";
    
    initscr();

    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    while (1) {
        //int ch = getch();
        for (int x = 0; x < WIDTH; x++) {
            float rayAngle = (playerA - fov / 2.0f) + ((float) x / (float) WIDTH) * fov;

            float distanceToWall = 0;
            bool hitWall = false;

            float eyeX = sinf(rayAngle);
            float eyeY = cosf(rayAngle);

            while (!hitWall && distanceToWall < depth) {
                distanceToWall += 0.1f;

                int testX = (int) (playerX + eyeX * distanceToWall);
                int testY = (int) (playerY + eyeY * distanceToWall);

                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight) {
                    hitWall = true;
                    distanceToWall = depth;
                } else {
                    if (map[testY * mapWidth + testX] == '#') {
                        hitWall = true;
                    }
                }
            }

            int ceiling = (float) (HEIGHT / 2.0) - HEIGHT / ((float) distanceToWall);
            int floor = HEIGHT - ceiling;

            for (int y = 0; y < HEIGHT; y++) {
                if (y < ceiling) {
                    screen[y * WIDTH + x] = '+';
                } else if (y > ceiling && y <= floor) {
                    screen[y * WIDTH + x] = '#';
                } else {
                    screen[y * WIDTH + x] = '+';
                }
            }
        }

        // screen[WIDTH * HEIGHT - 1] = '\0';
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                mvaddch(y, x, screen[y * WIDTH + x]);
            }
        }

        refresh();
        usleep(50000);
    }

    endwin();

    return 0;
}
