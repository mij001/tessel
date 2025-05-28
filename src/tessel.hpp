#pragma once
#ifndef WLR_USE_UNSTABLE
#define WLR_USE_UNSTABLE
#endif

#include <wayland-server-core.h>

extern "C" {
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/util/log.h>
#include <wlr/util/box.h>
#include <wlr/util/edges.h>
#include <xkbcommon/xkbcommon.h>
}

struct View;

struct Server {
	wl_display *display;
	wlr_backend *backend;
	wlr_renderer *renderer;
	wlr_allocator *allocator;
	wlr_output_layout *output_layout;
	wlr_scene *scene;
	wlr_scene_output_layout *scene_layout;
	wl_listener new_output;
	wl_list outputs;

	wlr_xdg_shell *xdg_shell;
	wl_listener new_xdg_toplevel;
	wl_list views;

	wlr_seat *seat;
	wl_listener new_input;
	wl_listener request_set_selection;
	wl_list keyboards;

	wlr_cursor *cursor;
	wlr_xcursor_manager *cursor_mgr;
	wl_listener cursor_motion;
	wl_listener cursor_motion_absolute;
	wl_listener cursor_button;
	wl_listener cursor_axis;
	wl_listener cursor_frame;
	double master_ratio;
	int gap;

	enum { PASSTHROUGH, MOVE, RESIZE } cursor_mode;
	View *grabbed;
	double grab_x, grab_y;
	wlr_box grab_geo;
	uint32_t resize_edges;
};

struct Output {
	wl_list link;
	Server *server;
	wlr_output *out;
	wl_listener frame;
	wl_listener destroy;
};

struct View {
	wl_list link;
	Server *server;
	wlr_xdg_toplevel *toplevel;
	wlr_scene_tree *tree;
	wl_listener map;
	wl_listener unmap;
	wl_listener commit;
	wl_listener destroy;
	wlr_box geo;
	bool floating;
	bool fullscreen;
};

struct Keyboard {
	wl_list link;
	Server *server;
	wlr_keyboard *kbd;
	wl_listener modifiers;
	wl_listener key;
	wl_listener destroy;
};

// tiling.cpp
void arrange(Server *s);

// view.cpp
void focus_view(View *v);
View *first_view(Server *s);
View *view_at(Server *s, double lx, double ly, wlr_surface **surface,
	double *sx, double *sy);
void new_xdg_toplevel(wl_listener *l, void *data);

// output.cpp
void new_output(wl_listener *l, void *data);

// input.cpp
void new_input(wl_listener *l, void *data);
void request_set_selection(wl_listener *l, void *data);
void cursor_motion(wl_listener *l, void *data);
void cursor_motion_absolute(wl_listener *l, void *data);
void cursor_button(wl_listener *l, void *data);
