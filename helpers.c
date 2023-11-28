#include "media.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void circle(int height, int width, Display *d, Drawable w, int s, int *pos,
            bool *ch, long *frame) {
  int size = height / 2;
  int h = 0;
  XFillArc(d, w, DefaultGC(d, s), (width / 4) - (*pos / 2),
           (height / 4) - (*pos / 2), size + *pos, size + *pos, 0, 24000);

  if (*ch && *frame % 5 == 0)
    (*pos)++;

  if (*pos > width) {
    *ch = false;
    *pos = 0;
  }
}

int l = 0;

void drawGrid(int height, int width, Display *d, Drawable w, int s, long *frame,
              bool *drwGrid) {

  for (int i = 0; i < l; i += 10) {
    XDrawLine(d, w, DefaultGC(d, s), 0, i, (i * 2), height);
    XDrawLine(d, w, DefaultGC(d, s), 0, i + 1, (i * 2) + 1, height);

    XDrawLine(d, w, DefaultGC(d, s), width, i, (i * 2), 0);
    XDrawLine(d, w, DefaultGC(d, s), width, i + 1, (i * 2) + 1, 0);
  }

  if (*frame % 25 == 0) {
    l += 2;
    if (l > width) {
      *drwGrid = false;
    }
  }
}

int scrollCode = 0;
void drawCode(int height, int width, Display *d, Drawable w, int s, long *frame,
              bool *drwGrid) {

  // XFontStruct *font;
  // int y;
  // char **h = XGetFontPath(d, &y);
  // printf("RR: %s; jjj %i\n", *h, y);

  if (*frame % 8 == 0) {
    scrollCode += 1;
  }
  int altura = 12;
  int i = altura;
  i -= scrollCode;

  char *t = (char *)t1;
  while (*t != '\0') {
    int len = 0;
    bool b = false;
    while (*(t++) != '\n') {
      len++;
      if (*t == '\0')
        b = true;
    }
    if (b)
      break;
    XDrawString(d, w, DefaultGC(d, s), 5, i, t, len);
    XDrawString(d, w, DefaultGC(d, s), width - 300, -i, t, len);
    i += altura;
  }
  usleep(1);
}

// looks cool
void drawGridBuggy(int height, int width, Display *d, Drawable w, int s,
                   long *frame) {
  for (int i = 0; i < width; i += 5) {
    XDrawLine(d, w, DefaultGC(d, s), 0, i, (i * 2), height);
    XDrawLine(d, w, DefaultGC(d, s), 130, i + 16, (i * 2), 15);
  }
}

struct Vector3D {
  float x;
  float y;
  float z;
};
const int matriz = 6;
#define m 6
struct Vector3D arrayDeVectores[m * m];

void llenarMatriz() {
  for (int i = 0; i < matriz; i++)
    for (int j = 0; j < matriz; j++) {
      arrayDeVectores[i + j].x = i;
      arrayDeVectores[i + j].y = j;
      arrayDeVectores[i + j].z = 1;
    }
}

// void drawWaves(int height, int width, Display *d, Drawable w, int s,
//                long *frame) {
void drawWaves() {
  for (int i = 0; i < matriz; i++)
    for (int j = 0; j < matriz; j++)
      printf("AAAA %i %i %f\n", i, j, arrayDeVectores[i + j].x);
}
