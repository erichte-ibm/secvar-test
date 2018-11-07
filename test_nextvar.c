#include "test.c"

char *test_name = "nextvar";

int run_test(void)
{
	char buffer[16] = "data";
	uint64_t size = 4;
	int64_t rc;

	// Load up the bank with some variables.
	// If these fail, we have bigger issues.
	rc = secvar_write("test1", buffer, size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	rc = secvar_write("test2", buffer, size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	rc = secvar_write("test3", buffer, size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&active_bank_list) == 3);

	// Test sequential nexts
	size = 16;
	memset(buffer, 0x0, 16);
	// first item
	rc = secvar_next(buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(buffer, "test1", 5));

	// second item
	rc = secvar_next(buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(buffer, "test2", 5));

	// last item
	rc = secvar_next(buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(buffer, "test3", 5));

	// end-of-list
	rc = secvar_next(buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_EMPTY);


	/*** Time for a break to test bad parameters ***/
	// NULL "return" buffer
	size = 16;
	rc = secvar_next(NULL, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_PARAMETER);

	// NULL size pointer
	rc = secvar_next(buffer, NULL, ACTIVE_BANK);
	ASSERT(rc == OPAL_PARAMETER);

	// zero size
	size = 0;
	rc = secvar_next(buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_PARAMETER);

	// Non-existing previous variable
	size = 16;
	strncpy(buffer, "foobar", 7);
	rc = secvar_next(buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_PARAMETER);

	// Bad section
	rc = secvar_next(buffer, &size, 0);
	ASSERT(rc == OPAL_PARAMETER);

	// Insufficient buffer size
	size = 1;
	strncpy(buffer, "test1", 6);
	rc = secvar_next(buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_PARTIAL);

	return 0;
}
