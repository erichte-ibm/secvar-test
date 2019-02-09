#include "test.c"

char *test_name = "nextvar";

int run_test(void)
{
	int64_t rc;

	struct secvar *tmpvar;

	char name[1024] = {0};
	char vendor[1024] = {0};	// TODO: use actual vendor here
	uint32_t attributes = 0;
	uint64_t size = 16;


	// Load up the bank with some variables.
	// If these fail, we have bigger issues.
	ASSERT(list_length(&active_bank) == 0);
	tmpvar = zalloc(sizeof(struct secvar));
	strncpy(tmpvar->name, "test1", 5);
	tmpvar->attributes = 27;
	tmpvar->data_size = 16;
	tmpvar->data = zalloc(16); // unused
	list_add_tail(&active_bank, &tmpvar->link);
	ASSERT(list_length(&active_bank) == 1);

	tmpvar = zalloc(sizeof(struct secvar));
	strncpy(tmpvar->name, "test2", 5);
	tmpvar->attributes = 32;
	tmpvar->data_size = 10;
	tmpvar->data = zalloc(10); // unused
	list_add_tail(&active_bank, &tmpvar->link);
	ASSERT(list_length(&active_bank) == 2);

	tmpvar = zalloc(sizeof(struct secvar));
	strncpy(tmpvar->name, "test3", 5);
	tmpvar->attributes = 16;
	tmpvar->data_size = 32;
	tmpvar->data = zalloc(32); // unused
	list_add_tail(&active_bank, &tmpvar->link);
	ASSERT(list_length(&active_bank) == 3);

	// Test sequential nexts
	// first item
	size = sizeof(name);
	memset(name, 0, sizeof(name));
	rc = opal_secvar_read_next(&size, name, vendor);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(name, "test1", 5));

	// second item
	rc = opal_secvar_read_next(&size, name, vendor);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(name, "test2", 5));

	// last item
	rc = opal_secvar_read_next(&size, name, vendor);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!strncmp(name, "test3", 5));

	// end-of-list
	rc = opal_secvar_read_next(&size, name, vendor);
	ASSERT(rc == OPAL_EMPTY);


	memset(name, 0, sizeof(name));
	/*** Time for a break to test bad parameters ***/
	// null name
	rc = opal_secvar_read_next(&size, NULL, vendor);
	ASSERT(rc == OPAL_PARAMETER);
	// null vendor
	rc = opal_secvar_read_next(&size, name, NULL);
	ASSERT(rc == OPAL_PARAMETER);
	// Size too small
	size = 16;
	rc = opal_secvar_read_next(&size, name, vendor);
	ASSERT(rc == OPAL_PARTIAL);
	ASSERT(size == sizeof(name));

	// NULL size pointer
	rc = opal_secvar_read_next(NULL, name, vendor);
	ASSERT(rc == OPAL_PARAMETER);

	// zero size
	size = 0;
	rc = opal_secvar_read_next(&size, name, vendor);
	ASSERT(rc == OPAL_PARAMETER);

	// Non-existing previous variable
	size = 1024;
	strncpy(name, "foobar", 7);
	rc = opal_secvar_read_next(&size, name, vendor);
	ASSERT(rc == OPAL_PARAMETER);

	clear_bank(&active_bank);

	return 0;
}
