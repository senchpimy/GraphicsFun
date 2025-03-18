#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  Display *display = XOpenDisplay(NULL);

  if (!display) {
    fprintf(stderr, "Unable to open display.\n");
    return 1;
  }

  Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0,
                                      WIDTH, HEIGHT, 0, 0, 0x00FF0000);

  XSelectInput(display, window, ExposureMask | KeyPressMask);
  // ExposureMask | KeyPressMask | MotionNotify | ButtonPressMask);
  XMapWindow(display, window);
  XSync(display, 0);
  int t = 0;
  int s;

  s = DefaultScreen(display);
  XEvent e;
  while (1) {
    printf("valor %i\n", XPending(display));
    printf("valor t %i\n", t);
    if (XPending(display))
      XNextEvent(display, &e); // BLoquea el programa hasta que haya un evento
    if (e.type == KeyPress) {
      t = 0;
      e.type = 0;
    } else {
      t++;
    }
    XClearWindow(display, window);
    XDrawLine(display, window, DefaultGC(display, s), 0, 0, 20, t / 100);
    usleep(1000);

    // XDrawPoint(display, window, DefaultGC(display, window), 1, 1);
    // XDrawLine(display, window, DefaultGC(display, window), 10, 10, 100, 100);
  }

  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return 0;
}
