#include "test.c"

char *test_name = "getvar";

// Run tests on the less obvious features of secvar_read
// Includes:
//  - Partial reads
//  - Size queries (NULL buffer)
int run_test(void)
{
	int64_t rc;
	uint64_t size = 4;

	char *buffer = malloc(16);
	memset(buffer, 0x0, 16);

	// Base test set up
	rc = secvar_write("test", "data", 4, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&active_bank_list) == 1);

	// Test regular read
	size = 16;
	rc = secvar_read("test", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(size == 4);
	ASSERT(!strncmp(buffer, "data", 4));

	// Test partial read
	memset(buffer, 0x0, 16); // Clear so we can correctly see only two bytes read
	size = 2;
	rc = secvar_read("test", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_PARTIAL);
	ASSERT(size == 2);
	ASSERT(!strncmp(buffer, "da", 2));  // This should match
	ASSERT(strncmp(buffer, "data", 4)); // This should NOT

	// Test variable size query
	memset(buffer, 0x0, 16);
	size = 0;
	rc = secvar_read("test", NULL, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(size == 4);


	/**** Error/Bad param cases ****/
	// NULL varsize
	rc = secvar_read("test", buffer, NULL, ACTIVE_BANK);
	ASSERT(rc == OPAL_PARAMETER);

	// non-NULL buffer and zero varsize
	size = 0;
	rc = secvar_read("test", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_PARAMETER);
	
	// Bad section
	size = 4; // make this valid again
	rc = secvar_read("test", buffer, &size, 0);
	ASSERT(rc == OPAL_PARAMETER);

	free(buffer);

	return 0;
}
