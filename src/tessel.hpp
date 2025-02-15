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
#include <wlr/util/log.h>
#include <wlr/util/box.h>
#include <wlr/util/edges.h>
#include <xkbcommon/xkbcommon.h>
}

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
};

struct Keyboard {
	wl_list link;
	Server *server;
	wlr_keyboard *kbd;
	wl_listener modifiers;
	wl_listener key;
	wl_listener destroy;
};

// view.cpp
void new_xdg_toplevel(wl_listener *l, void *data);

// output.cpp
void new_output(wl_listener *l, void *data);

// input.cpp
void new_input(wl_listener *l, void *data);
void request_set_selection(wl_listener *l, void *data);
