#include <stdio.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define CUBE_SIZE 64
#define FOV 60
#define P_HEIGHT 32

enum WorldCube { VOID, WALL };
const enum WorldCube world[8][8] = {
  {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}
};

int main() {
  char screen[SCREEN_WIDTH*SCREEN_HEIGHT];
  int px = 0, py = 0, p_angle = 0;

  return 0;
}
