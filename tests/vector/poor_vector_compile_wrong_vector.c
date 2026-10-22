#include <poor_vector.h>

poor_vector_declare(job_vector, int);
poor_vector_declare(other_vector, int);

struct worker {
	other_vector jobs;
	int queue[4];
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);
