#include "tessel.hpp"
#include <ctime>


static void output_destroy(wl_listener *l, void *data) {
	Output *o = wl_container_of(l, o, destroy);
	wl_list_remove(&o->frame.link);
	wl_list_remove(&o->destroy.link);
	delete o;
}
