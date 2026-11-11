#include "x11.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
static void type(quintptr id,const char *kind){Display*d=XOpenDisplay(nullptr);if(!d)return;Atom value=XInternAtom(d,kind,False);XChangeProperty(d,id,XInternAtom(d,"_NET_WM_WINDOW_TYPE",False),XA_ATOM,32,PropModeReplace,reinterpret_cast<unsigned char*>(&value),1);XFlush(d);XCloseDisplay(d);}
void desktopWindow(quintptr id){type(id,"_NET_WM_WINDOW_TYPE_DESKTOP");}
void dockWindow(quintptr id,int height,int width,int y){type(id,"_NET_WM_WINDOW_TYPE_DOCK");Display*d=XOpenDisplay(nullptr);if(!d)return;unsigned long strut[12]={0,0,0,static_cast<unsigned long>(height),0,0,0,0,0,0,0,static_cast<unsigned long>(width-1)};XChangeProperty(d,id,XInternAtom(d,"_NET_WM_STRUT_PARTIAL",False),XA_CARDINAL,32,PropModeReplace,reinterpret_cast<unsigned char*>(strut),12);XChangeProperty(d,id,XInternAtom(d,"_NET_WM_STRUT",False),XA_CARDINAL,32,PropModeReplace,reinterpret_cast<unsigned char*>(strut),4);(void)y;XFlush(d);XCloseDisplay(d);}
