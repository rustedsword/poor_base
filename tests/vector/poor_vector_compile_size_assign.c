#include <poor_vector.h>

poor_vector_declare(job_vector, int);

struct worker {
	job_vector jobs;
	int queue[4];
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);

int main(void) {
	struct worker w = {};
	poor_vector_size(&w.jobs) = 3;
	return 0;
}
