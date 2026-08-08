#pragma once

#include <X11/XF86keysym.h>

/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
const int snap                       = 0;       /* snap pixel (exported for modules) */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
#define COORDINATES_STYLE "[x%d y%d]" /* style of the infinite-tags coordinates in the bar */

static char col_gray1[]             = "#222222"; /* norm bg */
static char col_gray2[]             = "#444444"; /* norm border */
static char col_gray3[]             = "#bbbbbb"; /* norm fg */
static char col_gray4[]             = "#eeeeee"; /* sel fg */
static char col_cyan[]              = "#005577"; /* sel bg/border */
/* MAYBE_CONST: with XRDB the scheme pointers must be writable so loadxrdb()
 * can rebuild them from the X resource database. */
static MAYBE_CONST char *colors[][3] = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* new floating windows appear centered on the selected monitor (floating
 * layout). Set NEW_FLOATING_WINDOWS_APPEAR_UNDER_CURSOR to 1 instead to
 * place them under the pointer. */
#define CENTER_NEW_FLOATING_WINDOWS 1
#define NEW_FLOATING_WINDOWS_APPEAR_UNDER_CURSOR 0

/* bar appearance */
#if BAR_HEIGHT
static const int user_bh = 30;            /* override the bar height in pixels */
#endif
#if BAR_PADDING
static const int top_vertpad = 0;         /* top vertical padding of the bar */
static const int bottom_vertpad = 2;      /* bottom vertical padding of the bar */
static const int left_sidepad = 450;      /* left side padding ("centered" bar) */
static const int right_sidepad = 450;     /* right side padding ("centered" bar) */
#endif

#define BAR_ALWAYS_ON_TOP 1 /* keep the internal bar above all windows */
#if EXTERNAL_BARS
#define EXTERNAL_BARS_ALWAYS_ON_TOP 1 /* keep external bars above all windows */
#endif
#if INFINITE_TAGS
#define PINNED_WINDOWS_ALWAYS_ON_TOP 1 /* pinned windows stay above others */
#endif

/* tagging */
const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

#if INFINITE_TAGS
#define MOVE_CANVAS_STEP 120 /* pixels per movecanvas() call */
#endif
#if INFINITE_TAGS && IT_SHOW_COORDINATES_IN_BAR
#define COORDINATES_DIVISOR 10 /* divide coords by this for display */
#endif
#if MOVE_RESIZE_WITH_KEYBOARD
#define MOVE_WITH_KEYBOARD_STEP   50
#define RESIZE_WITH_KEYBOARD_STEP 50
#endif

#if GAPS
static const int gappx = 5; /* gaps: default gap size in pixels */
#endif

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
const int refreshrate = 360;  /* move/resize refresh rate (exported for modules) */



static const Layout layouts[] = {
	/* symbol     arrange function */
	/* tiled is the default (first entry). Floating has no arrange function and
	 * is reached with Super+Shift+t; Super+space toggles current/previous. */
#if GAPS
	{ "[]=",      gaps_tile },
#else
	{ "[]=",      tile },
#endif
	{ "><>",      NULL },    /* no arrange function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTERNATE_MODKEY Mod1Mask

#define SCROLL_UP Button4
#define SCROLL_DOWN Button5

#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ Mod1Mask,                     KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "rofi", "-show", "drun", NULL };
static const char *termcmd[]  = { "kitty", NULL };
static const char *bravecmd[] = { "brave", NULL };
static const char *wallcmd[] = { "/bin/sh", "-c", "~/wallpaper.sh", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} }, /* swap focused to master */
	{ MODKEY,                       XK_0,      view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} }, /* close focused window */
	{ MODKEY,                       XK_t,      spawn,          {.v = termcmd } }, /* terminal */
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[1]} }, /* floating */
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[0]} }, /* tiled (default) */
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} }, /* monocle */
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_Tab,    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY,                       XK_e,      spawn, SHCMD("thunar") },

	/* Volume Control (PipeWire/wireplumber via wpctl) */
	{ 0, XF86XK_AudioRaiseVolume,   spawn, SHCMD("~/volume.sh i") },
	{ 0, XF86XK_AudioLowerVolume,   spawn, SHCMD("~/volume.sh d") },
	{ 0, XF86XK_AudioMute,          spawn, SHCMD("~/volume.sh t") },

	/* Brightness (brightnessctl) */
	{ 0, XF86XK_MonBrightnessUp,    spawn, SHCMD("~/brightness.sh +5%") },
	{ 0, XF86XK_MonBrightnessDown,  spawn, SHCMD("~/brightness.sh 5%-") },

	/* Launcher / apps */
	{ MODKEY,                       XK_w,      spawn,          {.v = bravecmd } },

	/* Wallpaper picker */
	{ MODKEY|ShiftMask,             XK_w,      spawn,          {.v = wallcmd } },

	/* Screenshot */
	{ 0,                            XK_Print,  spawn,          SHCMD("~/screenshot.sh") },

#if XRDB
	{ MODKEY,                       XK_F5,     loadxrdb,       {0} },
#endif
#if FULLSCREEN
	{ MODKEY|ShiftMask,             XK_f,      fullscreen,     {0} },
#endif
#if ENHANCED_TOGGLE_FLOATING
	/* enhanced toggle floating (togglefloat) is compiled but unbound: Super+q
	 * now closes the focused window. Re-bind it here to a free key if wanted. */
#endif
#if GAPS
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
#endif
#if MOVE_RESIZE_WITH_KEYBOARD
	{ MODKEY,                       XK_Down,   moveresize,     {.v = (int []){ 0,  MOVE_WITH_KEYBOARD_STEP, 0, 0 } } }, /* move down */
	{ MODKEY,                       XK_Up,     moveresize,     {.v = (int []){ 0, -MOVE_WITH_KEYBOARD_STEP, 0, 0 } } }, /* move up */
	{ MODKEY,                       XK_Right,  moveresize,     {.v = (int []){ MOVE_WITH_KEYBOARD_STEP, 0, 0, 0 } } }, /* move right */
	{ MODKEY,                       XK_Left,   moveresize,     {.v = (int []){ -MOVE_WITH_KEYBOARD_STEP, 0, 0, 0 } } }, /* move left */
	{ MODKEY|ControlMask,           XK_Down,   moveresize,     {.v = (int []){ 0, 0, 0,  RESIZE_WITH_KEYBOARD_STEP } } }, /* resize taller */
	{ MODKEY|ControlMask,           XK_Up,     moveresize,     {.v = (int []){ 0, 0, 0, -RESIZE_WITH_KEYBOARD_STEP } } }, /* resize shorter */
	{ MODKEY|ControlMask,           XK_Right,  moveresize,     {.v = (int []){ 0, 0, RESIZE_WITH_KEYBOARD_STEP, 0 } } }, /* resize wider */
	{ MODKEY|ControlMask,           XK_Left,   moveresize,     {.v = (int []){ 0, 0, -RESIZE_WITH_KEYBOARD_STEP, 0 } } }, /* resize narrower */
#endif
#if INFINITE_TAGS
	{ MODKEY,                       XK_r,      homecanvas,     {0} }, /* return to x:0 y:0 */
	{ MODKEY|ShiftMask,             XK_Left,   movecanvas,     {.i = 0} }, /* canvas left */
	{ MODKEY|ShiftMask,             XK_Right,  movecanvas,     {.i = 1} }, /* canvas right */
	{ MODKEY|ShiftMask,             XK_Up,     movecanvas,     {.i = 2} }, /* canvas up */
	{ MODKEY|ShiftMask,             XK_Down,   movecanvas,     {.i = 3} }, /* canvas down */
	{ MODKEY|ShiftMask,             XK_d,      centerwindow,   {0} },
	{ MODKEY|ControlMask,           XK_z,      pinwindow,      {0} },
#endif
#if DIRECTIONAL_FOCUS
	{ ALTERNATE_MODKEY,             XK_Left,   focusdir,       {.i = 0} }, /* focus left */
	{ ALTERNATE_MODKEY,             XK_Right,  focusdir,       {.i = 1} }, /* focus right */
	{ ALTERNATE_MODKEY,             XK_Up,     focusdir,       {.i = 2} }, /* focus up */
	{ ALTERNATE_MODKEY,             XK_Down,   focusdir,       {.i = 3} }, /* focus down */
#endif
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
#if INFINITE_TAGS
	{ ClkRootWin,           MODKEY|ShiftMask, Button1,        movecanvasmouse, {.f = 1.5 } },
	{ ClkClientWin,         MODKEY|ShiftMask, Button1,        movecanvasmouse, {.f = 1.5 } },
	{ ClkRootWin,           0,                Button1,        movecanvasmouse, {.f = 1.5 } },
	/* .f = drag multiplier (0.5 = half speed, 2 = double speed); negative
	 * inverts the canvas direction */
#endif
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
#if BETTER_RESIZE
	{ ClkClientWin,         MODKEY,         Button3,        betterresize_resizemouse, {0} },
#else
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
#endif
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
