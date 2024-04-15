#pragma once
#ifndef WLR_USE_UNSTABLE
#define WLR_USE_UNSTABLE
#endif

#include <wayland-server-core.h>

extern "C" {
#include <wlr/backend.h>
#include <wlr/util/log.h>
}

struct Server {
	wl_display *display;
	wlr_backend *backend;
};

