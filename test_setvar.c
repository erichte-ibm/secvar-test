#include "test.c"

char *test_name = "setvar";

int run_test(void)
{
	int64_t rc;
	uint64_t size = 4;

	char *buffer = malloc(16);

	/*** GOOD PATH ***/
	// Add variable
	rc = secvar_write("test", "data", 4, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS)

	// Ensure the list contains only that one element.
	ASSERT(list_length(&active_bank_list) == 1);

	// Confirm variable exists and is correct
	rc = secvar_read("test", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);

	ASSERT(!strncmp(buffer, "data", 4));

	// Update variable
	rc = secvar_write("test", "meow", 4, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS)

	rc = secvar_read("test", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);

	ASSERT(!strncmp(buffer, "meow", 4));

	// Confirm a new variable was NOT added to the list
	ASSERT(list_length(&active_bank_list) == 1);

	// Delete the variable
	rc = secvar_write("test", NULL, 0, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);

	// Should fail to find the variable.
	rc = secvar_read("test", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_EMPTY)

	// Confirm list is now empty
	ASSERT(list_length(&active_bank_list) == 0);

	// ....Multiple variable tests....

	rc = secvar_write("test1", "data", 4, ACTIVE_BANK);
	rc = secvar_write("test2", "meow", 4, ACTIVE_BANK);
	rc = secvar_write("test3", "woof", 4, ACTIVE_BANK);

	ASSERT(list_length(&active_bank_list) == 3);

	rc = secvar_read("test1", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(buffer, "data", 4));

	rc = secvar_read("test2", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(buffer, "meow", 4));

	rc = secvar_read("test3", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(buffer, "woof", 4));


	// Delete the middle variable
	rc = secvar_write("test2", NULL, 0, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&active_bank_list) == 2);

	rc = secvar_read("test1", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);

	rc = secvar_read("test2", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_EMPTY); // should "fail" now

	rc = secvar_read("test3", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);

	// Delete the head variable
	rc = secvar_write("test1", NULL, 0, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&active_bank_list) == 1);

	rc = secvar_read("test1", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_EMPTY); // should "fail" now

	rc = secvar_read("test2", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_EMPTY); // should still fail

	rc = secvar_read("test3", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);


	// Add a new tail variable and delete it
	rc = secvar_write("test4", "baaa", 4, ACTIVE_BANK);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&active_bank_list) == 2);

	rc = secvar_write("test4", NULL, 0, ACTIVE_BANK);
	ASSERT(list_length(&active_bank_list) == 1);

	rc = secvar_read("test4", buffer, &size, ACTIVE_BANK);
	ASSERT(rc == OPAL_EMPTY); // should still fail


	free(buffer);

	return 0;
}
