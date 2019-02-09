#include "test.c"

char *test_name = "enqueue";

int run_test(void)
{
	int64_t rc;

	struct secvar *var;
	char name[1024] = {0};
	char vendor[128] = {0};
	uint32_t attributes = 0;
	uint64_t data_size = 128;
	char *data = zalloc(data_size);

	/*** Bad cases first this time ***/
	// Parameter checks
	// null name
	rc =opal_secvar_enqueue(NULL, vendor, attributes, data_size, data);
	ASSERT(rc == OPAL_PARAMETER);
	ASSERT(list_empty(&update_bank));

	// name is empty
	rc = opal_secvar_enqueue(name, vendor, attributes, data_size, data);
	ASSERT(rc == OPAL_PARAMETER);
	ASSERT(list_empty(&update_bank));

	// null vendor
	name[0] = 'a'; // fix "empty names"
	rc = opal_secvar_enqueue(name, NULL, attributes, data_size, data);
	ASSERT(rc == OPAL_PARAMETER);
	ASSERT(list_empty(&update_bank));

	// null data
	rc = opal_secvar_enqueue(name, vendor, attributes, data_size, NULL);
	ASSERT(rc == OPAL_PARAMETER);
	ASSERT(list_empty(&update_bank));

	// oddball -- we don't support deletion (yet?)
	rc = opal_secvar_enqueue(name, vendor, attributes, 0, data);
	ASSERT(rc == OPAL_UNSUPPORTED);
	ASSERT(list_empty(&update_bank));


	/*** Good cases ***/
	// TODO: add data?
	strncpy(name, "test", 4);
	rc = opal_secvar_enqueue(name, vendor, attributes, data_size, data);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&update_bank) == 1);

	strncpy(name, "foobar", 6);
	rc = opal_secvar_enqueue(name, vendor, attributes, data_size, data);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&update_bank) == 2);

	// Empty the in-memory cache, and reload from "pnor"
	clear_bank(&update_bank);
	ASSERT(list_empty(&update_bank));
	secvar_load_update_bank(&update_bank);
	ASSERT(list_length(&update_bank) == 2);

	var = list_top(&update_bank, struct secvar, link);
	ASSERT(var);
	ASSERT(!strncmp(var->name, "test", 4));
	var = list_next(&update_bank, var, link);
	ASSERT(var);
	ASSERT(!strncmp(var->name, "foobar", 6));

	return 0;

}
