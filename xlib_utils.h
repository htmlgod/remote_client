#ifndef XLIB_UTILS_H
#define XLIB_UTILS_H

#include <string.h>    // memset
#include <unistd.h>    // sleep and usleep
#include <X11/Xlib.h>  // X11
#include <X11/Xutil.h> // XGetPixel and XDestroyImage

#include <string>

extern Display* display;
void click(Display *display, int button)
{
    XEvent event;
    memset (&event, 0, sizeof (event));
    event.xbutton.button = button;
    event.xbutton.same_screen = True;
    event.xbutton.subwindow = DefaultRootWindow (display);
    while (event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer (display, event.xbutton.window,
             &event.xbutton.root, &event.xbutton.subwindow,
             &event.xbutton.x_root, &event.xbutton.y_root,
             &event.xbutton.x, &event.xbutton.y,
             &event.xbutton.state);
    }
    // Press
    event.type = ButtonPress;
    if (XSendEvent (display, PointerWindow, True, ButtonPressMask, &event) == 0)
        fprintf (stderr, "Error to send the event!\n");
    XFlush (display);
    usleep (1);
    // Release
    event.type = ButtonRelease;
    if (XSendEvent (display, PointerWindow, True, ButtonReleaseMask, &event) == 0)
        fprintf (stderr, "Error to send the event!\n");
    XFlush (display);
    usleep (1);
}

void move_cursor(Display *display, int x, int y)
{
  XWarpPointer (display, None, None, 0,0,0,0, x, y);
  XFlush (display);
  usleep (1);
}
void coords (Display *display, int *x, int *y)
{
    XEvent event;
    XQueryPointer (display, DefaultRootWindow (display),
                 &event.xbutton.root, &event.xbutton.window,
                 &event.xbutton.x_root, &event.xbutton.y_root,
                 &event.xbutton.x, &event.xbutton.y,
                 &event.xbutton.state);
    *x = event.xbutton.x;
    *y = event.xbutton.y;
}
void move_to (Display *display, int x, int y)
{
    int cur_x, cur_y;
    coords (display, &cur_x, &cur_y);
    XWarpPointer (display, None, None, 0,0,0,0, -cur_x, -cur_y);
    XWarpPointer (display, None, None, 0,0,0,0, x, y);
    usleep (1);
}

#endif // XLIB_UTILS_H
