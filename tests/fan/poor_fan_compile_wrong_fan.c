#include <poor_fan.h>

poor_fan_declare(frame_fan);
poor_fan_declare(other_fan);

struct screen {
	other_fan fan;
	alignas(64) struct poor_fan_ref *refs;
	alignas(64) uint32_t published;
};

poor_fan_attach(frame_fan, struct screen, fan, refs, published);
