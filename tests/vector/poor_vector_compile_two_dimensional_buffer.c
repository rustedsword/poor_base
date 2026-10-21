#include <poor_vector.h>

poor_vector_declare(job_vector, int);

struct worker {
	job_vector jobs;
	char queue[4][8];
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);
