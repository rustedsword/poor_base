#include <poor_vector.h>

poor_vector_declare(job_vector, int);

struct worker {
	job_vector jobs;
	int queue[4];
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);

int main(void) {
	const struct worker w = {};
	poor_vector_append(&w.jobs, 1);
	return 0;
}
