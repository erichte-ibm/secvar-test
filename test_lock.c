#include "test.c"

char *test_name = "lock";

int run_test(void)
{
	int64_t rc;
	char name[16] = {0};
	char data[16];
	uint64_t size = 16;

	// Base test set up -- these should never fail
	rc = secvar_write("test1", "data", 4, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	
	rc = secvar_commit(ACTIVE_BANK); // This will be used later
	ASSERT(rc == OPAL_SUCCESS);

	rc = secvar_write("test2", "something", 9, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&active_bank_list) == 2);

	// At this point:
	//  - flash count is 1
	//  - in-memory count is 2
	//  reload would normally reset count to 1, but not if lock is engaged

	// Lock the active bank
	rc = secvar_lock(ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(active_lock == 1); // make sure the function does the thing


	// Ensure adding a new variable is rejected
	rc = secvar_write("test3", "foobar", 6, ACTIVE_BANK);
	ASSERT(rc == OPAL_PERMISSION);
	ASSERT(list_length(&active_bank_list) == 2);

	// Ensure updating an existing variable is rejected
	rc = secvar_write("test1", "data", 4, ACTIVE_BANK);
	ASSERT(rc == OPAL_PERMISSION);
	ASSERT(list_length(&active_bank_list) == 2);

	// Ensure reloading is rejected
	rc = secvar_reload();
	ASSERT(rc == OPAL_PERMISSION);
	ASSERT(list_length(&active_bank_list) == 2);

	// Ensure commit is rejected
	rc = secvar_commit(ACTIVE_BANK);
	ASSERT(rc == OPAL_PERMISSION);


	// Lift the lock, so we can confirm nothing was written to flash...
	active_lock = 0;
	rc = secvar_reload();                        // reload the variables...
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&active_bank_list) == 1); // and we should have one less now.


	// Bad section/flag
	rc = secvar_lock(0);
	ASSERT(rc == OPAL_PARAMETER);
	

	// TODO: Verify data is correct? Is it worth it, or does commitreload handle this?	

	return 0;
}
