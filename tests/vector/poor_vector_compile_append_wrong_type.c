#include <poor_vector.h>

struct job {
	int id;
};

struct other {
	int id;
};

poor_vector_declare(job_vector, int);

struct worker {
	job_vector jobs;
	struct job queue[4];
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);

int main(void) {
	struct worker w = {};
	poor_vector_append(&w.jobs, (struct other){1});
	return 0;
}
