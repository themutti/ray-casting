/* 
  Made following this guide:
  https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/
  and this video:
  https://www.youtube.com/watch?v=94YOd0gimF8/
*/

#define _USE_MATH_DEFINES

#include <stdio.h>  // TODO remove
#include <math.h>
#include <string.h>
#include <windows.h>
#include <unistd.h>

#define SCR_WIDTH 120
#define SCR_HEIGHT 48
#define WORLD_WIDTH 12
#define WORLD_HEIGHT 12
#define CUBE_SIZE 64
#define WALL_CHAR '#'
#define FLOOR_CHAR '\''
#define CEILING_CHAR '-'
#define FOV 60
#define P_HEIGHT 28
#define MOVE_SPD 5
#define ROTATE_SPD 3

enum WorldCube { AIR, WALL };
const enum WorldCube world[WORLD_HEIGHT][WORLD_WIDTH] = {
  {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL},
  {WALL,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR, WALL},
  {WALL,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR, WALL},
  {WALL, WALL, WALL, WALL,  AIR,  AIR, WALL, WALL, WALL,  AIR, WALL, WALL},
  {WALL,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR, WALL,  AIR,  AIR,  AIR, WALL},
  {WALL,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR, WALL,  AIR,  AIR,  AIR, WALL},
  {WALL,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR, WALL,  AIR,  AIR,  AIR, WALL},
  {WALL,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR, WALL, WALL,  AIR, WALL, WALL},
  {WALL, WALL, WALL,  AIR,  AIR,  AIR,  AIR, WALL,  AIR,  AIR,  AIR, WALL},
  {WALL,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR, WALL,  AIR,  AIR,  AIR, WALL},
  {WALL,  AIR,  AIR,  AIR,  AIR,  AIR,  AIR, WALL,  AIR,  AIR,  AIR, WALL},
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

void clearProjection(char projection[SCR_HEIGHT][SCR_WIDTH]) {
  for (int y = 0; y < SCR_HEIGHT; y++) {
    for (int x = 0; x < SCR_WIDTH; x++) {
      projection[y][x] = ' ';
    }
  }
}

void displayProjection(const char projection[SCR_HEIGHT][SCR_WIDTH]) {
  HANDLE buffer = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD c = {0, 0};
  SetConsoleCursorPosition(buffer, c);
  DWORD written;
  for (int i = 0; i < SCR_HEIGHT; i++) {
    WriteConsole(buffer, projection[i], SCR_WIDTH, &written, NULL);
    WriteConsole(buffer, "\n", 1, &written, NULL);
  }
}

void drawProjection(char projection[SCR_HEIGHT][SCR_WIDTH], int px, int py, double p_angle) {
  double dist_to_projection = (SCR_WIDTH/2) / tan(degToRad(FOV/2));
  double ray_angle_add = (double)FOV / (double)SCR_WIDTH;
  double ray_angle = p_angle - FOV/2;
  for (int x = SCR_WIDTH-1; x >= 0; x--) {
    double distance = rayCast(px, py, ray_angle) * cos(degToRad(ray_angle - p_angle));
    int height = round(CUBE_SIZE * dist_to_projection / distance);
    for (int i = 0; i < SCR_HEIGHT/2; i++) {
      int wall_center_y = SCR_HEIGHT / ((double)CUBE_SIZE / (double)P_HEIGHT);
      int top_y = wall_center_y - i - 1;  // top half projection index
      int bottom_y = wall_center_y + i;  // bottom half projection index
      if (top_y >= 0) {
        projection[top_y][x] = i < height/2 ? WALL_CHAR : CEILING_CHAR;
      } else {
        projection[top_y+SCR_HEIGHT][x] = (top_y+SCR_HEIGHT)-wall_center_y < height/2 ? WALL_CHAR : FLOOR_CHAR;
      }
      if (bottom_y < SCR_HEIGHT) {
        projection[bottom_y][x] = i < height/2 ? WALL_CHAR : FLOOR_CHAR;
      } else {
        projection[bottom_y-SCR_HEIGHT][x] = (bottom_y-SCR_HEIGHT)+wall_center_y < height/2 ? WALL_CHAR : CEILING_CHAR;
      }
    }
    ray_angle = fixAngleRange(ray_angle + ray_angle_add);
  }
}

void gameLoop(int px, int py, double p_angle) {
  char projection[SCR_HEIGHT][SCR_WIDTH];
  int last_px = px;
  int last_py = py;
  while (!(GetKeyState(VK_ESCAPE) & (1 << 7))) {
    px += GetKeyState(VK_UP) & (1 << 7) ? cos(degToRad(p_angle)) * MOVE_SPD : 0;
    px -= GetKeyState(VK_DOWN) & (1 << 7) ? cos(degToRad(p_angle)) * MOVE_SPD : 0;
    py -= GetKeyState(VK_UP) & (1 << 7) ? sin(degToRad(p_angle)) * MOVE_SPD : 0;
    py += GetKeyState(VK_DOWN) & (1 << 7) ? sin(degToRad(p_angle)) * MOVE_SPD : 0;
    if (world[py/CUBE_SIZE][px/CUBE_SIZE] == WALL) {
      px = last_px;
      py = last_py;
    }
    last_px = px;
    last_py = py;
    p_angle += GetKeyState(VK_LEFT) & (1 << 7) ? ROTATE_SPD : 0;
    p_angle -= GetKeyState(VK_RIGHT) & (1 << 7) ? ROTATE_SPD : 0;
    clearProjection(projection);
    drawProjection(projection, px, py, p_angle);
    displayProjection(projection);
    Sleep(1000/300);
  }
}

int main() {
  gameLoop(128, 128, 0);
  return 0;
}
