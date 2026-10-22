#include <poor_vector.h>

poor_vector_declare(job_vector, int);

struct worker {
	job_vector jobs;
	int queue[4];
};

int main(void) {
	struct worker w = {};
	poor_vector_append(&w.jobs, 1);
	return 0;
}
