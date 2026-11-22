#include <poor_fan.h>

poor_fan_declare(frame_fan);

struct screen {
	frame_fan fan;
	alignas(64) struct poor_fan_ref *refs;
	alignas(64) uint32_t published;
};

uint32_t published(const struct screen *screen);

uint32_t published(const struct screen *screen) {
	return poor_fan_published(&screen->fan);
}
