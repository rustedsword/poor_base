#include <poor_fan.h>
#include <poor_asan.h>
#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>

constexpr uint32_t SMALL_SLOTS = 4;
constexpr uint64_t PIN_ONE = UINT64_C(1) << 32;

poor_fan_declare(small_fan);

struct small {
	small_fan fan;
	alignas(64) struct poor_fan_ref *refs;
	alignas(64) uint32_t published;
};

poor_fan_attach(small_fan, struct small, fan, refs, published);

#define SMALL_INIT(array) { .fan = POOR_FAN_INIT, .refs = (array), .published = 0 }

static uint64_t count_of(const struct poor_fan_ref *refs, uint32_t slot) {
	return atomic_load(&refs[slot].count);
}

static uint32_t small_pick_unused(const struct small *small) {
	for(uint32_t slot = 0; slot < SMALL_SLOTS; slot++)
		if(poor_fan_unused(&small->fan, slot))
			return slot;
	return POOR_FAN_NO_SLOT;
}

static uint64_t rng_next(uint64_t *state) {
	uint64_t x = *state;
	x ^= x >> 12;
	x ^= x << 25;
	x ^= x >> 27;
	*state = x;
	return x * UINT64_C(0x2545f4914f6cdd1d);
}

static int fan_init_test(void) {
	static struct poor_fan_ref refs[SMALL_SLOTS];
	static struct small small = SMALL_INIT(refs);
	struct poor_fan_hold hold = POOR_FAN_HOLD_INIT;
	struct poor_fan_pin pin = POOR_FAN_PIN_INIT;

	static_assert(sizeof(struct poor_fan_ref) == 64 && sizeof(small_fan) == 64 && alignof(small_fan) == 64);
	static_assert(sizeof(struct small) == 192);
	static_assert(_Generic(poor_fan_latest(&small.fan, &hold), uint32_t: 1, default: 0));
	static_assert(_Generic(poor_fan_published(&small.fan), uint32_t: 1, default: 0));
	static_assert(_Generic(poor_fan_unused(&small.fan, 0), bool: 1, default: 0));
	static_assert(_Generic(poor_fan_held(&small.fan, 0), bool: 1, default: 0));

	assert(small.refs == refs && small.published == 0);
	assert(poor_fan_published(&small.fan) == 0 && !poor_fan_unused(&small.fan, 0) && poor_fan_held(&small.fan, 0));
	for(uint32_t slot = 1; slot < SMALL_SLOTS; slot++)
		assert(!poor_fan_held(&small.fan, slot) && poor_fan_unused(&small.fan, slot));
	assert(small_pick_unused(&small) == 1);

	assert(hold.slot == POOR_FAN_NO_SLOT && pin.slot == POOR_FAN_NO_SLOT);
	poor_fan_release(&small.fan, &hold);
	poor_fan_unpin(&small.fan, &pin);
	poor_fan_pin(&small.fan, &hold, &pin);
	assert(hold.slot == POOR_FAN_NO_SLOT && pin.slot == POOR_FAN_NO_SLOT);
	assert(poor_fan_latest(&small.fan, &hold) == 0 && hold.slot == 0);
	assert(count_of(refs, 0) == 0 && poor_fan_held(&small.fan, 0));
	poor_fan_release(&small.fan, &hold);
	assert(hold.slot == POOR_FAN_NO_SLOT && poor_fan_held(&small.fan, 0));
	poor_fan_publish(&small.fan, 0);
	assert(count_of(refs, 0) == 0);

	struct poor_fan_ref dirty[SMALL_SLOTS];
	struct small again = { .fan = { .word = 77 }, .refs = nullptr, .published = 3 };
	int fan_evals = 0, refs_evals = 0;
	for(uint32_t slot = 0; slot < SMALL_SLOTS; slot++)
		atomic_init(&dirty[slot].count, UINT64_MAX);

	if(fan_evals == 0)
		poor_fan_init((fan_evals++, &again.fan), (refs_evals++, &dirty));
	else
		return 1;
	assert(fan_evals == 1 && refs_evals == 1);
	assert(atomic_load(&again.fan.word) == 0 && again.refs == dirty && again.published == 0);
	assert(poor_fan_published(&again.fan) == 0);
	for(uint32_t slot = 0; slot < SMALL_SLOTS; slot++)
		assert(count_of(dirty, slot) == 0 && poor_fan_unused(&again.fan, slot) == (slot != 0)
		       && poor_fan_held(&again.fan, slot) == (slot == 0));

	size_t some = SMALL_SLOTS - 1;
	struct poor_fan_ref (*view)[some] = (void *)dirty;
	for(uint32_t slot = 0; slot < SMALL_SLOTS; slot++)
		atomic_store(&dirty[slot].count, UINT64_MAX);
	poor_fan_init(&again.fan, view);
	assert(again.refs == dirty);
	for(uint32_t slot = 0; slot < SMALL_SLOTS; slot++)
		assert(count_of(dirty, slot) == (slot < some ? 0 : UINT64_MAX));
	return 0;
}

static int fan_holds_test(void) {
	static struct poor_fan_ref refs[SMALL_SLOTS];
	static struct small small = SMALL_INIT(refs);
	struct poor_fan_hold a = POOR_FAN_HOLD_INIT, b = POOR_FAN_HOLD_INIT, c = POOR_FAN_HOLD_INIT;

	assert(poor_fan_latest(&small.fan, &a) == 0);
	assert(poor_fan_latest(&small.fan, &b) == 0);
	assert(poor_fan_latest(&small.fan, &a) == 0);
	poor_fan_publish(&small.fan, 1);
	assert(poor_fan_published(&small.fan) == 1 && count_of(refs, 0) == 2);
	assert(poor_fan_held(&small.fan, 0) && !poor_fan_unused(&small.fan, 0));
	assert(small_pick_unused(&small) == 2);

	assert(poor_fan_latest(&small.fan, &a) == 1);
	assert(poor_fan_held(&small.fan, 0));
	poor_fan_release(&small.fan, &b);
	assert(!poor_fan_held(&small.fan, 0) && poor_fan_unused(&small.fan, 0));

	assert(poor_fan_latest(&small.fan, &c) == 1);
	poor_fan_release(&small.fan, &c);
	assert(poor_fan_latest(&small.fan, &c) == 1);
	poor_fan_release(&small.fan, &c);
	assert(!poor_fan_unused(&small.fan, 1));
	poor_fan_publish(&small.fan, 2);
	assert(count_of(refs, 1) == 1 && poor_fan_held(&small.fan, 1));
	poor_fan_release(&small.fan, &a);
	assert(!poor_fan_held(&small.fan, 1) && poor_fan_unused(&small.fan, 1));

	assert(poor_fan_latest(&small.fan, &c) == 2);
	poor_fan_publish(&small.fan, 2);
	assert(poor_fan_published(&small.fan) == 2 && count_of(refs, 2) == 1);
	assert(poor_fan_latest(&small.fan, &c) == 2);
	poor_fan_publish(&small.fan, 3);
	assert(count_of(refs, 2) == 1 && poor_fan_held(&small.fan, 2) && !poor_fan_unused(&small.fan, 2));
	poor_fan_release(&small.fan, &c);
	assert(poor_fan_unused(&small.fan, 2));
	return 0;
}

static int fan_pins_test(void) {
	static struct poor_fan_ref refs[SMALL_SLOTS];
	static struct small small = SMALL_INIT(refs);
	struct poor_fan_hold hold = POOR_FAN_HOLD_INIT;
	struct poor_fan_pin pin = POOR_FAN_PIN_INIT, other = POOR_FAN_PIN_INIT;

	assert(poor_fan_latest(&small.fan, &hold) == 0);
	poor_fan_pin(&small.fan, &hold, &pin);
	poor_fan_pin(&small.fan, &hold, &pin);
	assert(pin.slot == 0 && hold.slot == 0);
	poor_fan_publish(&small.fan, 1);
	assert(count_of(refs, 0) == PIN_ONE + 1);
	poor_fan_release(&small.fan, &hold);
	assert(count_of(refs, 0) == PIN_ONE);
	assert(!poor_fan_held(&small.fan, 0) && !poor_fan_unused(&small.fan, 0));
	assert(small_pick_unused(&small) == 2);

	assert(poor_fan_latest(&small.fan, &hold) == 1);
	poor_fan_pin(&small.fan, &hold, &other);
	poor_fan_pin(&small.fan, &hold, &pin);
	assert(pin.slot == 1 && other.slot == 1 && poor_fan_unused(&small.fan, 0));
	poor_fan_release(&small.fan, &hold);
	poor_fan_publish(&small.fan, 2);
	assert(count_of(refs, 1) == 2 * PIN_ONE);
	assert(!poor_fan_held(&small.fan, 1) && !poor_fan_unused(&small.fan, 1));
	poor_fan_unpin(&small.fan, &pin);
	assert(pin.slot == POOR_FAN_NO_SLOT && !poor_fan_unused(&small.fan, 1));
	poor_fan_pin(&small.fan, &hold, &other);
	assert(other.slot == POOR_FAN_NO_SLOT && poor_fan_unused(&small.fan, 1));
	poor_fan_unpin(&small.fan, &other);
	assert(other.slot == POOR_FAN_NO_SLOT);
	return 0;
}

static int fan_last_slot_test(void) {
	constexpr uint32_t LAST = POOR_FAN_MAX_SLOTS - 1;
	constexpr unsigned HOLDS = 70000;
	static struct poor_fan_ref refs[POOR_FAN_MAX_SLOTS];
	static struct small small = SMALL_INIT(refs);
	struct poor_fan_hold hold = POOR_FAN_HOLD_INIT;
	struct poor_fan_pin pin = POOR_FAN_PIN_INIT;

	assert(poor_fan_unused(&small.fan, LAST));
	poor_fan_publish(&small.fan, LAST);
	assert(poor_fan_published(&small.fan) == LAST && poor_fan_unused(&small.fan, 0));
	for(unsigned i = 0; i < HOLDS; i++) {
		assert(poor_fan_latest(&small.fan, &hold) == LAST);
		poor_fan_release(&small.fan, &hold);
	}
	assert(poor_fan_latest(&small.fan, &hold) == LAST && hold.slot == LAST);
	poor_fan_pin(&small.fan, &hold, &pin);
	poor_fan_publish(&small.fan, 0);
	assert(count_of(refs, LAST) == PIN_ONE + 1 && count_of(refs, 0) == 0);
	assert(poor_fan_held(&small.fan, LAST));
	assert(poor_fan_latest(&small.fan, &hold) == 0);
	assert(!poor_fan_held(&small.fan, LAST) && !poor_fan_unused(&small.fan, LAST));
	poor_fan_unpin(&small.fan, &pin);
	assert(poor_fan_unused(&small.fan, LAST) && poor_fan_unused(&small.fan, LAST - 1));
	poor_fan_release(&small.fan, &hold);

	struct small again;
	poor_fan_init(&again.fan, &refs);
	assert(again.refs == refs && poor_fan_unused(&again.fan, LAST) && count_of(refs, 0) == 0);
	return 0;
}

constexpr unsigned HISTORY_HOLDS = 3;
constexpr unsigned HISTORY_PINS = 3;

poor_fan_declare(history_fan);

struct history {
	struct poor_fan_ref refs[SMALL_SLOTS];
	struct {
		struct poor_fan_ref *refs;
		alignas(64) uint32_t published;
	} attached;
	history_fan fan;
	struct poor_fan_hold holds[HISTORY_HOLDS];
	struct poor_fan_pin pins[HISTORY_PINS];
	uint32_t held[SMALL_SLOTS];
	uint32_t pinned[SMALL_SLOTS];
	uint32_t published;
};

poor_fan_attach(history_fan, struct history, fan, attached.refs, attached.published);

static bool history_held(const struct history *h, uint32_t slot) {
	return slot == h->published || h->held[slot] != 0;
}

static bool history_unused(const struct history *h, uint32_t slot) {
	return !history_held(h, slot) && h->pinned[slot] == 0;
}

static void history_check(const struct history *h) {
	assert(poor_fan_published(&h->fan) == h->published && h->attached.published == h->published);
	assert(h->attached.refs == h->refs);
	for(uint32_t slot = 0; slot < SMALL_SLOTS; slot++) {
		assert(poor_fan_held(&h->fan, slot) == history_held(h, slot));
		assert(poor_fan_unused(&h->fan, slot) == history_unused(h, slot));
		assert(slot == h->published || count_of(h->refs, slot) == h->held[slot] + h->pinned[slot] * PIN_ONE);
	}
}

static void history_latest(struct history *h, unsigned index) {
	struct poor_fan_hold *hold = &h->holds[index];
	const uint32_t before = hold->slot;

	assert(poor_fan_latest(&h->fan, hold) == h->published && hold->slot == h->published);
	if(before == h->published)
		return;
	if(before != POOR_FAN_NO_SLOT)
		h->held[before]--;
	h->held[h->published]++;
}

static void history_release(struct history *h, unsigned index) {
	struct poor_fan_hold *hold = &h->holds[index];

	if(hold->slot != POOR_FAN_NO_SLOT)
		h->held[hold->slot]--;
	poor_fan_release(&h->fan, hold);
	assert(hold->slot == POOR_FAN_NO_SLOT);
}

static void history_pin(struct history *h, unsigned hold_index, unsigned pin_index) {
	const struct poor_fan_hold *hold = &h->holds[hold_index];
	struct poor_fan_pin *pin = &h->pins[pin_index];
	const uint32_t from = pin->slot;

	poor_fan_pin(&h->fan, hold, pin);
	assert(pin->slot == hold->slot);
	if(from == hold->slot)
		return;
	if(hold->slot != POOR_FAN_NO_SLOT)
		h->pinned[hold->slot]++;
	if(from != POOR_FAN_NO_SLOT)
		h->pinned[from]--;
}

static void history_unpin(struct history *h, unsigned index) {
	struct poor_fan_pin *pin = &h->pins[index];

	if(pin->slot != POOR_FAN_NO_SLOT)
		h->pinned[pin->slot]--;
	poor_fan_unpin(&h->fan, pin);
	assert(pin->slot == POOR_FAN_NO_SLOT);
}

static bool history_publish(struct history *h, bool again) {
	uint32_t slot = h->published;

	if(!again) {
		bool any = false;
		for(uint32_t candidate = 0; candidate < SMALL_SLOTS; candidate++)
			any = any || history_unused(h, candidate);
		slot = POOR_FAN_NO_SLOT;
		for(uint32_t candidate = SMALL_SLOTS; candidate-- != 0;)
			if(poor_fan_unused(&h->fan, candidate))
				slot = candidate;
		assert((slot != POOR_FAN_NO_SLOT) == any);
		if(slot == POOR_FAN_NO_SLOT)
			return false;
		assert(history_unused(h, slot));
	}
	poor_fan_publish(&h->fan, slot);
	h->published = slot;
	return true;
}

static int fan_histories_test(void) {
	constexpr unsigned HISTORIES = 500;
	constexpr unsigned STEPS = 2000;
	static struct history h;
	uint64_t rng = UINT64_C(0x9e3779b97f4a7c15);
	uint64_t publishes = 0, stuck = 0;

	for(unsigned history = 0; history < HISTORIES; history++) {
		poor_fan_init(&h.fan, &h.refs);
		h.published = 0;
		for(uint32_t slot = 0; slot < SMALL_SLOTS; slot++)
			h.held[slot] = h.pinned[slot] = 0;
		for(unsigned i = 0; i < HISTORY_HOLDS; i++)
			h.holds[i] = (struct poor_fan_hold)POOR_FAN_HOLD_INIT;
		for(unsigned i = 0; i < HISTORY_PINS; i++)
			h.pins[i] = (struct poor_fan_pin)POOR_FAN_PIN_INIT;

		for(unsigned step = 0; step < STEPS; step++) {
			const uint64_t random = rng_next(&rng);
			const unsigned hold = (unsigned)((random >> 8) % HISTORY_HOLDS);
			const unsigned pin = (unsigned)((random >> 16) % HISTORY_PINS);

			switch(random % 10) {
			case 0:
			case 1:
			case 2:
				history_latest(&h, hold);
				break;
			case 3:
			case 4:
				history_release(&h, hold);
				break;
			case 5:
				history_pin(&h, hold, pin);
				break;
			case 6:
				history_unpin(&h, pin);
				break;
			case 7:
				history_publish(&h, true);
				break;
			default:
				if(history_publish(&h, false))
					publishes++;
				else
					stuck++;
				break;
			}
			history_check(&h);
		}
		for(unsigned i = 0; i < HISTORY_HOLDS; i++)
			history_release(&h, i);
		for(unsigned i = 0; i < HISTORY_PINS; i++)
			history_unpin(&h, i);
		assert(history_publish(&h, false));
		history_check(&h);
		for(uint32_t slot = 0; slot < SMALL_SLOTS; slot++)
			assert(slot == h.published || poor_fan_unused(&h.fan, slot));
	}
	assert(publishes > 0 && stuck > 0);
	return 0;
}

poor_fan_declare(left_fan);
poor_fan_declare(right_fan);

struct pair {
	struct poor_fan_ref *left_refs;
	left_fan left;
	uint32_t left_published;
	right_fan right;
	uint32_t right_published;
	alignas(64) struct poor_fan_ref *right_refs;
};

poor_fan_attach(left_fan, struct pair, left, left_refs, left_published);
poor_fan_attach(right_fan, struct pair, right, right_refs, right_published);

static int fan_layout_test(void) {
	static struct poor_fan_ref left_refs[SMALL_SLOTS], right_refs[SMALL_SLOTS];
	static struct pair pairs[2];
	struct poor_fan_hold left = POOR_FAN_HOLD_INIT, right = POOR_FAN_HOLD_INIT;
	struct poor_fan_pin pin = POOR_FAN_PIN_INIT;
	struct pair *pair = &pairs[1];
	const struct pair *view = pair;

	static_assert(sizeof(left_fan) == 64 && sizeof(right_fan) == 64 && sizeof(struct pair) == 384);
	static_assert(offsetof(struct pair, left) == 64 && offsetof(struct pair, right) == 192);

	poor_fan_init(&pair->left, &left_refs);
	poor_fan_init(&pair->right, &right_refs);
	assert(pair->left_refs == left_refs && pair->right_refs == right_refs);
	assert(pair->left_published == 0 && pair->right_published == 0);

	assert(poor_fan_latest(&pair->left, &left) == 0);
	poor_fan_pin(&pair->left, &left, &pin);
	poor_fan_publish(&pair->left, 1);
	assert(poor_fan_latest(&pair->right, &right) == 0);
	poor_fan_publish(&pair->right, 3);
	assert(pair->left_published == 1 && pair->right_published == 3);
	assert(poor_fan_published(&view->left) == 1 && poor_fan_published(&view->right) == 3);
	assert(count_of(left_refs, 0) == PIN_ONE + 1 && count_of(right_refs, 0) == 1);
	assert(poor_fan_held(&view->left, 0) && !poor_fan_unused(&view->left, 0) && poor_fan_unused(&view->left, 3));
	assert(poor_fan_held(&view->left, 1) && !poor_fan_held(&view->left, 3));
	assert(poor_fan_held(&view->right, 3) && !poor_fan_held(&view->right, 1));
	assert(!poor_fan_unused(&view->right, 3) && poor_fan_held(&view->right, 0) && poor_fan_unused(&view->right, 1));

	poor_fan_release(&pair->right, &right);
	assert(count_of(right_refs, 0) == 0 && count_of(left_refs, 0) == PIN_ONE + 1);
	poor_fan_release(&pair->left, &left);
	poor_fan_unpin(&pair->left, &pin);
	assert(count_of(left_refs, 0) == 0);
	for(uint32_t slot = 0; slot < SMALL_SLOTS; slot++)
		assert(count_of(left_refs, slot) == 0 && count_of(right_refs, slot) == 0);
	assert(atomic_load(&pairs[0].left.word) == 0 && atomic_load(&pairs[0].right.word) == 0);
	assert(pairs[0].left_refs == nullptr && pairs[0].right_refs == nullptr);
	assert(pairs[0].left_published == 0 && pairs[0].right_published == 0);
	return 0;
}

constexpr unsigned READERS = 8;
constexpr unsigned READER_PINS = 2;
constexpr uint32_t STRESS_SLOTS = READERS * (1 + READER_PINS) + 2;
constexpr unsigned RESOURCES = 8;
constexpr uint64_t PUBLISHES = 200000;
constexpr uint64_t POISON = UINT64_C(0x5150515051505150);

struct payload {
	uint64_t value;
	uint64_t inverse;
	uint64_t resource;
	uint64_t attached;
};

struct resource {
	uint64_t attached;
	uint64_t inverse;
};

struct superseded {
	unsigned resource;
	uint64_t epoch;
};

poor_fan_declare(stress_fan);

struct stress {
	stress_fan fan;
	alignas(64) struct poor_fan_ref *refs;
	alignas(64) _Atomic uint64_t progress;
	alignas(64) uint32_t published;
	uint32_t cursor;
	unsigned resource;
	uint64_t attached;
	uint32_t free_resources;
	unsigned superseded_head;
	unsigned superseded_count;
	struct superseded superseded[RESOURCES];
	uint64_t epochs[STRESS_SLOTS];
	bool poisoned[STRESS_SLOTS];
	uint64_t oldest;
	uint32_t fewest_unused;
	alignas(64) struct payload payloads[STRESS_SLOTS];
	alignas(64) struct resource resources[RESOURCES];
	struct poor_fan_ref counts[STRESS_SLOTS];
};

poor_fan_attach(stress_fan, struct stress, fan, refs, published);

struct reader {
	alignas(64) struct stress *stress;
	struct poor_fan_hold hold;
	struct poor_fan_pin pins[READER_PINS];
	uint64_t pinned[READER_PINS];
	unsigned period;
	unsigned lane;
	bool keeps;
	bool lags;
	uint64_t frames;
	uint64_t observed;
	alignas(64) _Atomic uint64_t latched;
};

static void stress_attach(struct stress *stress, uint64_t value) {
	unsigned resource = 0;

	if(stress->free_resources == 0)
		return;
	while(!(stress->free_resources >> resource & 1))
		resource++;
	stress->free_resources &= ~(UINT32_C(1) << resource);
	poor_asan_unpoison_object(&stress->resources[resource]);
	stress->resources[resource] = (struct resource){ .attached = value, .inverse = ~value };
	stress->superseded[(stress->superseded_head + stress->superseded_count) % RESOURCES] =
		(struct superseded){ .resource = stress->resource, .epoch = value };
	stress->superseded_count++;
	stress->resource = resource;
	stress->attached = value;
}

static void stress_publish(struct stress *stress, uint64_t value) {
	uint32_t slot = POOR_FAN_NO_SLOT;

	for(uint32_t tried = 0; tried < STRESS_SLOTS && slot == POOR_FAN_NO_SLOT; tried++) {
		stress->cursor = (stress->cursor + 1) % STRESS_SLOTS;
		if(poor_fan_unused(&stress->fan, stress->cursor))
			slot = stress->cursor;
	}
	assert(slot != POOR_FAN_NO_SLOT);
	poor_asan_unpoison_object(&stress->payloads[slot]);
	stress->payloads[slot] = (struct payload){
		.value = value, .inverse = ~value, .resource = stress->resource, .attached = stress->attached,
	};
	stress->epochs[slot] = value;
	stress->poisoned[slot] = false;
	poor_fan_publish(&stress->fan, slot);
}

static void stress_reclaim(struct stress *stress) {
	uint64_t oldest = stress->epochs[poor_fan_published(&stress->fan)];
	uint32_t unused = 0;

	for(uint32_t slot = 0; slot < STRESS_SLOTS; slot++) {
		if(poor_fan_unused(&stress->fan, slot)) {
			unused++;
			if(stress->poisoned[slot])
				continue;
			stress->payloads[slot] = (struct payload){
				.value = POISON, .inverse = POISON, .resource = POISON, .attached = POISON,
			};
			poor_asan_poison_object(&stress->payloads[slot]);
			stress->poisoned[slot] = true;
		} else if(stress->epochs[slot] < oldest && poor_fan_held(&stress->fan, slot)) {
			oldest = stress->epochs[slot];
		}
	}
	assert(oldest >= stress->oldest);
	stress->oldest = oldest;
	if(unused < stress->fewest_unused)
		stress->fewest_unused = unused;
	while(stress->superseded_count != 0) {
		const struct superseded *entry = &stress->superseded[stress->superseded_head];

		if(entry->epoch > oldest)
			break;
		stress->resources[entry->resource] = (struct resource){ .attached = POISON, .inverse = POISON };
		poor_asan_poison_object(&stress->resources[entry->resource]);
		stress->free_resources |= UINT32_C(1) << entry->resource;
		stress->superseded_head = (stress->superseded_head + 1) % RESOURCES;
		stress->superseded_count--;
	}
}

static bool payload_intact(const struct payload *payload, uint64_t value) {
	return payload->value == value && payload->inverse == ~value;
}

static void *stress_read(void *arg) {
	struct reader *reader = arg;
	struct stress *stress = reader->stress;
	uint64_t previous = 0;

	while(previous != PUBLISHES) {
		const struct payload *payload = &stress->payloads[poor_fan_latest(&stress->fan, &reader->hold)];
		const uint64_t value = payload->value;
		const struct resource *resource;
		uint64_t progress = value;

		atomic_store_explicit(&reader->latched, value, memory_order_relaxed);
		assert(payload_intact(payload, value) && value >= previous);
		assert(payload->resource < RESOURCES);
		resource = &stress->resources[payload->resource];
		assert(resource->attached == payload->attached && resource->inverse == ~payload->attached);
		reader->observed += value != previous;
		previous = value;
		while(reader->lags && progress != PUBLISHES && (progress == value || progress % READERS != reader->lane)) {
			sched_yield();
			progress = atomic_load_explicit(&stress->progress, memory_order_relaxed);
		}

		const unsigned index = (unsigned)(reader->frames % READER_PINS);
		struct poor_fan_pin *pin = &reader->pins[index];
		const bool yield = ++reader->frames % reader->period == 0;

		assert(pin->slot == POOR_FAN_NO_SLOT
		       || payload_intact(&stress->payloads[pin->slot], reader->pinned[index]));
		reader->pinned[index] = value;
		poor_fan_pin(&stress->fan, &reader->hold, pin);
		if(!reader->keeps || yield)
			poor_fan_release(&stress->fan, &reader->hold);
		if(yield)
			sched_yield();
	}
	for(unsigned i = 0; i < READER_PINS; i++)
		poor_fan_unpin(&stress->fan, &reader->pins[i]);
	poor_fan_release(&stress->fan, &reader->hold);
	return nullptr;
}

static int stress_run(bool lagging) {
	static struct stress stress;
	static struct reader readers[READERS];
	pthread_t threads[READERS];
	uint64_t observed = 0;

	poor_fan_init(&stress.fan, &stress.counts);
	stress.fewest_unused = STRESS_SLOTS;
	stress.free_resources = ((UINT32_C(1) << RESOURCES) - 1) & ~UINT32_C(1);
	stress.payloads[0] = (struct payload){ .value = 0, .inverse = UINT64_MAX, .resource = 0, .attached = 0 };
	stress.resources[0] = (struct resource){ .attached = 0, .inverse = UINT64_MAX };
	for(unsigned resource = 1; resource < RESOURCES; resource++)
		poor_asan_poison_object(&stress.resources[resource]);
	for(unsigned i = 0; i < READERS; i++) {
		readers[i] = (struct reader){
			.stress = &stress, .hold = POOR_FAN_HOLD_INIT, .period = 1 + 5 * (i / 2 % 4),
			.lane = i, .keeps = i % 2 != 0, .lags = lagging,
		};
		for(unsigned pin = 0; pin < READER_PINS; pin++)
			readers[i].pins[pin] = (struct poor_fan_pin)POOR_FAN_PIN_INIT;
		assert(pthread_create(&threads[i], nullptr, stress_read, &readers[i]) == 0);
	}

	for(uint64_t value = 1; value <= PUBLISHES; value++) {
		if(value % 4 == 0)
			stress_attach(&stress, value);
		stress_publish(&stress, value);
		stress_reclaim(&stress);
		atomic_store_explicit(&stress.progress, value, memory_order_relaxed);
		while(lagging && atomic_load_explicit(&readers[value % READERS].latched, memory_order_relaxed) != value)
			sched_yield();
		if(value % 256 == 0) {
			sched_yield();
			stress_reclaim(&stress);
		}
	}

	for(unsigned i = 0; i < READERS; i++) {
		assert(pthread_join(threads[i], nullptr) == 0);
		assert(readers[i].observed > 0);
		observed += readers[i].observed;
	}
	poor_fan_publish(&stress.fan, poor_fan_published(&stress.fan));
	stress_reclaim(&stress);
	for(uint32_t slot = 0; slot < STRESS_SLOTS; slot++)
		assert(slot == poor_fan_published(&stress.fan) || poor_fan_unused(&stress.fan, slot));
	assert(count_of(stress.counts, poor_fan_published(&stress.fan)) == 0);
	assert(stress.superseded_count == 0);
	assert(observed >= READERS && (!lagging || stress.fewest_unused == 1));
	for(uint32_t slot = 0; slot < STRESS_SLOTS; slot++)
		poor_asan_unpoison_object(&stress.payloads[slot]);
	for(unsigned resource = 0; resource < RESOURCES; resource++)
		poor_asan_unpoison_object(&stress.resources[resource]);
	return 0;
}

static int fan_threads_test(void) {
	return stress_run(false);
}

static int fan_threads_lagging_test(void) {
	return stress_run(true);
}

typedef int test_fn (void);

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(fan_init_test),
	TEST_FN(fan_holds_test),
	TEST_FN(fan_pins_test),
	TEST_FN(fan_last_slot_test),
	TEST_FN(fan_histories_test),
	TEST_FN(fan_layout_test),
	TEST_FN(fan_threads_test),
	TEST_FN(fan_threads_lagging_test),
};

int main(int argc, char **argv) {
	if(argc != 2)
		return fprintf(stderr, "usage: %s test_name\n", argv[0]), 1;

	for(size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
		if(!strcmp(argv[1], tests[i].test_name))
			return tests[i].fn();

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
