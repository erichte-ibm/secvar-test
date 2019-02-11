#include "test.c"

char *test_name = "enqueue";

int run_test(void)
{
	int64_t rc;

	struct secvar *var;
	wchar_t name[1024] = {0};
	char vendor[16] = {0};
	uint32_t attributes = 0;
	uint64_t data_size = 128;
	char *data = zalloc(data_size);

	/*** Bad cases first this time ***/
	// Parameter checks
	// null name
	rc = opal_secvar_enqueue(NULL, vendor, attributes, data_size, data);
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

	memset(name, 1, sizeof(name));
	rc = opal_secvar_enqueue(name, vendor, attributes, data_size, data);
	ASSERT(rc == OPAL_PARAMETER);
	memset(name, 0, sizeof(name));

	secvar_enabled = 0;
	rc = opal_secvar_enqueue(name, vendor, attributes, data_size, data);
	ASSERT(rc == OPAL_HARDWARE);
	secvar_enabled = 1;


	/*** Good cases ***/
	// TODO: add data?
	memcpy(name, L"test", 4*2);
	rc = opal_secvar_enqueue(name, vendor, attributes, data_size, data);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(list_length(&update_bank) == 1);

	memcpy(name, L"foobar", 6*2);
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
	ASSERT(!memcmp(var->name, L"test", 4*2));
	var = list_next(&update_bank, var, link);
	ASSERT(var);
	ASSERT(!memcmp(var->name, L"foobar", 6*2));

	return 0;

}
