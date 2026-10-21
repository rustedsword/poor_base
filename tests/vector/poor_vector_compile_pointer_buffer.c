#include <poor_vector.h>

poor_vector_declare(job_vector, int);

struct worker {
	job_vector jobs;
	int *queue;
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);
