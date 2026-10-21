#include <poor_vector.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

struct job {
	int id;
};

poor_vector_declare(job_vector, uint8_t);
static_assert(sizeof(job_vector) == sizeof(void *) && alignof(job_vector) == alignof(void *));

struct worker {
	job_vector jobs;
	struct job queue[4];
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);

poor_vector_declare(id_vector, int);

struct registry {
	int ids[3];
	char pad[70000];
	struct {
		char tag;
		id_vector ids;
	} index;
};

poor_vector_attach_buffer(id_vector, struct registry, index.ids, ids);

poor_vector_declare(name_vector, unsigned _BitInt(12));

struct name {
	char text[8];
};

struct phonebook {
	char tag;
	name_vector names;
	struct {
		alignas(64) struct name names[3];
	} store;
};

poor_vector_attach_buffer(name_vector, struct phonebook, names, store.names);

poor_vector_declare(queue_vector, uint8_t);
poor_vector_declare(stack_vector, unsigned);

struct shift {
	queue_vector queue;
	int id;
	stack_vector stack;
	struct job buffer[4];
};

poor_vector_attach_buffer(queue_vector, struct shift, queue, buffer);
poor_vector_attach_buffer(stack_vector, struct shift, stack, buffer);

#define is_empty(vec) (!poor_vector_size(vec) && poor_vector_empty(vec) && !poor_vector_full(vec))

static bool has_ids(const job_vector *jobs, size_t n, const int *ids) {
	const struct job *queue = *poor_vector_buffer(jobs);
	for(size_t i = 0; i < n; i++)
		if(poor_vector_at(jobs, i) != &queue[i] || queue[i].id != ids[i])
			return false;
	return poor_vector_size(jobs) == n && !poor_vector_empty(jobs) && poor_vector_full(jobs) == (n == poor_vector_capacity(jobs));
}

#define is_vector(vec, ...) has_ids(vec, sizeof((int[]){__VA_ARGS__}) / sizeof(int), (int[]){__VA_ARGS__})

static void append_jobs(job_vector *jobs, int n) {
	for(int i = 1; i <= n; i++)
		poor_vector_append(jobs, (struct job){.id = i});
}

static int vector_init_test(void) {
	static struct worker static_worker;
	struct worker w = {}, *p = &w;

	assert(is_empty(&static_worker.jobs) && is_empty(&w.jobs) && is_empty(&p->jobs));
	assert(poor_vector_buffer(&w.jobs) == &w.queue && poor_vector_at(&w.jobs, 0) == &w.queue[0]);
	assert(poor_vector_buffer(&static_worker.jobs) == &static_worker.queue);
	static_assert(poor_vector_capacity(&w.jobs) == 4 && _Generic(poor_vector_capacity(&w.jobs), size_t: 1, default: 0));
	static_assert(_Generic(poor_vector_size(&w.jobs), uint8_t: 1, default: 0));
	static_assert(_Generic(poor_vector_empty(&w.jobs), bool: 1, default: 0));
	static_assert(_Generic(poor_vector_full(&w.jobs), bool: 1, default: 0));

	append_jobs(&w.jobs, 2);
	poor_vector_clear(&p->jobs);
	assert(is_empty(&w.jobs) && w.queue[0].id == 1 && w.queue[1].id == 2);
	return 0;
}

static int vector_append_test(void) {
	struct worker w = {.queue = {{7}, {7}, {7}, {7}}};
	struct job job = {.id = 4};

	poor_vector_append(&w.jobs, (struct job){.id = 1});
	assert(is_vector(&w.jobs, 1));
	poor_vector_append(&w.jobs, (struct job){.id = 2});
	poor_vector_append(&w.jobs, (struct job){});
	assert(is_vector(&w.jobs, 1, 2, 0));
	poor_vector_append(&w.jobs, job);
	assert(is_vector(&w.jobs, 1, 2, 0, 4) && poor_vector_full(&w.jobs));

	poor_vector_remove(&w.jobs, 2);
	poor_vector_append(&w.jobs, *poor_vector_at(&w.jobs, poor_vector_size(&w.jobs) - 1));
	assert(is_vector(&w.jobs, 1, 2, 4, 4));
	return 0;
}

static int vector_insert_test(void) {
	struct worker w = {};

	poor_vector_insert(&w.jobs, 0, (struct job){.id = 1});
	poor_vector_insert(&w.jobs, 1, (struct job){.id = 2});
	poor_vector_insert(&w.jobs, 0, (struct job){.id = 3});
	assert(is_vector(&w.jobs, 3, 1, 2));

	poor_vector_insert(&w.jobs, 2, *poor_vector_at(&w.jobs, 0));
	assert(is_vector(&w.jobs, 3, 1, 3, 2));

	poor_vector_clear(&w.jobs);
	poor_vector_insert(&w.jobs, 0, (struct job){});
	poor_vector_insert(&w.jobs, poor_vector_size(&w.jobs), (struct job){.id = 5});
	assert(is_vector(&w.jobs, 0, 5));
	return 0;
}

static int vector_remove_test(void) {
	struct worker w = {};
	append_jobs(&w.jobs, 4);

	poor_vector_remove(&w.jobs, 1);
	assert(is_vector(&w.jobs, 1, 3, 4));
	poor_vector_remove(&w.jobs, 0);
	assert(is_vector(&w.jobs, 3, 4));
	poor_vector_remove(&w.jobs, poor_vector_size(&w.jobs) - 1);
	assert(is_vector(&w.jobs, 3));

	append_jobs(&w.jobs, 3);
	poor_vector_remove_ref(&w.jobs, &w.queue[2]);
	assert(is_vector(&w.jobs, 3, 1, 3));
	poor_vector_remove_ref(&w.jobs, poor_vector_at(&w.jobs, 0));
	assert(is_vector(&w.jobs, 1, 3));

	int popped = 0;
	while(!poor_vector_empty(&w.jobs)) {
		struct job *last = poor_vector_at(&w.jobs, poor_vector_size(&w.jobs) - 1);
		popped = popped * 10 + last->id;
		poor_vector_remove_ref(&w.jobs, last);
	}
	assert(popped == 31 && is_empty(&w.jobs));
	return 0;
}

static int vector_access_test(void) {
	struct worker w = {}, other = {};
	const struct worker *const_w = &w;
	append_jobs(&w.jobs, 3);
	append_jobs(&other.jobs, 1);

	static_assert(_Generic(poor_vector_at(&w.jobs, 0), struct job *: 1, default: 0));
	static_assert(_Generic(poor_vector_buffer(&w.jobs), struct job (*)[4]: 1, default: 0));
	static_assert(_Generic(poor_vector_at(&const_w->jobs, 0), const struct job *: 1, default: 0));
	static_assert(_Generic(poor_vector_buffer(&const_w->jobs), const struct job (*)[4]: 1, default: 0));

	assert(poor_vector_at(&const_w->jobs, 2) == &w.queue[2] && poor_vector_at(&const_w->jobs, 1)->id == 2);
	assert(poor_vector_buffer(&const_w->jobs) == &w.queue && !poor_vector_empty(&const_w->jobs) && !poor_vector_full(&const_w->jobs));
	assert(poor_vector_size(&const_w->jobs) == 3);
	static_assert(_Generic(poor_vector_size(&const_w->jobs), uint8_t: 1, default: 0));

	bool use_other = false;
	assert(poor_vector_at(use_other ? &other.jobs : &w.jobs, 1) == &w.queue[1]);
	poor_vector_append(use_other ? &other.jobs : &w.jobs, (struct job){.id = 4});
	assert(is_vector(&w.jobs, 1, 2, 3, 4) && is_vector(&other.jobs, 1));
	return 0;
}

static int vector_buffer_test(void) {
	struct worker w = {};

	static_assert(ARRAY_SIZE(poor_vector_buffer(&w.jobs)) == poor_vector_capacity(&w.jobs));
	copy_array(poor_vector_buffer(&w.jobs), ((const struct job[]){{5}, {6}}));
	poor_vector_set_size(&w.jobs, 2);
	assert(is_vector(&w.jobs, 5, 6));

	foreach_array_ref(arrview_first(poor_vector_size(&w.jobs), poor_vector_buffer(&w.jobs)), ref)
		ref->id *= 2;
	assert(is_vector(&w.jobs, 10, 12));
	return 0;
}

static int vector_foreach_test(void) {
	struct worker w = {}, empty = {};
	append_jobs(&w.jobs, 4);

	poor_vector_foreach(&empty.jobs, ref)
		assert(false);
	poor_vector_foreach_bw(&empty.jobs, ref)
		assert(false);

	int visited = 0;
	poor_vector_foreach(&w.jobs, ref) {
		static_assert(_Generic(ref, struct job *: 1, default: 0));
		visited = visited * 10 + ref->id;
	}
	poor_vector_foreach_bw(&w.jobs, ref)
		visited = visited * 10 + ref->id;
	assert(visited == 12344321);

	const struct worker *const_w = &w;
	visited = 0;
	poor_vector_foreach(&const_w->jobs, ref) {
		static_assert(_Generic(ref, const struct job *: 1, default: 0));
		visited = visited * 10 + ref->id;
	}
	poor_vector_foreach_bw(&const_w->jobs, ref) {
		static_assert(_Generic(ref, const struct job *: 1, default: 0));
		visited = visited * 10 + ref->id;
	}
	assert(visited == 12344321);

	bool use_empty = false;
	visited = 0;
	poor_vector_foreach(use_empty ? &empty.jobs : &w.jobs, ref)
		poor_vector_foreach_bw(&w.jobs, inner)
			visited++;
	assert(visited == 16);

	visited = 0;
	poor_vector_foreach(&w.jobs, ref) {
		if(ref->id % 2)
			continue;
		if(ref->id == 4)
			break;
		visited = visited * 10 + ref->id;
	}
	poor_vector_foreach_bw(&w.jobs, ref)
		if((visited = visited * 10 + ref->id) > 1000)
			break;
	assert(visited == 2432);

	poor_vector_foreach(&w.jobs, ref)
		ref->id *= 2;
	assert(is_vector(&w.jobs, 2, 4, 6, 8));
	return 0;
}

static int vector_foreach_bw_remove_test(void) {
	struct worker w = {};
	append_jobs(&w.jobs, 4);

	poor_vector_foreach_bw(&w.jobs, ref)
		if(ref->id % 2)
			poor_vector_remove_ref(&w.jobs, ref);
	assert(is_vector(&w.jobs, 2, 4));

	int visited = 0;
	poor_vector_foreach_bw(&w.jobs, ref) {
		visited = visited * 10 + ref->id;
		poor_vector_remove_ref(&w.jobs, ref);
	}
	assert(visited == 42 && is_empty(&w.jobs));

	append_jobs(&w.jobs, 4);
	visited = 0;
	poor_vector_foreach_bw(&w.jobs, ref) {
		visited = visited * 10 + ref->id;
		if(ref->id == 3)
			poor_vector_clear(&w.jobs);
	}
	assert(visited == 43 && is_empty(&w.jobs));
	return 0;
}

#define ITER entry
#define test_cat(a, b) a##b

static int vector_foreach_macro_name_test(void) {
	struct worker w = {};
	append_jobs(&w.jobs, 3);

	int visited = 0;
	poor_vector_foreach(&w.jobs, ITER)
		visited = visited * 10 + entry->id;
	poor_vector_foreach_bw(&w.jobs, test_cat(it, 1))
		visited = visited * 10 + it1->id;
	poor_vector_foreach_bw(&w.jobs, ITER)
		poor_vector_remove_ref(&w.jobs, ITER);
	assert(visited == 123321 && is_empty(&w.jobs));
	return 0;
}

static int evals[3];
#define arg(i, x) (evals[i]++, (x))
#define once(...) do {											\
	memset(evals, 0, sizeof(evals));								\
	__VA_ARGS__;											\
	assert(evals[0] == 1 && evals[1] <= 1 && evals[2] <= 1);					\
} while(0)

static int vector_single_eval_test(void) {
	struct worker ws[2] = {};

	int i = 0;
	poor_vector_append(&ws[i++].jobs, (struct job){.id = 1});
	poor_vector_insert(&ws[i++].jobs, 0, (struct job){.id = 9});
	poor_vector_remove_ref(&ws[--i].jobs, poor_vector_at(&ws[1].jobs, 0));
	poor_vector_append(&ws[i--].jobs, (struct job){.id = 9});
	poor_vector_remove(&ws[++i].jobs, 0);
	poor_vector_clear(&ws[i++].jobs);
	assert(i == 2 && is_vector(&ws[0].jobs, 1) && is_empty(&ws[1].jobs));

	once(poor_vector_append(arg(0, &ws[0].jobs), arg(1, (struct job){.id = 2})));
	once(poor_vector_insert(arg(0, &ws[0].jobs), arg(1, 0), arg(2, (struct job){.id = 3})));
	once(poor_vector_append(arg(0, &ws[0].jobs), (struct job){}));
	once(poor_vector_remove(arg(0, &ws[0].jobs), arg(1, 3)));
	once(poor_vector_append(arg(0, &ws[0].jobs), (struct job){}));
	once(poor_vector_remove_ref(arg(0, &ws[0].jobs), arg(1, poor_vector_at(&ws[0].jobs, 3))));
	assert(is_vector(&ws[0].jobs, 3, 1, 2));

	once(assert(poor_vector_at(arg(0, &ws[0].jobs), arg(1, 1)) == &ws[0].queue[1]));
	once(assert(!poor_vector_empty(arg(0, &ws[0].jobs)) && !poor_vector_full(arg(1, &ws[0].jobs))));
	once(assert(poor_vector_buffer(arg(0, &ws[0].jobs)) == &ws[0].queue));
	once(assert(poor_vector_size(arg(0, &ws[0].jobs)) == 3));
	once(poor_vector_set_size(arg(0, &ws[1].jobs), arg(1, 2)));
	assert(poor_vector_size(&ws[1].jobs) == 2);
	once(poor_vector_clear(arg(0, &ws[1].jobs)));

	memset(evals, 0, sizeof(evals));
	static_assert(poor_vector_capacity(arg(0, &ws[0].jobs)) == 4);
	assert(!evals[0]);

	int visited = 0;
	poor_vector_foreach(arg(0, &ws[0].jobs), ref)
		visited = visited * 10 + ref->id;
	poor_vector_foreach_bw(arg(1, &ws[0].jobs), ref)
		visited = visited * 10 + ref->id;
	assert(evals[0] == 1 && evals[1] == 1 && visited == 312213);
	return 0;
}

#define size_type_test(size_type, capacity) {								\
	poor_vector_declare(counted, size_type);							\
	static struct holder {										\
		counted items;										\
		unsigned char buffer[capacity];								\
	} h;												\
	poor_vector_attach_buffer(counted, struct holder, items, buffer);				\
	static_assert(_Generic(poor_vector_size(&h.items), size_type: 1, default: 0));			\
													\
	for(size_t i = 0; i < (capacity); i++)								\
		poor_vector_append(&h.items, (unsigned char)i);						\
	assert(poor_vector_size(&h.items) == (capacity) && poor_vector_full(&h.items));			\
	for(size_t i = 0; i < (capacity); i++)								\
		assert(*poor_vector_at(&h.items, i) == (unsigned char)i);				\
													\
	poor_vector_remove(&h.items, 0);								\
	assert(poor_vector_size(&h.items) == (capacity) - 1 && !poor_vector_full(&h.items));		\
	assert((capacity) == 1 || *poor_vector_at(&h.items, 0) == 1);					\
	assert((capacity) == 1 || *poor_vector_at(&h.items, poor_vector_size(&h.items) - 1) == (unsigned char)((capacity) - 1));	\
													\
	poor_vector_set_size(&h.items, (capacity));							\
	assert(poor_vector_full(&h.items));								\
}

static int vector_size_type_test(void) {
	enum count : unsigned short { NO_ITEMS };

	size_type_test(char, CHAR_MAX)
	size_type_test(signed char, SCHAR_MAX)
	size_type_test(unsigned char, UCHAR_MAX)
	size_type_test(short, SHRT_MAX)
	size_type_test(unsigned short, USHRT_MAX)
	size_type_test(int, 70000)
	size_type_test(unsigned, 70000)
	size_type_test(long, 70000)
	size_type_test(unsigned long, 70000)
	size_type_test(long long, 70000)
	size_type_test(unsigned long long, 70000)
	size_type_test(size_t, 70000)
	size_type_test(enum count, 1000)
	size_type_test(bool, 1)
	size_type_test(unsigned _BitInt(1), 1)
	size_type_test(_BitInt(9), 255)
	size_type_test(unsigned _BitInt(12), 4095)
	size_type_test(_BitInt(100), 70000)
	size_type_test(unsigned _BitInt(128), 70000)
	return 0;
}

static int vector_default_size_type_test(void) {
	poor_vector_declare(default_vector);
	struct defaults {
		default_vector items;
		int buffer[2];
	} d = {};
	poor_vector_attach_buffer(default_vector, struct defaults, items, buffer);

	static_assert(_Generic(poor_vector_size(&d.items), unsigned: 1, default: 0));
	poor_vector_append(&d.items, 1);
	assert(poor_vector_size(&d.items) == 1 && d.buffer[0] == 1);
	return 0;
}

static int vector_layout_test(void) {
	static struct registry registry, copy;
	struct phonebook book = {}, books[3] = {};

	poor_vector_append(&registry.index.ids, 1);
	poor_vector_append(&registry.index.ids, 2);
	poor_vector_insert(&registry.index.ids, 0, 3);
	assert(poor_vector_buffer(&registry.index.ids) == &registry.ids && poor_vector_full(&registry.index.ids));
	assert(registry.ids[0] == 3 && registry.ids[1] == 1 && registry.ids[2] == 2 && *poor_vector_at(&registry.index.ids, 2) == 2);

	copy = registry;
	poor_vector_remove(&copy.index.ids, 0);
	assert(poor_vector_size(&copy.index.ids) == 2 && copy.ids[0] == 1 && copy.ids[1] == 2);
	assert(poor_vector_size(&registry.index.ids) == 3 && registry.ids[0] == 3 && registry.ids[1] == 1);

	static_assert(_Generic(poor_vector_at(&book.names, 0), struct name *: 1, default: 0));
	poor_vector_append(&book.names, (struct name){"alice"});
	poor_vector_append(&book.names, (struct name){"bob"});
	poor_vector_insert(&book.names, 1, (struct name){"carol"});
	assert(poor_vector_buffer(&book.names) == &book.store.names && poor_vector_size(&book.names) == 3);
	assert(!strcmp(poor_vector_at(&book.names, 0)->text, "alice") && !strcmp(poor_vector_at(&book.names, 1)->text, "carol"));
	assert(!strcmp(poor_vector_at(&book.names, 2)->text, "bob"));

	for(size_t i = 0; i < 3; i++)
		for(size_t n = 0; n <= i; n++)
			poor_vector_append(&books[i].names, (struct name){"x"});
	for(size_t i = 0; i < 3; i++)
		assert(poor_vector_size(&books[i].names) == i + 1 && poor_vector_buffer(&books[i].names) == &books[i].store.names);
	return 0;
}

static int vector_shared_buffer_test(void) {
	struct shift s = {};

	static_assert(poor_vector_capacity(&s.queue) == 4 && poor_vector_capacity(&s.stack) == 4);
	assert(poor_vector_buffer(&s.queue) == &s.buffer && poor_vector_buffer(&s.stack) == &s.buffer);

	poor_vector_append(&s.queue, (struct job){.id = 1});
	poor_vector_append(&s.queue, (struct job){.id = 2});
	assert(poor_vector_size(&s.queue) == 2 && poor_vector_size(&s.stack) == 0 && s.buffer[0].id == 1 && s.buffer[1].id == 2);

	poor_vector_clear(&s.queue);
	poor_vector_append(&s.stack, (struct job){.id = 3});
	poor_vector_insert(&s.stack, 0, (struct job){.id = 4});
	assert(poor_vector_size(&s.stack) == 2 && poor_vector_size(&s.queue) == 0 && s.buffer[0].id == 4 && s.buffer[1].id == 3);
	assert(poor_vector_at(&s.stack, 0) == &s.buffer[0] && poor_vector_at(&s.stack, 1)->id == 3);
	return 0;
}

static int vector_shadow_test(void) {
	poor_vector_declare(shadow_vector, short);
	struct outer {
		shadow_vector items;
		int buffer[2];
	} outer = {};
	poor_vector_attach_buffer(shadow_vector, struct outer, items, buffer);
	poor_vector_append(&outer.items, 1);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
	{
		poor_vector_declare(shadow_vector, long);
		struct inner {
			long buffer[3];
			shadow_vector items;
		} inner = {};
		poor_vector_attach_buffer(shadow_vector, struct inner, items, buffer);
		poor_vector_append(&inner.items, 5L);
		assert(poor_vector_capacity(&inner.items) == 3 && poor_vector_at(&inner.items, 0) == &inner.buffer[0]);
		assert(poor_vector_capacity(&outer.items) == 2 && poor_vector_at(&outer.items, 0) == &outer.buffer[0]);
	}
#pragma GCC diagnostic pop

	assert(poor_vector_capacity(&outer.items) == 2 && poor_vector_size(&outer.items) == 1 && outer.buffer[0] == 1);
	return 0;
}

typedef int test_fn (void);

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(vector_init_test),
	TEST_FN(vector_append_test),
	TEST_FN(vector_insert_test),
	TEST_FN(vector_remove_test),
	TEST_FN(vector_access_test),
	TEST_FN(vector_buffer_test),
	TEST_FN(vector_foreach_test),
	TEST_FN(vector_foreach_bw_remove_test),
	TEST_FN(vector_foreach_macro_name_test),
	TEST_FN(vector_single_eval_test),
	TEST_FN(vector_size_type_test),
	TEST_FN(vector_default_size_type_test),
	TEST_FN(vector_layout_test),
	TEST_FN(vector_shared_buffer_test),
	TEST_FN(vector_shadow_test),
};

int main(int argc, char **argv) {
	if(argc != 2)
		return fprintf(stderr, "usage: %s test_name\n", argv[0]), 1;

	for(size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
		if(!strcmp(argv[1], tests[i].test_name))
			return tests[i].fn();

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
