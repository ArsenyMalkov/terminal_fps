#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>
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
    wchar_t *screen = new wchar_t[WIDTH * HEIGHT];

    std::string map;

    bool quit = false;

    map += "################";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..........#...#";
    map += "#..........#...#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#.......########";
    map += "#..............#";
    map += "#..............#";
    map += "################";
    
    setlocale(LC_ALL, "");
    initscr();

    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    while (!quit) {
        int ch = getch();

        if (ch != ERR) {
            if (ch == 'q') {
                quit = true;
            }

            playerA -= (ch == 'a') ? 0.1 : 0;
            playerA += (ch == 'd') ? 0.1 : 0;

            if (ch == 'w') {
                playerX += sinf(playerA) * 1.0f;
                playerY += cosf(playerA) * 1.0f;

                if (map[(int) playerY * mapWidth + (int) playerX] == '#') {
                    playerX -= sinf(playerA) * 1.0f;
                    playerY -= cosf(playerA) * 1.0f;
                }
            }
            
            if (ch == 's') {
                playerX -= sinf(playerA) * 1.0f;
                playerY -= cosf(playerA) * 1.0f;

                if (map[(int) playerY * mapWidth + (int) playerX] == '#') {
                    playerX += sinf(playerA) * 1.0f;
                    playerY += cosf(playerA) * 1.0f;
                }
            }
        }

        for (int x = 0; x < WIDTH; x++) {
            float rayAngle = (playerA - fov / 2.0f) + ((float) x / (float) WIDTH) * fov;

            float distanceToWall = 0;
            bool hitWall = false;
            bool boundary = false;

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
                        
                        std::vector<std::pair<float, float>> p;

                        for (int tx = 0; tx < 2; tx++) {
                            for (int ty = 0; ty < 2; ty++) {
                                float vy = (float) testY + ty - playerY;
                                float vx = (float) testX + tx - playerX;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (eyeX * vx / d) + (eyeY * vy / d);
                                p.push_back(std::make_pair(d, dot));
                            }
                        }

                        std::sort(p.begin(), p.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) { return left.first < right.first; });

                        float bound = 0.01;
                        if (acos(p.at(0).second) < bound) boundary = true;
                        if (acos(p.at(1).second) < bound) boundary = true;
                    }
                }
            }

            int ceiling = (float) (HEIGHT / 2.0) - HEIGHT / ((float) distanceToWall);
            int floor = HEIGHT - ceiling;

            short shade = ' ';
            if (distanceToWall <= depth / 4.0f) shade = 0x2588;
            else if (distanceToWall < depth / 3.0f) shade = 0x2593;
            else if (distanceToWall < depth / 2.0f) shade = 0x2592;
            else if (distanceToWall < depth) shade = 0x2591;
            else shade = ' ';

            if (boundary) shade = ' ';

            for (int y = 0; y < HEIGHT; y++) {
                if (y < ceiling) {
                    screen[y * WIDTH + x] = ' ';
                } else if (y > ceiling && y <= floor) {
                    screen[y * WIDTH + x] = shade;
                } else {
                    short floorShade = ' ';
                    float b = 1.0f - (((float) y - HEIGHT / 2.0f) / ((float) HEIGHT / 2.0f));
                    if (b < 0.25) floorShade = '#';
                    else if (b < 0.5) floorShade = 'x';
                    else if (b < 0.75) floorShade = '.';
                    else if (b < 0.9) floorShade = '-';
                    else floorShade = ' ';
                    screen[y * WIDTH + x] = floorShade;
                }
            }
        }

        // screen[WIDTH * HEIGHT - 1] = '\0';
        // mvprint(400, 0, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", playerX, playerY, playerA, 1.0f / elapsedTime);
        
        for (int nx = 0; nx < mapWidth; nx++) {
            for (int ny = 0; ny < mapWidth; ny++) {
                screen[(ny + 1) * WIDTH + nx] = map[ny * mapWidth + nx];
            }
        }

        screen[((int) playerY + 1) * WIDTH + (int) playerX] = 'P';

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                wchar_t c[] = { screen[y * WIDTH + x], 0 };
                mvaddwstr(y, x, c);
            }
        }

        refresh();
        usleep(50000);
    }

    endwin();

    return 0;
}
