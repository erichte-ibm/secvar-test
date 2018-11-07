#include "test.c"

char *test_name = "commitreload";

int run_test(void)
{
	int64_t rc;
	char name[16] = {0};
	char data[16];
	uint64_t size = 16;

	// Base test set up -- these should never fail
	rc = secvar_write("test1", "data", 4, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);

	rc = secvar_write("test2", "something", 9, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);

	rc = secvar_write("test3", "foobar", 6, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&active_bank_list) == 3);

	// First confirm that we cannot commit when locked
	// NOTE: we're only testing the return code here
	// run test_lock for a more in-depth lock check
	active_lock = 1;
	rc = secvar_commit(ACTIVE_BANK);
	ASSERT(rc == OPAL_PERMISSION);
	active_lock = 0;


	// Test commit call returns correctly
	rc = secvar_commit(ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);

	clear_bank_list(&active_bank_list);          // Dump the in-memory list
	ASSERT(list_length(&active_bank_list) == 0); // ensure it's actually gone

	// Reload, check list length
	rc = secvar_reload();
	ASSERT(list_length(&active_bank_list) == 3);

	// Scan and verify all the data...
	// first item
	size = 16;
	rc = secvar_next(name, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(name, "test1", 6)); // Include NULL-terminator in check
	size = 16;
	rc = secvar_read(name, data, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(data, "data", 4)); // Data blob, no NULL-terminator

	// second item
	size = 16;
	rc = secvar_next(name, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(name, "test2", 6));
	size = 16;
	rc = secvar_read(name, data, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(data, "something", 9));

	// last item
	size = 16;
	rc = secvar_next(name, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(name, "test3", 6));
	size = 16;
	rc = secvar_read(name, data, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(data, "foobar", 6));


	// Bad section
	rc = opal_secvar_commit(0);
	ASSERT(rc == OPAL_PARAMETER);

	// Re-commit and reload with a smaller list
	// There should not be any artifacts from the previous list state
	rc = secvar_write("test2", NULL, 0, ACTIVE_BANK); // Delete item 2
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&active_bank_list) == 2);

	rc = secvar_commit(ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);

	clear_bank_list(&active_bank_list);
	ASSERT(list_length(&active_bank_list) == 0);

	rc = secvar_reload();	
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&active_bank_list) == 2);

	// Verify the reloaded list again, now without the second item
	// first item
	size = 16;
	memset(name, 0x0, 16); // reset name to empty
	rc = secvar_next(name, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(name, "test1", 6)); // Include NULL-terminator in check
	size = 16;
	rc = secvar_read(name, data, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(data, "data", 4)); // Data blob, no NULL-terminator
	
	// last item
	size = 16;
	rc = secvar_next(name, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(name, "test3", 6));
	size = 16;
	rc = secvar_read(name, data, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(data, "foobar", 6));

	return 0;
}
