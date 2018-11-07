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

	size = 16;
	secvar_next(buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(strncmp(buffer, "test1", 5));


	return 0;
}
