// Made following this guide: https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <math.h>

#define SCR_WIDTH 120
#define SCR_HEIGHT 48
#define WORLD_WIDTH 8
#define WORLD_HEIGHT 8
#define CUBE_SIZE 64
#define FOV 60
#define P_HEIGHT 32

enum WorldCube { VOID, WALL };
const enum WorldCube world[WORLD_HEIGHT][WORLD_WIDTH] = {
  {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}
};

void clear_projection(char projection[SCR_WIDTH*SCR_HEIGHT]) {
  for (int i = 0; i < SCR_WIDTH*SCR_HEIGHT; i++) {
    projection[i] = '#';
  }
}

void display_projection(const char projection[SCR_WIDTH*SCR_HEIGHT]) {
  for (int i = 0; i < SCR_WIDTH*SCR_HEIGHT; i++) {
    putchar(projection[i]);
    if ((i+1) % SCR_WIDTH == 0) {
      putchar('\n');
    }
  }
}

double degToRad(double angle) {
  return angle * M_PI / 180.0;
}

double horizontalRayCast(int x, int y, double angle) {
  double rads = degToRad(angle); 
  int is_facing_up = angle < 180;

  int intersect_y = (int)(y / CUBE_SIZE) * CUBE_SIZE + (is_facing_up ? -1 : CUBE_SIZE);
  int intersect_x = x + (y - intersect_y) / tan(rads);
  int y_add = is_facing_up ? -CUBE_SIZE : CUBE_SIZE;
  // not in the guide but the code doesn't work without controlling the sign here
  int x_add = CUBE_SIZE / tan(rads) * (is_facing_up ? 1 : -1);

  if (intersect_x < 0 || intersect_y < 0 ||
        intersect_x >= CUBE_SIZE*WORLD_WIDTH ||
        intersect_y >= CUBE_SIZE*WORLD_HEIGHT) {
      return -1;
  }
  while (world[intersect_y/64][intersect_x/64] != WALL) {
    intersect_y += y_add;
    intersect_x += x_add;
    if (intersect_x < 0 || intersect_y < 0 ||
        intersect_x >= CUBE_SIZE*WORLD_WIDTH ||
        intersect_y >= CUBE_SIZE*WORLD_HEIGHT) {
      return -1;
    }
  }
  return sqrt(pow(x - intersect_x, 2) + pow(y - intersect_y, 2));
}

double verticalRayCast(int x, int y, double angle) {
  double rads = degToRad(angle);
  int is_facing_right = angle < 90 || angle >= 270;

  int intersect_x = (int)(x / CUBE_SIZE) * CUBE_SIZE + (is_facing_right ? CUBE_SIZE : -1);
  int intersect_y = y + (x - intersect_x) * tan(rads);
  int x_add = is_facing_right ? CUBE_SIZE : -CUBE_SIZE;
  // not in the guide but the code doesn't work without controlling the sign here
  int y_add = CUBE_SIZE * tan(rads) * (is_facing_right ? -1 : 1);

  if (intersect_x < 0 || intersect_y < 0 ||
        intersect_x >= CUBE_SIZE*WORLD_WIDTH ||
        intersect_y >= CUBE_SIZE*WORLD_HEIGHT) {
      return -1;
  }
  while (world[intersect_y/64][intersect_x/64] != WALL) {
    intersect_x += x_add;
    intersect_y += y_add;
    if (intersect_x < 0 || intersect_y < 0 ||
        intersect_x >= CUBE_SIZE*WORLD_WIDTH ||
        intersect_y >= CUBE_SIZE*WORLD_HEIGHT) {
      return -1;
    }
  }
  return sqrt(pow(x - intersect_x, 2) + pow(y - intersect_y, 2));
}

double rayCast(int x, int y, double angle) {
  double hor_distance = horizontalRayCast(x, y, angle);
  double ver_distance = verticalRayCast(x, y, angle);
  if (hor_distance < 0 && ver_distance >= 0) return ver_distance;
  if (ver_distance < 0 && hor_distance >= 0) return hor_distance;
  return hor_distance < ver_distance ? hor_distance : ver_distance;
}

int main() {
  int px = 256, py = 256;
  double p_angle = 60;
  
  // char projection[SCR_WIDTH*SCR_HEIGHT];
  // clear_projection(projection);
  // display_projection(projection);

  return 0;
}
