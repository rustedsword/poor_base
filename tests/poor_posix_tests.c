#include <poor_mman.h>
#include <poor_poll.h>
#include <poor_socket.h>
#include <poor_stdio.h>
#include <poor_uio.h>
#include <poor_unistd.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

static int read_write_array_test(void) {
	const int out[] = {1, 2, 3};
	int in[4] = {0}, (*in_p)[2] = &(int[2]){0};
	size_t n = 2;
	int vla[n];
	int p[2];

	assert(pipe(p) == 0);
	assert(write_array(p[1], out) == (ssize_t)sizeof(out));
	assert(write_array(p[1], arrview_first(1, out)) == (ssize_t)sizeof(int));
	assert(write_array(p[1], (int[]){4, 5}) == 2 * (ssize_t)sizeof(int));
	assert(write_array(p[1], &(int[]){6, 7}) == 2 * (ssize_t)sizeof(int));
	assert(read_array(p[0], in) == (ssize_t)sizeof(in));
	assert(in[0] == 1 && in[1] == 2 && in[2] == 3 && in[3] == 1);
	assert(read_array(p[0], in_p) == 2 * (ssize_t)sizeof(int));
	assert((*in_p)[0] == 4 && (*in_p)[1] == 5);
	assert(read_array(p[0], vla) == 2 * (ssize_t)sizeof(int));
	assert(vla[0] == 6 && vla[1] == 7);
	close(p[0]);
	close(p[1]);
	return 0;
}

static int pread_pwrite_array_test(void) {
	FILE *f = tmpfile();
	char in[3];

	assert(f);
	assert(pwrite_array(fileno(f), arrview_str("abcdef"), 2) == 6);
	assert(pread_array(fileno(f), in, 4) == 3);
	assert(memcmp(in, "cde", 3) == 0);
	assert(pread_array(fileno(f), &in, 7) == 1);
	assert(in[0] == 'f');
	fclose(f);
	return 0;
}

static int send_recv_array_test(void) {
	char in[8];
	int sv[2];

	assert(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0);
	assert(send_array(sv[0], arrview_str("ping"), 0) == 4);
	assert(recv_array(sv[1], in, 0) == 4);
	assert(memcmp(in, "ping", 4) == 0);
	close(sv[0]);
	close(sv[1]);
	return 0;
}

static int sendto_recvfrom_array_test(void) {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	char in[4];
	int sv[2];

	assert(socketpair(AF_UNIX, SOCK_DGRAM, 0, sv) == 0);
	assert(sendto_array(sv[0], arrview_str("datagram"), 0, NULL, 0) == 8);
	assert(recvfrom_array(sv[1], in, 0, (struct sockaddr *)&addr, &addrlen) == 4);
	assert(memcmp(in, "data", 4) == 0);
	close(sv[0]);
	close(sv[1]);
	return 0;
}

static int readv_writev_array_test(void) {
	char head[] = "head", body[] = "body!", a[3], b[6];
	struct iovec out[] = {array_iovec(arrview_str(head)), array_iovec(arrview_str(body))};
	struct iovec in[] = {array_iovec(a), array_iovec(&b)};
	int p[2];

	assert(pipe(p) == 0);
	assert(writev_array(p[1], out) == 9);
	assert(writev_array(p[1], (struct iovec[]){{.iov_base = head, .iov_len = 1}, {.iov_base = body, .iov_len = 2}}) == 3);
	assert(readv_array(p[0], &in) == 9);
	assert(memcmp(a, "hea", 3) == 0 && memcmp(b, "dbody!", 6) == 0);
	assert(readv_array(p[0], arrview_first(1, in)) == 3);
	assert(memcmp(a, "hbo", 3) == 0);
	close(p[0]);
	close(p[1]);
	return 0;
}

static int readv_writev_arrays_test(void) {
	static const char head[] = "head";
	const int nums[] = {1, 2};
	char a[3], b[5];
	int c[2];
	int p[2];

	assert(pipe(p) == 0);
	assert(writev_arrays(p[1], arrview_str(head), arrview_str("body"), nums) == 8 + (ssize_t)sizeof(nums));
	assert(readv_arrays(p[0], a, &b, c) == 8 + (ssize_t)sizeof(c));
	assert(memcmp(a, "hea", 3) == 0 && memcmp(b, "dbody", 5) == 0 && c[0] == 1 && c[1] == 2);
	close(p[0]);
	close(p[1]);
	return 0;
}

static int poll_array_test(void) {
	int p[2];

	assert(pipe(p) == 0);
	struct pollfd fds[] = {{.fd = p[0], .events = POLLIN}, {.fd = p[1], .events = POLLOUT}};
	assert(poll_array(fds, 0) == 1);
	assert(!(fds[0].revents & POLLIN) && (fds[1].revents & POLLOUT));
	assert(write_array(p[1], "x") == 2);
	assert(poll_array(&fds, 0) == 2);
	assert(fds[0].revents & POLLIN);
	close(p[0]);
	close(p[1]);
	return 0;
}

static int getcwd_array_test(void) {
	char expected[4096], tiny[1];
	size_t len = sizeof(expected);
	char (*cwd)[len] = malloc_array(cwd);

	assert(cwd);
	assert(getcwd(expected, sizeof(expected)));
	assert(getcwd_array(cwd) == *cwd);
	assert(strcmp(*cwd, expected) == 0);
	errno = 0;
	assert(!getcwd_array(tiny) && errno == ERANGE);
	free(cwd);
	return 0;
}

static int readlink_array_test(void) {
	char dir[] = "/tmp/poor_posix_XXXXXX", in[8], small[3];

	assert(mkdtemp(dir));
	int dirfd = open(dir, O_RDONLY | O_DIRECTORY);
	assert(dirfd >= 0);
	assert(symlinkat("target", dirfd, "link") == 0);
	assert(readlinkat_array(dirfd, "link", in) == 6);
	assert(memcmp(in, "target", 6) == 0);
	assert(readlinkat_array(dirfd, "link", small) == 3);
	assert(memcmp(small, "tar", 3) == 0);

	concat_vla(path, dir, "/link");
	assert(readlink_array(path, &small) == 3);
	assert(readlink_array(path, in) == 6);
	assert(memcmp(in, "target", 6) == 0);

	assert(unlinkat(dirfd, "link", 0) == 0);
	close(dirfd);
	assert(rmdir(dir) == 0);
	return 0;
}

static int gethostname_array_test(void) {
	char expected[256], name[256];

	assert(gethostname(expected, sizeof(expected)) == 0);
	assert(gethostname_array(name) == 0);
	assert(strcmp(name, expected) == 0);
	assert(gethostname_array(&name) == 0);
	assert(strcmp(name, expected) == 0);
	return 0;
}

static int getentropy_array_test(void) {
	uint32_t key[64] = {0}, zero[64] = {0}, too_big[65];

	assert(getentropy_array(key) == 0);
	assert(memcmp(key, zero, sizeof(key)) != 0);
	assert(getentropy_array(too_big) == -1);
	return 0;
}

static int mmap_munmap_array_test(void) {
	int (*data)[4] = mmap_array(data, NULL, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	assert(data != MAP_FAILED);
	foreach_array_ref(data, ref)
		assert(*ref == 0);
	fill_array(data, 42);
	assert((*data)[0] == 42 && (*data)[3] == 42);
	assert(munmap_array(data) == 0);

	errno = 0;
	assert(mmap_array(data, NULL, PROT_READ, MAP_PRIVATE, -1, 0) == MAP_FAILED);
	assert(data == MAP_FAILED && errno == EBADF);
	return 0;
}

static int mmap_msync_array_test(void) {
	long page_size = sysconf(_SC_PAGESIZE);
	assert(page_size > 0);
	size_t n = 2 * (size_t)page_size / sizeof(int);
	int (*data)[n];
	int in[1];
	FILE *f = tmpfile();

	assert(f);
	assert(ftruncate(fileno(f), (off_t)page_size + (off_t)sizeof(*data)) == 0);
	assert(mmap_array(data, NULL, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(f), (off_t)page_size) != MAP_FAILED);
	(*data)[0] = 11;
	(*data)[n - 1] = 22;
	assert(msync_array(*data, MS_SYNC) == 0);
	assert(pread_array(fileno(f), in, (off_t)page_size) == (ssize_t)sizeof(in));
	assert(in[0] == 11);
	assert(pread_array(fileno(f), in, (off_t)page_size + (off_t)sizeof(*data) - (off_t)sizeof(int)) == (ssize_t)sizeof(in));
	assert(in[0] == 22);
	assert(munmap_array(*data) == 0);
	fclose(f);
	return 0;
}

static int mprotect_madvise_array_test(void) {
	int (*data)[4] = mmap_array(data, NULL, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	assert(data != MAP_FAILED);
	(*data)[3] = 42;
	assert(mprotect_array(data, PROT_READ) == 0);
	assert((*data)[3] == 42);
	assert(madvise_array(*data, MADV_NORMAL) == 0);
	assert(posix_madvise_array(data, POSIX_MADV_SEQUENTIAL) == 0);
	assert(mprotect_array(arrview_full(data), PROT_READ | PROT_WRITE) == 0);
	(*data)[3] = 43;
	assert((*data)[3] == 43);
	assert(munmap_array(data) == 0);
	return 0;
}

static int mlock_munlock_array_test(void) {
	int (*data)[4] = mmap_array(data, NULL, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	assert(data != MAP_FAILED);
	const int (*view)[4] = data;
	/* Locking may be denied by the process's permissions or locked-memory limit. */
	if(mlock_array(view) != 0)
		assert(errno == EPERM || errno == ENOMEM || errno == EAGAIN);
	assert(munlock_array(*view) == 0);
	assert(munmap_array(data) == 0);
	return 0;
}

typedef int test_fn (void);

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(read_write_array_test),
	TEST_FN(pread_pwrite_array_test),
	TEST_FN(send_recv_array_test),
	TEST_FN(sendto_recvfrom_array_test),
	TEST_FN(readv_writev_array_test),
	TEST_FN(readv_writev_arrays_test),
	TEST_FN(poll_array_test),
	TEST_FN(getcwd_array_test),
	TEST_FN(readlink_array_test),
	TEST_FN(gethostname_array_test),
	TEST_FN(getentropy_array_test),
	TEST_FN(mmap_munmap_array_test),
	TEST_FN(mmap_msync_array_test),
	TEST_FN(mprotect_madvise_array_test),
	TEST_FN(mlock_munlock_array_test),
};

int main(int argc, char **argv) {
	if(argc != 2)
		return fprintf(stderr, "usage: %s test_name\n", argv[0]), 1;

	for(size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
		if(!strcmp(argv[1], tests[i].test_name))
			return tests[i].fn();

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
