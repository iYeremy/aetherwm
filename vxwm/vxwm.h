/* See LICENSE file for copyright and license details.
 *
 * vxwm.h - public interface of the vxwm core.
 *
 * This header is the explicit API that the core (vxwm.c), drw and every module
 * compile against. It holds the shared types (Client, Monitor, ...), the
 * macros, the core globals exported to modules and the core function
 * prototypes. Modules never `#include "vxwm.c"`; they use only what is
 * declared here.
 *
 * Compile-time feature switches live in modules/modules.h (included below),
 * which is why Client and Monitor can carry module-owned fields guarded by the
 * module macros.
 */
#ifndef VXWM_H
#define VXWM_H

#include <stddef.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif /* XINERAMA */
#include <X11/Xft/Xft.h>

#include "drw.h"
#include "util.h"

#include "modules/modules.h"

/* A bit less sucky config: with XRDB the color arrays must be mutable so
 * loadxrdb() can overwrite them. */
#if !XRDB
#define MAYBE_CONST const
#else
#define MAYBE_CONST
#endif

/* macros */
#define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)
#define CLEANMASK(mask)         (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define INTERSECT(x,y,w,h,m)    (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
                               * MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))
#define ISVISIBLE(C)            ((C->tags & C->mon->tagset[C->mon->seltags]))
#define MOUSEMASK               (BUTTONMASK|PointerMotionMask)
#define WIDTH(X)                ((X)->w + 2 * (X)->bw)
#define HEIGHT(X)               ((X)->h + 2 * (X)->bw)
#define TAGMASK                 ((1 << LENGTH(tags)) - 1)
#define TEXTW(X)                (drw_fontset_getwidth(drw, (X)) + lrpad)

/* enums */
enum { CurNormal, CurResize, CurMove,
#if BETTER_RESIZE && BR_CHANGE_CURSOR
	CurNW, CurNE, CurSW, CurSE, /* corner cursors */
	CurN, CurS, CurE, CurW,     /* edge cursors */
#endif
	CurLast }; /* cursor */
enum { SchemeNorm, SchemeSel }; /* color schemes */
enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
       NetWMFullscreen, NetActiveWindow, NetWMWindowType,
       NetWMWindowTypeDialog, NetClientList,
#if EWMH_TAGS
       NetDesktopViewport, NetNumberOfDesktops, NetCurrentDesktop,
       NetDesktopNames, NetDesktopNum,
#endif
#if EXTERNAL_BARS
       NetWMStrut, NetWMStrutPartial,
#endif
       NetLast }; /* EWMH atoms */
enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast }; /* default atoms */
enum { ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
       ClkClientWin, ClkRootWin, ClkLast }; /* clicks */

typedef union {
	int i;
	unsigned int ui;
	float f;
	const void *v;
} Arg;

typedef struct {
	unsigned int click;
	unsigned int mask;
	unsigned int button;
	void (*func)(const Arg *arg);
	const Arg arg;
} Button;

typedef struct Monitor Monitor;
typedef struct Client Client;
struct Client {
	char name[256];
	float mina, maxa;
	int x, y, w, h;
	int oldx, oldy, oldw, oldh;
	int basew, baseh, incw, inch, maxw, maxh, minw, minh, hintsvalid;
	int bw, oldbw;
	unsigned int tags;
	int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen;
	Client *next;
	Client *snext;
	Monitor *mon;
	Window win;
#if WINDOWMAP
	int ismapped; /* windowmap: whether the window is currently mapped */
#endif
#if INFINITE_TAGS
	int saved_cx, saved_cy; /* infinitetags: canvas-absolute position */
	int saved_cw, saved_ch; /* infinitetags: saved size */
	int was_on_canvas;      /* infinitetags: position was saved for this tag */
	int is_pinned;          /* infinitetags: pinned windows do not move with the canvas */
#endif
#if ENHANCED_TOGGLE_FLOATING
	int sfx, sfy, sfw, sfh; /* etf: last floating size/position */
	int wasfloating;        /* etf: client is floating because of a toggle */
	int nfixed;             /* etf: client has fixed size hints */
#if RESTORE_SIZE_AND_POS_ETF
	int wasmanuallyedited;  /* etf: user moved/resized since last toggle */
#endif
#endif
#if BSP_LAYOUT
	struct BspNode *bspnode; /* bsp: leaf node of this client in the split tree */
#endif
};

typedef struct {
	unsigned int mod;
	KeySym keysym;
	void (*func)(const Arg *);
	const Arg arg;
} Key;

typedef struct {
	const char *symbol;
	void (*arrange)(Monitor *);
} Layout;

#if INFINITE_TAGS
typedef struct {
	int cx, cy;        /* current viewport offset for a tag */
	int saved_cx, saved_cy; /* offset saved while the layout was tiled */
} CanvasOffset;
#endif

struct Monitor {
	char ltsymbol[16];
	float mfact;
	int nmaster;
	int num;
	int by;               /* bar geometry */
	int mx, my, mw, mh;   /* screen size */
	int wx, wy, ww, wh;   /* window area  */
	unsigned int seltags;
	unsigned int sellt;
	unsigned int tagset[2];
	int showbar;
	int topbar;
	Client *clients;
	Client *sel;
	Client *stack;
	Monitor *next;
	Window barwin;
	const Layout *lt[2];
#if INFINITE_TAGS
	CanvasOffset *canvas; /* infinitetags: viewport offset per tag */
#endif
#if GAPS
	int gappx; /* gaps: outer/inner gap size in pixels */
#endif
#if EXTERNAL_BARS
	int strut_top, strut_bottom, strut_left, strut_right; /* externalbars */
#endif
#if BSP_LAYOUT
	struct BspNode *bsproot; /* bsp: root of the binary split tree */
#endif
};

typedef struct {
	const char *class;
	const char *instance;
	const char *title;
	unsigned int tags;
	int isfloating;
	int monitor;
} Rule;

/* core globals shared with modules */
extern Display *dpy;
extern Window root;
extern Monitor *mons, *selmon;
extern Atom wmatom[WMLast], netatom[NetLast];
extern Clr **scheme;
extern Drw *drw;
extern Cur *cursor[CurLast];
extern int sw, sh;        /* X display screen geometry width, height */
extern const char *tags[]; /* tag names (defined in config.h) */
extern size_t ntags;       /* number of tags */
extern const int refreshrate; /* move/resize rate limit (defined in config.h) */
extern const int snap;        /* snap pixel (defined in config.h) */

/* core API (used by config.h keybinds and by modules) */
void applyrules(Client *c);
int applysizehints(Client *c, int *x, int *y, int *w, int *h, int interact);
void arrange(Monitor *m);
void arrangemon(Monitor *m);
void attach(Client *c);
void attachstack(Client *c);
void buttonpress(XEvent *e);
void checkotherwm(void);
void cleanup(void);
void cleanupmon(Monitor *mon);
void clientmessage(XEvent *e);
void configure(Client *c);
void configurenotify(XEvent *e);
void configurerequest(XEvent *e);
Monitor *createmon(void);
void destroynotify(XEvent *e);
void detach(Client *c);
void detachstack(Client *c);
Monitor *dirtomon(int dir);
void drawbar(Monitor *m);
void drawbars(void);
void enternotify(XEvent *e);
void expose(XEvent *e);
void focus(Client *c);
void focusin(XEvent *e);
void focusmon(const Arg *arg);
void focusstack(const Arg *arg);
Atom getatomprop(Client *c, Atom prop);
int getrootptr(int *x, int *y);
long getstate(Window w);
int gettextprop(Window w, Atom atom, char *text, unsigned int size);
void grabbuttons(Client *c, int focused);
void grabkeys(void);
void incnmaster(const Arg *arg);
void keypress(XEvent *e);
void killclient(const Arg *arg);
void manage(Window w, XWindowAttributes *wa);
void mappingnotify(XEvent *e);
void maprequest(XEvent *e);
void monocle(Monitor *m);
void motionnotify(XEvent *e);
void movemouse(const Arg *arg);
Client *nexttiled(Client *c);
void pop(Client *c);
void swaptile(Client *a, Client *b);
void propertynotify(XEvent *e);
void quit(const Arg *arg);
Monitor *recttomon(int x, int y, int w, int h);
void resize(Client *c, int x, int y, int w, int h, int interact);
void resizeclient(Client *c, int x, int y, int w, int h);
void resizemouse(const Arg *arg);
void restack(Monitor *m);
void run(void);
void scan(void);
int sendevent(Client *c, Atom proto);
void sendmon(Client *c, Monitor *m);
void setclientstate(Client *c, long state);
void setfocus(Client *c);
void setfullscreen(Client *c, int fullscreen);
void setlayout(const Arg *arg);
void setmfact(const Arg *arg);
void setup(void);
void seturgent(Client *c, int urg);
void showhide(Client *c);
void spawn(const Arg *arg);
void tag(const Arg *arg);
void tagmon(const Arg *arg);
void tile(Monitor *m);
void togglebar(const Arg *arg);
void togglefloating(const Arg *arg);
void toggletag(const Arg *arg);
void toggleview(const Arg *arg);
void unfocus(Client *c, int setfocus);
void unmanage(Client *c, int destroyed);
void unmapnotify(XEvent *e);
void updatebarpos(Monitor *m);
void updatebars(void);
void updateclientlist(void);
int updategeom(void);
void updatenumlockmask(void);
void updatesizehints(Client *c);
void updatestatus(void);
void updatetitle(Client *c);
void updatewindowtype(Client *c);
void updatewmhints(Client *c);
void view(const Arg *arg);
Client *wintoclient(Window w);
Monitor *wintomon(Window w);
int xerror(Display *dpy, XErrorEvent *ee);
int xerrordummy(Display *dpy, XErrorEvent *ee);
int xerrorstart(Display *dpy, XErrorEvent *ee);
void zoom(const Arg *arg);

#endif /* VXWM_H */
