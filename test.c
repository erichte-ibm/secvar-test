/* Copyright 2019 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define SECBOOT_FILE "secboot.img"
#define SECBOOT_SIZE 128000

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ccan/list/list.h>
#include <skiboot.h>
#include <stdarg.h>

// Force p9
enum proc_gen proc_gen = proc_gen_p9;

// Replace memalign with regular old malloc
#define memalign(a, b) malloc(b)
#define zalloc(a) calloc(1, a)

/**** Stubs for translating PNOR I/O -> File I/O ****/
int secboot_info(uint32_t *total_size)
{
	*total_size = 128000;

	return 0;
}

int secboot_read(void *dst, uint32_t src, uint32_t len)
{
	FILE *fd;

	fd = fopen(SECBOOT_FILE, "r");

	if (!fd)
		return -1;

	fread(dst, 1, len, fd);

	fclose(fd);

	return 0;
}

int secboot_write(uint32_t dst, void *src, uint32_t len)
{
	FILE *fd;

	fd = fopen(SECBOOT_FILE, "w");

	if (!fd)
		return -1;

	fwrite(src, 1, len, fd);

	fclose(fd);

	return 0;
}

struct platform platform = {
	.secboot_info = secboot_info,
	.secboot_read = secboot_read,
	.secboot_write = secboot_write,
};

// Hack to include the code we actually want to test here...
#include "secboot_p9.c"
#include "secvar_api.c"
#include "secvar.c"

// For log file output instead of stdout
FILE *outfile;

// Define the log printing function so skiboot is happy...
void _prlog(int log_level, const char* fmt, ...)
{
	va_list args;
	fprintf(outfile, fmt, args); 
}

/**** Helper wrappers, so the caller doesn't have to cast ****/

static int64_t secvar_get(char16_t *k_name, char *k_vendor, uint32_t *k_attributes, size_t *k_data_size, void *k_data)
{
	return opal_secvar_get((uint64_t) k_name,
				(uint64_t) k_vendor,
				(uint64_t) k_attributes,
				(uint64_t) k_data_size,
				(uint64_t) k_data);
}

static int64_t secvar_get_next(size_t *k_name_size, char16_t *k_name, char *k_vendor)
{

	return opal_secvar_get_next((uint64_t) k_name_size,
					(uint64_t) k_name,
					(uint64_t) k_vendor);
}



static int64_t secvar_enqueue(char16_t *k_name, char *k_vendor, uint32_t k_attributes, size_t k_data_size, void *k_data)
{
	return opal_secvar_enqueue((uint64_t) k_name,
					(uint64_t) k_vendor,
					(uint64_t) k_attributes,
					(uint64_t) k_data_size,
					(uint64_t) k_data);

}

// To be defined by test case
int run_test(void);
char *test_name;

#ifndef NO_COLOR
#define COLOR_RED	"\033[0;31m"
#define COLOR_GREEN	"\033[1;32m"
#define COLOR_RESET	"\033[0m"
#else
#define COLOR_RED	""
#define COLOR_GREEN	""
#define COLOR_RESET	""
#endif

// Helper functions and macros to make test case writing easier

// Semi-configurable assert, can use to jump to a clean up step on fail
#define ASSERT_POST(a,b) if(!(a)){fprintf(stdout, "Assert '%s' failed at %s:%d...", #a, __FILE__, __LINE__);b;}
#define ASSERT(a) ASSERT_POST(a, return 1)


static int list_length(struct list_head *head)
{
	struct secvar *var;
	int i = 0;

	list_for_each(head, var, link) {
		i++;
	}

	return i;
}


// Entry point
// TODO: do some real argparsing
int main(int argc, char **argv)
{
	int ret;

	if (argc > 1)
		outfile = fopen(argv[1], "w");
	else
		outfile = stdout;

	ret = secvar_init();

	if (ret) {
		fprintf(stderr, "Something went wrong setting up the keystore...\n");
		return ret;
	}

	printf("Running test '%s'...", test_name);
	ret = run_test();
	if (ret)
		printf(COLOR_RED "FAILED" COLOR_RESET "\n");
	else
		printf(COLOR_GREEN "OK" COLOR_RESET "\n");

	if (outfile != stdout)
		fclose(outfile);


	// Clean up for the test cases
	clear_bank_list(&variable_bank);
	clear_bank_list(&update_bank);
	free(secboot_image);

	return ret;
}
