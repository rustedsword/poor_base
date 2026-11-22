#include <poor_fan.h>

poor_fan_declare(frame_fan);

struct screen {
	frame_fan fan;
	alignas(64) struct poor_fan_ref *refs;
	alignas(64) uint32_t published;
};

poor_fan_attach(frame_fan, struct screen, fan, refs, published);

void init(struct screen *screen);

void init(struct screen *screen) {
	static struct poor_fan_ref refs[POOR_FAN_MAX_SLOTS + 1];
	poor_fan_init(&screen->fan, &refs);
}
