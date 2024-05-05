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
#include <wlr/util/log.h>
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

};

struct Output {
	wl_list link;
	Server *server;
	wlr_output *out;
	wl_listener frame;
	wl_listener destroy;
};

// output.cpp
void new_output(wl_listener *l, void *data);

