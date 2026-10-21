#include <poor_vector.h>

struct job {
	int id;
};

struct task {
	int id;
};

poor_vector_declare(job_vector, int);

struct worker {
	job_vector jobs;
	struct job queue[4];
};

struct planner {
	job_vector jobs;
	struct task queue[4];
};

poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);
poor_vector_attach_buffer(job_vector, struct planner, jobs, queue);
