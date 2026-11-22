// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_FAN_H
#define POOR_FAN_H
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <poor_traits.h>

/* A fan: a wait-free, single-producer multi-consumer (SPMC) publication mechanism.
 *
 * Use it when a single writer thread continuously publishes updated revisions of shared data
 * (such as video frames, configuration snapshots, or simulation state) and any number of
 * concurrent reader threads independently consume the latest version.
 *
 * All operations are wait-free and non-blocking with zero retries: every call completes in
 * a bounded number of atomic instructions.
 *
 * What is a slot?
 * The fan does not manage or store payload data directly. Instead, the caller maintains an
 * array of data items, where each array index represents a "slot". The fan coordinates access
 * exclusively through these slot indices:
 *
 *  - The writer acquires an unreferenced slot, populates it, and publishes its index.
 *  - A reader requests the latest slot, acquires a reference to the currently published index,
 *    and reads the data in that slot.
 *  - The writer queries the fan to determine which slots are no longer referenced by any reader,
 *    recycling only those. Readers are guaranteed that data in a referenced slot remains unchanged.
 *
 * What are holds and pins?
 * Readers retain references to slots using two complementary handles:
 *
 *  - A hold tracks the latest revision. Calling poor_fan_latest() atomically advances the hold
 *    to the currently published slot and releases the slot previously held.
 *  - A pin preserves access to a specific slot across subsequent updates. Calling poor_fan_pin()
 *    pins the slot currently held, keeping it referenced even after the hold advances or is released.
 *    Use it when a reader needs an earlier revision alongside the latest one (e.g. comparing the
 *    previous frame against the current one).
 *
 * Why two kinds of reader references?
 * They allow the writer to query slot usage with different granularity:
 *
 *  - poor_fan_held(): Checks whether any active hold references the slot. When false, no reader
 *    is actively consuming it as their primary revision. This is useful when the slot contains
 *    resources that can be reclaimed early (e.g. returning borrowed buffers or releasing locks)
 *    before reader processing fully concludes.
 *  - poor_fan_unused(): Checks whether the slot is completely unreferenced (no active holds,
 *    no active pins, and not currently published). An unused slot is safe for the writer to reuse.
 *
 * If your workload does not require early reclamation, you can ignore poor_fan_held() and check
 * only poor_fan_unused().
 *
 * Struct layout and integration:
 * The fan structure itself fits entirely within a single 64-byte cache line and contains a single
 * atomic 64-bit control word shared between the writer and all readers.
 *
 * It requires two additional fields in the enclosing struct, connected via compile-time offset
 * metadata (poor_fan_attach):
 *
 *  - refs: A pointer to an array of struct poor_fan_ref (one per slot). Loaded by readers and
 *    the writer; never modified after initialization.
 *  - published: A uint32_t storing the current published slot index. Accessed and modified
 *    exclusively by the writer thread.
 *
 * IMPORTANT RULES:
 *
 * 1. Single writer thread:
 *    poor_fan_publish(), poor_fan_published(), poor_fan_unused(), and poor_fan_held()
 *    must only be called by the single writer thread.
 *
 * 2. Dedicated reader handles:
 *    Each reader thread must use its own distinct poor_fan_hold and poor_fan_pin instances.
 *    Any number of reader threads may run concurrently.
 *
 * 3. Write only to unused slots:
 *    Populate a slot only when poor_fan_unused() returns true (or before it was ever published),
 *    and publish it only after writing is complete.
 *
 * 4. Maintain a candidate pool for recycling:
 *    The fan does not manage a free list internally. An unused slot remains unused until published,
 *    so the writer only needs to check previously published slots for reclamation. Avoid querying
 *    every slot on every iteration: poor_fan_unused() reads cache lines updated by readers.
 *
 * 5. Allocate sufficient slots:
 *    To ensure the writer always finds an unused slot without blocking, allocate at least:
 *      R * (1 + P) + 2 slots
 *    where R is the maximum number of reader threads, each having 1 hold and up to P pins.
 *    (1 published slot + R held slots + R * P pinned slots + 1 writer scratch slot).
 *
 * 6. Cache line separation:
 *    Ensure the 'refs' pointer is located on a read-mostly cache line, and 'published' is on a
 *    cache line private to the writer. Because readers frequently load 'refs', writing to its
 *    cache line would cause unnecessary cache invalidation across all reader cores.
 *
 * 7. Initialization:
 *    Initialize the fan with poor_fan_init() at runtime, or with POOR_FAN_INIT in static storage.
 *    Slot 0 is published by default on initialization; populate initial data into slot 0 before
 *    reader threads begin. Up to POOR_FAN_MAX_SLOTS (65,536) slots are supported.
 *
 * 8. Hold acquisition limit:
 *    Readers may acquire a published slot at most 2^48 - 1 times (about 2.8 * 10^14) before the
 *    next publication. Every poor_fan_latest() call that moves a hold onto the published slot counts;
 *    calls on a hold that already references it do not. Exceeding the limit overflows the acquisition
 *    count kept in the control word: the slot's reference count never returns to zero, and the slot
 *    is never reported as unused again. If publications can be that rare (the limit is roughly one
 *    month at 10^8 acquisitions per second), periodically republish the currently published slot:
 *    every poor_fan_publish() call resets the count.
 *
 * Quick example:
 *
 * 	struct frame {
 * 		int number;
 * 	};
 *
 * 	poor_fan_declare(frame_fan);
 *
 * 	struct screen {
 * 		frame_fan fan;
 * 		alignas(64) struct poor_fan_ref *refs; // Read-mostly line; readers load this pointer
 * 		alignas(64) uint32_t published;        // Writer-private cache line
 * 	};
 *
 * 	poor_fan_attach(frame_fan, struct screen, fan, refs, published);
 *
 * 	static struct frame frames[4] = { { .number = 1 } }; // Array of slot data
 * 	static struct poor_fan_ref refs[4];
 * 	static struct screen screen;
 *
 * 	poor_fan_init(&screen.fan, &refs); // Initializes fan; slot 0 is initially published
 *
 * 	// Reader acquires the latest published slot (slot 0)
 * 	struct poor_fan_hold hold = POOR_FAN_HOLD_INIT;
 * 	struct poor_fan_pin pin = POOR_FAN_PIN_INIT;
 * 	println(frames[poor_fan_latest(&screen.fan, &hold)].number); // prints: 1
 *
 * 	// Writer fills slot 1 (never published, hence unused) and publishes it
 * 	frames[1].number = 2;
 * 	poor_fan_publish(&screen.fan, 1);
 * 	println(poor_fan_held(&screen.fan, 0)); // prints: true (slot 0 is actively held by reader)
 *
 * 	// Reader pins the current slot before advancing its hold to compare both frames
 * 	poor_fan_pin(&screen.fan, &hold, &pin); // Pin retains slot 0
 * 	const struct frame *latest = &frames[poor_fan_latest(&screen.fan, &hold)]; // Hold moves to slot 1
 * 	const struct frame *before = &frames[pin.slot];
 * 	println(before->number, " -> ", latest->number); // prints: 1 -> 2
 *
 * 	// Writer checks slot 0: no longer held, but still referenced via pin
 * 	println(poor_fan_held(&screen.fan, 0));   // prints: false
 * 	println(poor_fan_unused(&screen.fan, 0)); // prints: false
 *
 * 	// Reader finishes with the previous frame and unpins it
 * 	poor_fan_unpin(&screen.fan, &pin);
 *
 * 	// Writer checks slot 0: now completely unreferenced and ready for reuse
 * 	println(poor_fan_unused(&screen.fan, 0)); // prints: true
 *
 * 	// Reader releases hold when done
 * 	poor_fan_release(&screen.fan, &hold);
 */

/* Sentinel value representing an invalid or unassigned slot. */
constexpr uint32_t POOR_FAN_NO_SLOT = UINT32_MAX;

/* Maximum number of slots supported by a fan (determined by the 16-bit slot index field). */
constexpr uint32_t POOR_FAN_MAX_SLOTS = 1u << 16;

/* Per-slot reference counter tracking active holds and pins.
 * Each entry occupies a dedicated 64-byte cache line to eliminate false sharing
 * between concurrent readers accessing different slots. */
struct poor_fan_ref {
	alignas(64) _Atomic uint64_t count;
};

/* Declares a fan type with the given typedef name. Sized and aligned to a 64-byte cache line. */
#define poor_fan_declare(name) h_fan_declare(name, h_fan_cat(h_fan_tag_, name), h_fan_cat(h_fan_meta_, name))

/* Connects the refs array pointer and the published slot member to the fan type
 * within the enclosing struct (using container_of style offset calculation).
 * Enforces at compile time that 'refs' and 'published' reside on separate cache lines.
 *
 * 	poor_fan_declare(frame_fan);
 * 	struct screen {
 * 		frame_fan fan;
 * 		alignas(64) struct poor_fan_ref *refs;
 * 		...
 * 		alignas(64) uint32_t published;
 * 		...
 * 	};
 * 	poor_fan_attach(frame_fan, struct screen, fan, refs, published);
 */
#define poor_fan_attach(name, struct_type, fan_member, refs_member, published_member) h_fan_attach(name,	\
	h_fan_cat(h_fan_meta_, name), struct_type, fan_member, refs_member, published_member)

/* Reader hold handle. Tracks the latest published slot acquired by the reader.
 * Initialize with POOR_FAN_HOLD_INIT.
 * 'slot' contains the currently held slot index, or POOR_FAN_NO_SLOT if unassigned. */
struct poor_fan_hold {
	uint32_t slot;
};

/* Reader pin handle. Retains a reference to an acquired slot across subsequent hold updates.
 * Initialize with POOR_FAN_PIN_INIT.
 * 'slot' contains the currently pinned slot index, or POOR_FAN_NO_SLOT if unassigned. */
struct poor_fan_pin {
	uint32_t slot;
};

#define POOR_FAN_HOLD_INIT { .slot = POOR_FAN_NO_SLOT }
#define POOR_FAN_PIN_INIT { .slot = POOR_FAN_NO_SLOT }

/* Static initializer for a fan.
 * The attached 'refs' pointer must be initialized to point to zeroed ref structures
 * (such as in static storage), and 'published' must be initialized to 0.
 *
 * 	static struct poor_fan_ref refs[8];
 * 	static struct screen screen = { .fan = POOR_FAN_INIT, .refs = refs, .published = 0 };
 */
#define POOR_FAN_INIT { .word = 0 }

/* Initializes a fan instance at runtime, zeroing all ref counters and setting the initial published slot to 0.
 * 'refs' must be a pointer to the complete array of struct poor_fan_ref (e.g. &refs_array).
 * Enforces at compile time that a fixed-size array does not exceed POOR_FAN_MAX_SLOTS entries;
 * the size of a variable-length array is not checked.
 *
 * 	struct poor_fan_ref refs[8];
 * 	struct screen screen;
 * 	poor_fan_init(&screen.fan, &refs);
 *
 * Evaluates as a statement. */
#define poor_fan_init(fan, refs) h_fan_init(fan, refs, #refs)

/* Writer API */

/* Publishes 'slot' as the latest revision for readers.
 * Subsequent calls to poor_fan_latest() will return this slot.
 * Readers holding the previously published slot retain access until released.
 * 'slot' must either be unreferenced (poor_fan_unused) or equal to the currently published slot.
 * Republishing the currently published slot changes nothing for readers and resets the hold
 * acquisition count (see rule 8). */
#define poor_fan_publish(fan, slot) h_fan_publish(fan, slot)

/* Returns the index of the currently published slot. */
#define poor_fan_published(fan) h_fan_published(fan)

/* Returns true if 'slot' is completely unreferenced: neither currently published,
 * nor referenced by any active hold or pin. Returns false otherwise.
 * An unreferenced slot remains unreferenced until explicitly published by the writer,
 * making it safe to write to. */
#define poor_fan_unused(fan, slot) h_fan_unused(fan, slot)

/* Returns true if 'slot' is the currently published slot or is referenced by any active reader hold,
 * false otherwise (pins are ignored).
 * The currently published slot is always reported as held, because readers can acquire it at any time.
 * Returning false indicates no reader is actively consuming this slot as its primary revision,
 * and no new holds or pins can be acquired on it until republishing. */
#define poor_fan_held(fan, slot) h_fan_held(fan, slot)

/* Reader API */

/* Advances 'hold' to the latest published slot and returns its index.
 * Automatically releases any slot previously held.
 * If 'hold' already references the currently published slot, no atomic update is performed. */
#define poor_fan_latest(fan, hold) h_fan_latest(fan, hold)

/* Releases the slot currently referenced by 'hold'. No-op if 'hold' references no slot. */
#define poor_fan_release(fan, hold) h_fan_release(fan, hold)

/* Pins the slot currently referenced by 'hold' into 'pin', releasing any slot previously pinned.
 * The pinned slot remains referenced even after 'hold' advances to a newer slot or is released.
 * To retain access to a slot while observing newer updates, call poor_fan_pin() before poor_fan_latest().
 * If 'hold' references no slot, 'pin' will also reference no slot. */
#define poor_fan_pin(fan, hold, pin) h_fan_pin(fan, hold, pin)

/* Releases the slot currently referenced by 'pin'. No-op if 'pin' references no slot. */
#define poor_fan_unpin(fan, pin) h_fan_unpin(fan, pin)

/****** ------------------------------------------------------------------------------ *******/
/*** Internal implementation details. Not part of the public API; subject to change.       ***/
/****** ------------------------------------------------------------------------------ *******/

/* Control word layout:
 * - Bits [15:0]: Currently published slot index (up to POOR_FAN_MAX_SLOTS - 1).
 * - Bits [63:16]: Latch counter tracking reader hold acquisitions since publication.
 *   It is 48 bits wide and overflows at 2^48 acquisitions without a publication (see rule 8).
 *
 * Synchronization protocol:
 * - poor_fan_latest() atomically adds (1 << 16) to the word. The returned value provides
 *   both the acquired slot index (bits 15:0) and increments the latch count in a single atomic step,
 *   guaranteeing a reader cannot acquire a slot without being accounted for.
 * - poor_fan_publish() atomically exchanges the word with the new slot index (and a zeroed latch count).
 *   It then adds the accumulated latch count from the exchange to the previous slot's refcounter.
 *
 * Per-slot reference count (struct poor_fan_ref):
 * - Bits [31:0]: Active hold count.
 * - Bits [63:32]: Active pin count.
 *
 * Note on published slot count:
 * Because new hold acquisitions accumulate in the control word's latch counter and releases immediately
 * decrement the slot's struct poor_fan_ref counter, the per-slot hold count for the actively published
 * slot is transiently incomplete (and may dip below zero before publication transfers the latches).
 * Consequently, the currently published slot is never considered unused and is always reported as held. */
constexpr unsigned h_fan_slot_bits = 16;
constexpr uint64_t h_fan_slot_mask = (UINT64_C(1) << h_fan_slot_bits) - 1;
constexpr uint64_t h_fan_latch_one = UINT64_C(1) << h_fan_slot_bits;
constexpr uint64_t h_fan_hold_one = 1;
constexpr uint64_t h_fan_pin_one = UINT64_C(1) << 32;

static_assert(POOR_FAN_MAX_SLOTS - 1 == h_fan_slot_mask);

#define h_fan_cat(a, ...) h_fan_primitive_cat(a, __VA_ARGS__)
#define h_fan_primitive_cat(a, ...) a ## __VA_ARGS__

/* Declaring the meta tag first binds h_meta to this scope's meta, preventing conflicts with an outer declaration */
#define h_fan_declare(name, tag, meta)			\
	struct meta;					\
	typedef union tag {				\
		alignas(64) _Atomic uint64_t word;	\
		struct meta *h_meta;			\
	} name

/* The meta struct is never instantiated. Its member array sizes encode the byte offsets
 * of the fan, refs pointer, and published slot within the enclosing structure for compile-time introspection. */
#define h_fan_attach(name, meta, type, fan, refs, published)							\
	struct meta {												\
		unsigned char h_fan_offset[offsetof(type, fan) + 1];						\
		unsigned char h_refs_offset[offsetof(type, refs) + 1];						\
		unsigned char h_published_offset[offsetof(type, published) + 1];				\
		static_assert(_Generic(&((type *)nullptr)->fan, name *: 1, default: 0),				\
			      "fan (" #fan ") must be an unqualified " #name);					\
		static_assert(_Generic(&((type *)nullptr)->refs, struct poor_fan_ref **: 1, default: 0),	\
			      "refs (" #refs ") must be an unqualified pointer to struct poor_fan_ref");	\
		static_assert(_Generic(&((type *)nullptr)->published, uint32_t *: 1, default: 0),		\
			      "published slot (" #published ") must be an unqualified uint32_t");		\
		static_assert(offsetof(type, refs) / 64 != offsetof(type, published) / 64,			\
			      "refs (" #refs ") and published slot (" #published ") share a cache line");	\
	}

#define h_fan_to_refs(fan) \
	((ptrdiff_t)sizeof((fan)->h_meta->h_refs_offset) - (ptrdiff_t)sizeof((fan)->h_meta->h_fan_offset))
#define h_fan_to_published(fan) \
	((ptrdiff_t)sizeof((fan)->h_meta->h_published_offset) - (ptrdiff_t)sizeof((fan)->h_meta->h_fan_offset))

#define h_fan_init(fan, refs, text) do {									\
	auto const _tmp_fan_refs_ = (refs);									\
	static_assert(_Generic(*_tmp_fan_refs_, struct poor_fan_ref *: 1, default: 0)				\
		      && _Generic(_tmp_fan_refs_, struct poor_fan_ref **: 0, default: 1),			\
		      "refs (" text ") must be a pointer to an array of struct poor_fan_ref");			\
	static_assert(if_vla(*_tmp_fan_refs_, true,								\
			     sizeof(*_tmp_fan_refs_) / sizeof(struct poor_fan_ref) <= POOR_FAN_MAX_SLOTS),	\
		      "refs (" text ") has more items than POOR_FAN_MAX_SLOTS");				\
	h_fan_init_at(&(fan)->word, h_fan_to_refs(fan), h_fan_to_published(fan), *_tmp_fan_refs_,		\
		      sizeof(*_tmp_fan_refs_) / sizeof(struct poor_fan_ref));					\
} while(0)

#define h_fan_publish(fan, slot) h_fan_publish_at(&(fan)->word, h_fan_to_refs(fan), h_fan_to_published(fan), slot)
#define h_fan_published(fan) h_fan_published_at(&(fan)->word, h_fan_to_published(fan))
#define h_fan_unused(fan, slot) h_fan_unused_at(&(fan)->word, h_fan_to_refs(fan), h_fan_to_published(fan), slot)
#define h_fan_held(fan, slot) h_fan_held_at(&(fan)->word, h_fan_to_refs(fan), h_fan_to_published(fan), slot)
#define h_fan_latest(fan, hold) h_fan_latest_at(&(fan)->word, h_fan_to_refs(fan), hold)
#define h_fan_release(fan, hold) h_fan_release_at(&(fan)->word, h_fan_to_refs(fan), hold)
#define h_fan_pin(fan, hold, pin) h_fan_pin_at(&(fan)->word, h_fan_to_refs(fan), hold, pin)
#define h_fan_unpin(fan, pin) h_fan_unpin_at(&(fan)->word, h_fan_to_refs(fan), pin)

static inline struct poor_fan_ref **h_fan_refs_mut(_Atomic uint64_t *word, ptrdiff_t to_refs) {
	void *const refs = (unsigned char *)word + to_refs;
	return refs;
}

static inline struct poor_fan_ref *const *h_fan_refs_const(const _Atomic uint64_t *word, ptrdiff_t to_refs) {
	const void *const refs = (const unsigned char *)word + to_refs;
	return refs;
}

static inline uint32_t *h_fan_slot_mut(_Atomic uint64_t *word, ptrdiff_t to_published) {
	void *const published = (unsigned char *)word + to_published;
	return published;
}

static inline const uint32_t *h_fan_slot_const(const _Atomic uint64_t *word, ptrdiff_t to_published) {
	const void *const published = (const unsigned char *)word + to_published;
	return published;
}

static inline void h_fan_init_at(_Atomic uint64_t *word, ptrdiff_t to_refs, ptrdiff_t to_published,
				 struct poor_fan_ref *refs, size_t slots) {
	atomic_init(word, 0);
	*h_fan_refs_mut(word, to_refs) = refs;
	*h_fan_slot_mut(word, to_published) = 0;
	for(size_t slot = 0; slot < slots; slot++)
		atomic_init(&refs[slot].count, 0);
}

static inline uint32_t h_fan_published_at(const _Atomic uint64_t *word, ptrdiff_t to_published) {
	return *h_fan_slot_const(word, to_published);
}

/* Memory ordering:
 * Readers release slots using memory_order_release, while the writer queries slot state
 * using memory_order_acquire. This establishes a happens-before relationship, guaranteeing
 * all reader accesses on a slot are completed and visible before the writer detects
 * the slot as unused and overwrites it. */
static inline bool h_fan_unused_at(const _Atomic uint64_t *word, ptrdiff_t to_refs, ptrdiff_t to_published,
				   uint32_t slot) {
	const struct poor_fan_ref *ref = &(*h_fan_refs_const(word, to_refs))[slot];
	return slot != *h_fan_slot_const(word, to_published)
	       && atomic_load_explicit(&ref->count, memory_order_acquire) == 0;
}

static inline bool h_fan_held_at(const _Atomic uint64_t *word, ptrdiff_t to_refs, ptrdiff_t to_published,
				 uint32_t slot) {
	const struct poor_fan_ref *ref = &(*h_fan_refs_const(word, to_refs))[slot];
	return slot == *h_fan_slot_const(word, to_published)
	       || (uint32_t)atomic_load_explicit(&ref->count, memory_order_acquire) != 0;
}

static inline void h_fan_publish_at(_Atomic uint64_t *word, ptrdiff_t to_refs, ptrdiff_t to_published,
				    uint32_t slot) {
	const uint64_t holds = atomic_exchange_explicit(word, slot, memory_order_acq_rel) >> h_fan_slot_bits;
	uint32_t *published = h_fan_slot_mut(word, to_published);
	struct poor_fan_ref *old = &(*h_fan_refs_const(word, to_refs))[*published];
	if(holds != 0)
		atomic_fetch_add_explicit(&old->count, holds * h_fan_hold_one, memory_order_relaxed);
	*published = slot;
}

static inline void h_fan_release_at(const _Atomic uint64_t *word, ptrdiff_t to_refs, struct poor_fan_hold *hold) {
	if(hold->slot == POOR_FAN_NO_SLOT)
		return;
	struct poor_fan_ref *ref = &(*h_fan_refs_const(word, to_refs))[hold->slot];
	atomic_fetch_sub_explicit(&ref->count, h_fan_hold_one, memory_order_release);
	hold->slot = POOR_FAN_NO_SLOT;
}

static inline uint32_t h_fan_latest_at(_Atomic uint64_t *word, ptrdiff_t to_refs, struct poor_fan_hold *hold) {
	if(hold->slot != POOR_FAN_NO_SLOT) {
		if((atomic_load_explicit(word, memory_order_relaxed) & h_fan_slot_mask) == hold->slot)
			return hold->slot;
		h_fan_release_at(word, to_refs, hold);
	}
	const uint64_t latched = atomic_fetch_add_explicit(word, h_fan_latch_one, memory_order_acq_rel);
	hold->slot = (uint32_t)(latched & h_fan_slot_mask);
	return hold->slot;
}

static inline void h_fan_unpin_at(const _Atomic uint64_t *word, ptrdiff_t to_refs, struct poor_fan_pin *pin) {
	if(pin->slot == POOR_FAN_NO_SLOT)
		return;
	struct poor_fan_ref *ref = &(*h_fan_refs_const(word, to_refs))[pin->slot];
	atomic_fetch_sub_explicit(&ref->count, h_fan_pin_one, memory_order_release);
	pin->slot = POOR_FAN_NO_SLOT;
}

static inline void h_fan_pin_at(const _Atomic uint64_t *word, ptrdiff_t to_refs, const struct poor_fan_hold *hold,
				struct poor_fan_pin *pin) {
	if(pin->slot == hold->slot)
		return;
	if(hold->slot != POOR_FAN_NO_SLOT) {
		struct poor_fan_ref *ref = &(*h_fan_refs_const(word, to_refs))[hold->slot];
		atomic_fetch_add_explicit(&ref->count, h_fan_pin_one, memory_order_relaxed);
	}
	h_fan_unpin_at(word, to_refs, pin);
	pin->slot = hold->slot;
}

#endif // POOR_FAN_H
