// Made following this guide: https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <math.h>

#define SCR_WIDTH 120
#define SCR_HEIGHT 48
#define WORLD_WIDTH 12
#define WORLD_HEIGHT 12
#define CUBE_SIZE 64
#define FOV 60
#define P_HEIGHT 32

enum WorldCube { VOID, WALL };
const enum WorldCube world[WORLD_HEIGHT][WORLD_WIDTH] = {
  {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, WALL, WALL, WALL, WALL, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, WALL, WALL, WALL, WALL, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, WALL, WALL, WALL, WALL, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, WALL, WALL, WALL, WALL, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, VOID, WALL},
  {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}
};

double degToRad(double angle) {
  return angle * M_PI / 180.0;
}

// returns the angle in the range between 0 and 359
double fixAngleRange(double angle) {
  if (angle < 0) angle = 360 + fmod(angle, 360);
  if (angle >= 360) angle = fmod(angle, 360);
  return angle;
}

double horizontalRayCast(int x, int y, double angle) {
  double rads = degToRad(fixAngleRange(angle)); 
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
  double rads = degToRad(fixAngleRange(angle));
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

void clearProjection(char projection[SCR_WIDTH*SCR_HEIGHT]) {
  for (int i = 0; i < SCR_WIDTH*SCR_HEIGHT; i++) {
    projection[i] = ' ';
  }
}

void displayProjection(const char projection[SCR_WIDTH*SCR_HEIGHT]) {
  for (int i = 0; i < SCR_WIDTH*SCR_HEIGHT; i++) {
    putchar(projection[i]);
    if ((i+1) % SCR_WIDTH == 0) {
      putchar('\n');
    }
  }
}

void draw(char projection[SCR_WIDTH*SCR_HEIGHT], int px, int py, double p_angle) {
  double dist_to_projection = (SCR_WIDTH/2) / tan(degToRad(FOV/2));
  double ray_angle_add = (double)FOV / (double)SCR_WIDTH;
  double ray_angle = p_angle - FOV/2;
  for (int x = SCR_WIDTH-1; x >= 0; x--) {
    double distance = rayCast(px, py, ray_angle) * cos(degToRad(ray_angle - p_angle));
    int height = round(CUBE_SIZE * dist_to_projection / distance);
    if (height > SCR_HEIGHT) {
      height = SCR_HEIGHT;
    }
    for (int i = 0; i < height/2; i++) {
      projection[SCR_WIDTH*(SCR_HEIGHT/2 + i) + x] = projection[SCR_WIDTH*(SCR_HEIGHT/2 - i) + x] = '#';
    }
    ray_angle = fixAngleRange(ray_angle + ray_angle_add);
  }
}

int main() {
  int px = 150, py = 350;
  double p_angle = 30;
  char projection[SCR_WIDTH*SCR_HEIGHT];
  clearProjection(projection);
  draw(projection, px, py, p_angle);
  displayProjection(projection);
  return 0;
}
