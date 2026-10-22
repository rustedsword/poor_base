#include <poor_vector.h>

poor_vector_declare(job_vector, int);

struct worker {
	job_vector jobs;
	int queue[4];
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);

int main(void) {
	struct worker w = {};
	const int *item = poor_vector_at(&w.jobs, 0);
	poor_vector_remove_ref(&w.jobs, item);
	return 0;
}
