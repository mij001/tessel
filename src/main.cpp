#include "tessel.hpp"
#include <cstdlib>

int main(void) {
	wlr_log_init(WLR_INFO, NULL);

	Server s{};
	s.display = wl_display_create();
	wl_event_loop *loop = wl_display_get_event_loop(s.display);

	s.backend = wlr_backend_autocreate(loop, NULL);
	if (!s.backend) {
		wlr_log(WLR_ERROR, "no backend");
		return 1;
	}

	wlr_backend_destroy(s.backend);
	wl_display_destroy(s.display);
	return 0;
}
