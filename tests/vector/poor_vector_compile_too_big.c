#include <poor_vector.h>

poor_vector_declare(job_vector, uint8_t);

struct worker {
	job_vector jobs;
	int queue[256];
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);
