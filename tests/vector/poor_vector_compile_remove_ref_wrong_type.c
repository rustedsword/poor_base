#include <poor_vector.h>

poor_vector_declare(job_vector, int);

struct worker {
	job_vector jobs;
	int queue[4];
	long other;
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);

int main(void) {
	struct worker w = {};
	poor_vector_remove_ref(&w.jobs, &w.other);
	return 0;
}
