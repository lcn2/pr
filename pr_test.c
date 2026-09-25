/*
 * pr_test - test the stdio helper library
 *
 * "Small acts of kindness can fill the world with light."
 *
 *      -- J.R.R. Tolkien
 *
 * Copyright (c) 2008-2026 by Landon Curt Noll and Cody Boone Ferguson.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright, this permission notice and text
 * this comment, and the disclaimer below appear in all of the following:
 *
 *       supporting documentation
 *       source copies
 *       source works derived from this source
 *       binaries derived from this source or from derived source
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
 *
 * The code in this from repo was copied out of the mkiocccentry toolkit repo:
 *
 *	https://github.com/ioccc-src/mkiocccentry
 *
 * and out of the jparse repo:
 *
 *	https://github.com/xexyl/jparse
 *
 * The origin of libpr dates back to code written by Landon Curt Noll around 2008.
 *
 * That 2008 code was copied into the jparse repo, and the mkiocccentry toolkit repo
 * by Landon Curt Noll.  While in the jparse repo, both Landon Curt Noll and
 * Cody Boone Ferguson added to and improved this code base:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * dyn_array_test - test the dynamic array facility
 */
#include "pr.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */
#define PR_TEST_BASENAME "pr_test"
#define PR_TEST_VERSION "1.2.0 2026-09-25"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V]\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    1\ttest suite failed\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tcommand line error\n"
    " >=10\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "pr library version: %s\n";


/*
 * forward declarations
 */
static void usage(int exitcode, char const *prog, char const *str);
static FILE *open_tmp_stream(void const *buf, size_t len);
static bool stream_equals(FILE *stream, char const *expected, size_t expected_len);
#if defined(_GNU_SOURCE)
static ssize_t error_stream_read(void *cookie, char *buf, size_t len);
#endif
static bool test_read_all_reuse(void);
static bool test_read_all_empty_and_state(void);
static bool test_readline_dup_contract(void);
static bool test_fprint_line_helpers(void);
static bool test_null_name_diagnostics(void);
static long count_open_fds(void);
static bool test_open_dir_file_fd_leak(void);
static bool expect_open_dir_file_rejected(char const *dir, char const *file, int expected_exit);
static bool test_open_dir_file_path_traversal(void);


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    bool error = false;		/* true ==> test error found */
    bool opt_error = false;	/* fchk_inval_opt() return */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:V")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, ""); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version and exit */
	    (void) printf("%s version: %s\n", PR_TEST_BASENAME, PR_TEST_VERSION);
	    (void) printf("libpr version: %s\n", pr_version);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
	    opt_error = fchk_inval_opt(stderr, program, i, optopt);
	    if (opt_error) {
		usage(3, program, ""); /*ooo*/
		not_reached();
	    } else {
		fwarn(stderr, __func__, "getopt() return: %c optopt: %c", (char)i, (char)optopt);
	    }
	    break;
	}
    }
    if (argc - optind != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    if (test_read_all_reuse() == true) {
	error = true;
    }
    if (test_read_all_empty_and_state() == true) {
	error = true;
    }
    if (test_readline_dup_contract() == true) {
	error = true;
    }
    if (test_fprint_line_helpers() == true) {
	error = true;
    }
    if (test_null_name_diagnostics() == true) {
	error = true;
    }
    if (test_open_dir_file_fd_leak() == true) {
	error = true;
    }
    if (test_open_dir_file_path_traversal() == true) {
	error = true;
    }

    /*
     * exit based on the test result
     */
    if (error == true) {
	exit(1); /*ooo*/
    }
    exit(0); /*ooo*/
}


/*
 * open_tmp_stream - create a temporary stream with optional contents
 */
static FILE *
open_tmp_stream(void const *buf, size_t len)
{
    FILE *stream = NULL;
    size_t written;

    stream = tmpfile();
    if (stream == NULL) {
	warnp(__func__, "tmpfile failed");
	return NULL;
    }
    if (len > 0) {
	written = fwrite(buf, 1, len, stream);
	if (written != len) {
	    warnp(__func__, "fwrite wrote %zu bytes, expected %zu", written, len);
	    fclose(stream);
	    return NULL;
	}
    }
    rewind(stream);
    return stream;
}


/*
 * stream_equals - compare the contents of a stream with expected data
 */
static bool
stream_equals(FILE *stream, char const *expected, size_t expected_len)
{
    char *buf = NULL;
    long stream_len;
    size_t got;
    bool failed = true;

    if (stream == NULL || expected == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return true;
    }

    if (fseek(stream, 0L, SEEK_END) != 0) {
	warnp(__func__, "fseek to end failed");
	return true;
    }
    stream_len = ftell(stream);
    if (stream_len < 0) {
	warnp(__func__, "ftell failed");
	return true;
    }
    if ((size_t)stream_len != expected_len) {
	warn(__func__, "stream length mismatch: got %ld expected %zu", stream_len, expected_len);
	return true;
    }
    rewind(stream);
    buf = calloc(expected_len + 1, sizeof(*buf));
    if (buf == NULL) {
	warnp(__func__, "calloc failed");
	return true;
    }
    got = fread(buf, 1, expected_len, stream);
    if (got != expected_len) {
	warnp(__func__, "fread read %zu bytes, expected %zu", got, expected_len);
	free(buf);
	return failed;
    }
    if (memcmp(buf, expected, expected_len) != 0) {
	warn(__func__, "stream contents did not match expected output");
	free(buf);
	return failed;
    }
    failed = false;
    free(buf);
    return failed;
}


#if defined(_GNU_SOURCE)
/*
 * error_stream_read - deterministic read failure hook for fopencookie()
 */
static ssize_t
error_stream_read(void *cookie, char *buf, size_t len)
{
    (void)cookie;
    (void)buf;
    (void)len;
    errno = EIO;
    return -1;
}
#endif


/*
 * test_read_all_reuse - verify read_all() caller ownership across repeated calls
 */
static bool
test_read_all_reuse(void)
{
    unsigned char *sample = NULL;
    size_t sample_len = READ_ALL_CHUNK + 17;
    size_t i;
    bool failed = false;

    sample = calloc(sample_len, sizeof(*sample));
    if (sample == NULL) {
	warnp(__func__, "calloc failed");
	return true;
    }
    for (i = 0; i < sample_len; ++i) {
	sample[i] = (unsigned char)(i & 0xff);
    }

    for (i = 0; i < 32; ++i) {
	FILE *stream = NULL;
	size_t len = 1;
	unsigned char *data = NULL;

	stream = open_tmp_stream(sample, sample_len);
	if (stream == NULL) {
	    failed = true;
	    break;
	}
	data = read_all(stream, &len);
	if (data == NULL) {
	    warn(__func__, "read_all returned NULL on iteration %zu", i);
	    failed = true;
	    fclose(stream);
	    break;
	}
	if (len != sample_len) {
	    warn(__func__, "read_all length mismatch: got %zu expected %zu", len, sample_len);
	    failed = true;
	} else if (memcmp(data, sample, sample_len) != 0) {
	    warn(__func__, "read_all data mismatch on iteration %zu", i);
	    failed = true;
	} else if (data[len] != '\0') {
	    warn(__func__, "read_all buffer missing trailing NUL on iteration %zu", i);
	    failed = true;
	}
	free(data);
	fclose(stream);
	if (failed == true) {
	    break;
	}
    }
    free(sample);
    return failed;
}


/*
 * test_read_all_empty_and_state - verify empty, EOF, and error flag handling
 */
static bool
test_read_all_empty_and_state(void)
{
    FILE *stream = NULL;
    unsigned char *data = NULL;
    size_t len = SIZE_MAX;
    int c;

    stream = open_tmp_stream(NULL, 0);
    if (stream == NULL) {
	return true;
    }
    data = read_all(stream, &len);
    if (data == NULL || len != 0 || data[0] != '\0') {
	warn(__func__, "read_all failed empty-input contract");
	fclose(stream);
	free(data);
	return true;
    }
    free(data);
    fclose(stream);

    stream = open_tmp_stream(NULL, 0);
    if (stream == NULL) {
	return true;
    }
    c = fgetc(stream);
    if (c != EOF || feof(stream) == 0) {
	warn(__func__, "failed to preset EOF on empty stream");
	fclose(stream);
	return true;
    }
    len = SIZE_MAX;
    data = read_all(stream, &len);
    if (data == NULL || len != 0 || data[0] != '\0') {
	warn(__func__, "read_all failed preset-EOF contract");
	fclose(stream);
	free(data);
	return true;
    }
    free(data);
    fclose(stream);

#if defined(_GNU_SOURCE)
    {
	cookie_io_functions_t error_funcs = { .read = error_stream_read };

	stream = fopencookie(NULL, "r", error_funcs);
	if (stream == NULL) {
	    warnp(__func__, "fopencookie failed");
	    return true;
	}
	c = fgetc(stream);
	if (!(c == EOF && ferror(stream) != 0)) {
	    warn(__func__, "failed to preset error indicator on synthetic error stream");
	    fclose(stream);
	    return true;
	}
	len = SIZE_MAX;
	data = read_all(stream, &len);
	if (data != NULL || len != 0) {
	    warn(__func__, "read_all failed preset-error contract");
	    fclose(stream);
	    free(data);
	    return true;
	}
	fclose(stream);
    }
#endif
    return false;
}


/*
 * test_readline_dup_contract - verify caller ownership of getline buffer and EOF behavior
 */
static bool
test_readline_dup_contract(void)
{
    static char const input[] = "trim me \n";
    FILE *stream = NULL;
    char *line = NULL;
    char *dup = NULL;
    size_t len = 0;

    stream = open_tmp_stream(input, sizeof(input) - 1);
    if (stream == NULL) {
	return true;
    }
    dup = readline_dup(&line, true, &len, stream);
    if (dup == NULL || strcmp(dup, "trim me") != 0 || len != strlen("trim me")) {
	warn(__func__, "readline_dup failed normal read contract");
	fclose(stream);
	free(dup);
	free(line);
	return true;
    }
    if (line == NULL || strcmp(line, "trim me ") != 0) {
	warn(__func__, "getline buffer contract mismatch after readline_dup");
	fclose(stream);
	free(dup);
	free(line);
	return true;
    }
    free(dup);

    dup = readline_dup(&line, false, &len, stream);
    if (dup != NULL) {
	warn(__func__, "readline_dup returned data at EOF");
	fclose(stream);
	free(dup);
	free(line);
	return true;
    }
    if (line == NULL) {
	warn(__func__, "getline buffer unexpectedly lost ownership at EOF");
	fclose(stream);
	return true;
    }
    free(line);
    fclose(stream);

    stream = open_tmp_stream(NULL, 0);
    if (stream == NULL) {
	return true;
    }
    line = NULL;
    dup = readline_dup(&line, false, &len, stream);
    if (dup != NULL) {
	warn(__func__, "readline_dup empty-stream EOF contract mismatch");
	fclose(stream);
	free(dup);
	free(line);
	return true;
    }
    free(line);
    fclose(stream);
    return false;
}


/*
 * test_fprint_line_helpers - verify encoded output and length accounting helpers
 */
static bool
test_fprint_line_helpers(void)
{
    static unsigned char const buf[] = {'<', 'A', '\n', '>', '\\', '\0'};
    static char const expected[] = "<\\x3cA\\n\\x3e\\\\\\0>";
    FILE *stream = NULL;
    ssize_t count;
    size_t len = 0;

    stream = tmpfile();
    if (stream == NULL) {
	warnp(__func__, "tmpfile failed");
	return true;
    }
    count = fprint_line_buf(stream, buf, sizeof(buf), '<', '>');
    if (count != (ssize_t)(sizeof(expected) - 1)) {
	warn(__func__, "fprint_line_buf returned %zd, expected %zu", count, sizeof(expected) - 1);
	fclose(stream);
	return true;
    }
    if (stream_equals(stream, expected, sizeof(expected) - 1) == true) {
	fclose(stream);
	return true;
    }
    fclose(stream);

    count = fprint_line_buf(NULL, buf, sizeof(buf), '<', '>');
    if (count != (ssize_t)(sizeof(expected) - 1)) {
	warn(__func__, "fprint_line_buf(NULL, ...) returned %zd, expected %zu", count, sizeof(expected) - 1);
	return true;
    }

    count = fprint_line_str(NULL, "hello", &len, '"', '"');
    if (count != 7 || len != 5) {
	warn(__func__, "fprint_line_str length mismatch: count=%zd len=%zu", count, len);
	return true;
    }
    return false;
}


/*
 * test_null_name_diagnostics - verify NULL diagnostic names use a safe fallback
 */
static bool
test_null_name_diagnostics(void)
{
    int pipefd[2];
    int saved_verbosity = verbosity_level;
    ssize_t written;
    bool ready = false;

    if (pipe(pipefd) != 0) {
	warnp(__func__, "pipe failed");
	return true;
    }
    verbosity_level = DBG_VVHIGH;
    if (fd_is_ready(NULL, true, -1) != false) {
	warn(__func__, "fd_is_ready(NULL, true, -1) unexpectedly returned true");
	close(pipefd[0]);
	close(pipefd[1]);
	verbosity_level = saved_verbosity;
	return true;
    }
    written = write(pipefd[1], "x", 1);
    if (written != 1) {
	warnp(__func__, "write to pipe failed");
	close(pipefd[0]);
	close(pipefd[1]);
	verbosity_level = saved_verbosity;
	return true;
    }
    ready = fd_is_ready(NULL, false, pipefd[0]);
    flush_tty(NULL, false, false);
    close(pipefd[0]);
    close(pipefd[1]);
    verbosity_level = saved_verbosity;
    if (ready != true) {
	warn(__func__, "fd_is_ready(NULL, false, pipefd[0]) unexpectedly returned false");
	return true;
    }
    return false;
}


/*
 * count_open_fds - count currently open file descriptors for this process
 */
static long
count_open_fds(void)
{
    long count = 0;
    long fd;
    long maxfd = sysconf(_SC_OPEN_MAX);

    if (maxfd < 0) {
	maxfd = 256;
    }
    for (fd = 0; fd < maxfd; ++fd) {
	errno = 0;
	if (fcntl((int)fd, F_GETFD) != -1 || errno != EBADF) {
	    ++count;
	}
    }
    return count;
}


/*
 * test_open_dir_file_fd_leak - verify open_dir_file() does not leak cwd fds when dir == NULL
 */
static bool
test_open_dir_file_fd_leak(void)
{
    static char const sample[] = "fd leak regression\n";
    char path[] = "/tmp/pr_test.open_dir_file.XXXXXX";
    FILE *stream = NULL;
    int fd = -1;
    ssize_t written;
    long before;
    long during;
    long after;
    bool failed = false;

    fd = mkstemp(path);
    if (fd < 0) {
	warnp(__func__, "mkstemp failed");
	return true;
    }
    written = write(fd, sample, sizeof(sample) - 1);
    if (written != (ssize_t)(sizeof(sample) - 1)) {
	warnp(__func__, "write failed");
	close(fd);
	unlink(path);
	return true;
    }
    if (close(fd) != 0) {
	warnp(__func__, "close failed");
	unlink(path);
	return true;
    }

    before = count_open_fds();
    stream = open_dir_file(NULL, path);
    if (stream == NULL) {
	warn(__func__, "open_dir_file(NULL, ...) returned NULL");
	unlink(path);
	return true;
    }
    during = count_open_fds();
    if (during != before + 1) {
	warn(__func__, "open_dir_file(NULL, ...) leaked file descriptors: before=%ld during=%ld", before, during);
	failed = true;
    }
    if (fclose(stream) != 0) {
	warnp(__func__, "fclose failed");
	failed = true;
    }
    after = count_open_fds();
    if (after != before) {
	warn(__func__, "open_dir_file(NULL, ...) left file descriptors open after fclose: before=%ld after=%ld",
	     before, after);
	failed = true;
    }
    if (unlink(path) != 0) {
	warnp(__func__, "unlink failed");
	failed = true;
    }
    return failed;
}


/*
 * expect_open_dir_file_rejected - verify open_dir_file() rejects a dangerous path
 */
static bool
expect_open_dir_file_rejected(char const *dir, char const *file, int expected_exit)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0) {
	warnp(__func__, "fork failed");
	return true;
    }
    if (pid == 0) {
	FILE *stream = NULL;

	stream = open_dir_file(dir, file);
	if (stream != NULL) {
	    fclose(stream);
	}
	_exit(0);
    }
    if (waitpid(pid, &status, 0) < 0) {
	warnp(__func__, "waitpid failed");
	return true;
    }
    if (WIFSIGNALED(status)) {
	warn(__func__, "open_dir_file(%s, %s) died from signal %d, expected exit %d",
	     dir != NULL ? dir : "((NULL dir))", file != NULL ? file : "((NULL file))",
	     WTERMSIG(status), expected_exit);
	return true;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != expected_exit) {
	warn(__func__, "open_dir_file(%s, %s) exited %d, expected %d",
	     dir != NULL ? dir : "((NULL dir))", file != NULL ? file : "((NULL file))",
	     WIFEXITED(status) ? WEXITSTATUS(status) : -1, expected_exit);
	return true;
    }
    return false;
}


/*
 * test_open_dir_file_path_traversal - verify open_dir_file() rejects escaping paths
 */
static bool
test_open_dir_file_path_traversal(void)
{
    static char const sample[] = "path traversal regression\n";
    char dir_template[] = "/tmp/pr_test.open_dir_file.dir.XXXXXX";
    char allowed_path[128];
    char outside_template[128];
    char relative_escape[128];
    char *dir = NULL;
    char *outside_base = NULL;
    char *parent_slash = NULL;
    FILE *stream = NULL;
    int fd = -1;
    ssize_t written;
    bool failed = false;

    dir = mkdtemp(dir_template);
    if (dir == NULL) {
	warnp(__func__, "mkdtemp failed");
	return true;
    }
    if (snprintf(allowed_path, sizeof(allowed_path), "%s/%s", dir, "allowed.txt") >= (int)sizeof(allowed_path)) {
	warn(__func__, "allowed path overflow");
	rmdir(dir);
	return true;
    }
    fd = open(allowed_path, O_WRONLY|O_CREAT|O_TRUNC|O_CLOEXEC, 0600);
    if (fd < 0) {
	warnp(__func__, "open allowed_path failed");
	rmdir(dir);
	return true;
    }
    written = write(fd, sample, sizeof(sample) - 1);
    if (written != (ssize_t)(sizeof(sample) - 1)) {
	warnp(__func__, "write allowed_path failed");
	close(fd);
	unlink(allowed_path);
	rmdir(dir);
	return true;
    }
    if (close(fd) != 0) {
	warnp(__func__, "close allowed_path failed");
	unlink(allowed_path);
	rmdir(dir);
	return true;
    }
    parent_slash = strrchr(dir, '/');
    if (parent_slash == NULL || parent_slash == dir) {
	warn(__func__, "unable to compute parent directory");
	unlink(allowed_path);
	rmdir(dir);
	return true;
    }
    if (snprintf(outside_template, sizeof(outside_template), "%.*s/%s",
		 (int)(parent_slash - dir), dir, "pr_test.open_dir_file.outside.XXXXXX") >= (int)sizeof(outside_template)) {
	warn(__func__, "outside template overflow");
	unlink(allowed_path);
	rmdir(dir);
	return true;
    }
    stream = open_dir_file(dir, "allowed.txt");
    if (stream == NULL) {
	warn(__func__, "open_dir_file(dir, \"allowed.txt\") returned NULL");
	unlink(allowed_path);
	rmdir(dir);
	return true;
    }
    if (stream_equals(stream, sample, sizeof(sample) - 1) == true) {
	fclose(stream);
	unlink(allowed_path);
	rmdir(dir);
	return true;
    }
    if (fclose(stream) != 0) {
	warnp(__func__, "fclose allowed_path failed");
	unlink(allowed_path);
	rmdir(dir);
	return true;
    }

    fd = mkstemp(outside_template);
    if (fd < 0) {
	warnp(__func__, "mkstemp failed");
	unlink(allowed_path);
	rmdir(dir);
	return true;
    }
    written = write(fd, sample, sizeof(sample) - 1);
    if (written != (ssize_t)(sizeof(sample) - 1)) {
	warnp(__func__, "write failed");
	close(fd);
	unlink(allowed_path);
	unlink(outside_template);
	rmdir(dir);
	return true;
    }
    if (close(fd) != 0) {
	warnp(__func__, "close failed");
	unlink(allowed_path);
	unlink(outside_template);
	rmdir(dir);
	return true;
    }
    outside_base = strrchr(outside_template, '/');
    if (outside_base == NULL || outside_base[1] == '\0') {
	warn(__func__, "unable to compute outside basename");
	unlink(allowed_path);
	unlink(outside_template);
	rmdir(dir);
	return true;
    }
    if (snprintf(relative_escape, sizeof(relative_escape), "../%s", outside_base + 1) >= (int)sizeof(relative_escape)) {
	warn(__func__, "relative escape path overflow");
	unlink(allowed_path);
	unlink(outside_template);
	rmdir(dir);
	return true;
    }

    if (expect_open_dir_file_rejected(dir, outside_template, 116) == true) {
	failed = true;
    }
    if (expect_open_dir_file_rejected(dir, relative_escape, 116) == true) {
	failed = true;
    }

    if (unlink(outside_template) != 0) {
	warnp(__func__, "unlink failed");
	failed = true;
    }
    if (unlink(allowed_path) != 0) {
	warnp(__func__, "unlink allowed_path failed");
	failed = true;
    }
    if (rmdir(dir) != 0) {
	warnp(__func__, "rmdir failed");
	failed = true;
    }
    return failed;
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program, "missing required argument(s), program: %s");
 *
 * given:
 *	exitcode        value to exit with
 *	prog		our program name
 *	str		top level usage message
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *prog, char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT,
						     PR_TEST_BASENAME, PR_TEST_VERSION,
						     dyn_array_version);
    exit(exitcode); /*ooo*/
    not_reached();
}
