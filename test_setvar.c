#include "test.c"

char *test_name = "foo";

int run_test(void)
{
	int64_t rc;
	uint64_t size = 4;

	char *buffer = malloc(16);

	rc = secvar_write("test", "data", 4, ACTIVE_BANK);
	if (rc != OPAL_SUCCESS)
		return rc;

	rc = secvar_read(buffer, &size, "test", ACTIVE_BANK);
	if (rc != OPAL_SUCCESS)
		return rc;

	if (strncmp(buffer, "data", 4))
		return -1;

	return rc;

}
