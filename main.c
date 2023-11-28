#include "helpers.c"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool drw_cir = false;
int height;
int width;
Display *d;
Window w;
XEvent e;
const char *msg = "TEXTO DE EJEMPLO";
int s;
int cir = 0;
int drawGraphics();
void resetEvent(XEvent *);
long frame = 0;

int main(void) {

  d = XOpenDisplay(NULL);
  if (d == NULL) {
    fprintf(stderr, "Cannot open display\n");
    exit(1);
  }

  s = DefaultScreen(d);
  w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 500, 500, 1,
                          BlackPixel(d, s), 0x00FF0000);
  XSelectInput(d, w,
               ExposureMask | KeyPressMask | ButtonPressMask |
                   ButtonReleaseMask | PointerMotionMask);
  XMapWindow(d, w);
  // XStoreName(d, w, "test");

  int i = 0;
  XSync(d, 0);
  XWindowAttributes windowAttributes;
  llenarMatriz();

  while (1) {
    XGetWindowAttributes(d, w, &windowAttributes);
    width = windowAttributes.width;
    height = windowAttributes.height;

    if (XPending(d))
      XNextEvent(d, &e); // BLoquea el programa hasta que haya un evento
    if (e.type == Expose) {
      XFillRectangle(d, w, DefaultGC(d, s), 20, 20, i, 10);
      XDrawString(d, w, DefaultGC(d, s), 10, 50, msg, strlen(msg));
    }

    switch (e.type) {
    case ButtonPress:
      printf("Button press: button %u, x = %d, y = %d\n", e.xbutton.button,
             e.xbutton.x, e.xbutton.y);
      break;
    case ButtonRelease:
      printf("Button release: button %u, x = %d, y = %d\n", e.xbutton.button,
             e.xbutton.x, e.xbutton.y);
      break;
    case MotionNotify:
      printf("Motion: x = %d, y = %d\n", e.xmotion.x, e.xmotion.y);
      break;
    case KeyPress:
      drw_cir = true;
      break;
    }

    XClearWindow(d, w);
    drawGraphics();
    usleep(1);
    resetEvent(&e);
    frame++;
  }

  XCloseDisplay(d);
  return 0;
}

bool drw_grid = true;
int drawGraphics() {
  // if (drw_cir)
  //   circle(height, width, d, w, s, &cir, &drw_cir, &frame);

  if (drw_grid)
    drawGrid(height, width, d, w, s, &frame, &drw_grid);
  else
    drawGridBuggy(height, width, d, w, s, &frame);

  drawCode(height, width, d, w, s, &frame, &drw_grid);
  drawWaves();

  return 0;
}

void resetEvent(XEvent *e) { e->type = -1; }
