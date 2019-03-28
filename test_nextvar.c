#include "test.c"

char *test_name = "nextvar";

int run_test(void)
{
	int64_t rc;

	struct secvar *tmpvar;

	wchar_t name[1024] = {0};
	char vendor[1024] = {0};	// TODO: use actual vendor here
	uint32_t attributes = 0;
	uint64_t size = 16;


	// Load up the bank with some variables.
	// If these fail, we have bigger issues.
	ASSERT(list_length(&variable_bank) == 0);
	tmpvar = zalloc(sizeof(struct secvar));
	memcpy(tmpvar->name, L"test1", 5*2);
	tmpvar->attributes = 27;
	tmpvar->data_size = 16;
	tmpvar->data = zalloc(16); // unused
	list_add_tail(&variable_bank, &tmpvar->link);
	ASSERT(list_length(&variable_bank) == 1);

	tmpvar = zalloc(sizeof(struct secvar));
	memcpy(tmpvar->name, L"test2", 5*2);
	tmpvar->attributes = 32;
	tmpvar->data_size = 10;
	tmpvar->data = zalloc(10); // unused
	list_add_tail(&variable_bank, &tmpvar->link);
	ASSERT(list_length(&variable_bank) == 2);

	tmpvar = zalloc(sizeof(struct secvar));
	memcpy(tmpvar->name, L"test3", 5*2);
	tmpvar->attributes = 16;
	tmpvar->data_size = 32;
	tmpvar->data = zalloc(32); // unused
	list_add_tail(&variable_bank, &tmpvar->link);
	ASSERT(list_length(&variable_bank) == 3);

	// Test sequential nexts
	// first item
	size = sizeof(name);
	memset(name, 0, sizeof(name));
	rc = secvar_get_next(&size, name, vendor);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!memcmp(name, L"test1", 5*2));

	// second item
	rc = secvar_get_next(&size, name, vendor);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!memcmp(name, L"test2", 5*2));

	// last item
	rc = secvar_get_next(&size, name, vendor);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(!memcmp(name, L"test3", 5*2));

	// end-of-list
	rc = secvar_get_next(&size, name, vendor);
	ASSERT(rc == OPAL_EMPTY);


	memset(name, 0, sizeof(name));
	/*** Time for a break to test bad parameters ***/
	// null name
	rc = secvar_get_next(&size, NULL, vendor);
	ASSERT(rc == OPAL_PARAMETER);
	// null vendor
	rc = secvar_get_next(&size, name, NULL);
	ASSERT(rc == OPAL_PARAMETER);
	// Size too small
	size = 1;
	rc = secvar_get_next(&size, name, vendor);
	ASSERT(rc == OPAL_PARTIAL);
	ASSERT(size == 10);

	// NULL size pointer
	rc = secvar_get_next(NULL, name, vendor);
	ASSERT(rc == OPAL_PARAMETER);

	// zero size
	size = 0;
	rc = secvar_get_next(&size, name, vendor);
	ASSERT(rc == OPAL_PARAMETER);

	// Non-existing previous variable
	size = 1024;
	memcpy(name, L"foobar", 7*2);
	rc = secvar_get_next(&size, name, vendor);
	ASSERT(rc == OPAL_PARAMETER);

	memset(name, 1, sizeof(name));
	rc = secvar_get_next(&size, name, vendor);
	ASSERT(rc == OPAL_PARAMETER);

	secvar_enabled = 0;
	rc = secvar_get_next(&size, name, vendor);
	ASSERT(rc == OPAL_HARDWARE);

	clear_bank_list(&variable_bank);

	return 0;
}
