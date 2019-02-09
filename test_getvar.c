#include "test.c"

char *test_name = "getvar";

// Run tests on the less obvious features of secvar_read
// Includes:
//  - Partial reads
//  - Size queries (NULL buffer)
//int run_test_helper(uint64_t bank_enum)
int run_test(void)
{
	int64_t rc;
	uint64_t size = 4;

	struct secvar var = {0};
	char name[1024] = {0};
	char vendor[128] = {0}; // TODO: copy real vendor here for a real test
	uint32_t attributes = 0;
	char *data = zalloc(16);
	var.data = zalloc(16);
	strcpy(name, "test");

	// List should be empty at start
	rc = opal_secvar_read(name, vendor, &attributes, &size, data);
	ASSERT(rc == OPAL_EMPTY);
	ASSERT(list_length(&active_bank) == 0);

	// Manually add variables, and check get_variable call
	strcpy(var.name, name);
	memcpy(var.vendor, vendor, sizeof(vendor));
	var.attributes = 27;
	var.data_size = 16;
	memcpy(var.data, "foobar", sizeof("foobar"));
	list_add_tail(&active_bank, &var.link);

	ASSERT(list_length(&active_bank) == 1);
	
	// Test variable size query
	size = 0;
	rc = opal_secvar_read(name, vendor, &attributes, &size, NULL);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(size == 16);
	// TODO: assert attributes?

	// Test actual variable get
	rc = opal_secvar_read(name, vendor, &attributes, &size, data);
	ASSERT(0 == memcmp(name, var.name, sizeof(var.name)));
	ASSERT(0 == memcmp(vendor, var.vendor, sizeof(var.vendor)));
	ASSERT(27 == attributes);
	ASSERT(16 == size);
	ASSERT(0 == memcmp(data, var.data, size));

	// Test buffer too small
	size = 14;
	rc = opal_secvar_read(name, vendor, &attributes, &size, data);
	ASSERT(rc == OPAL_PARTIAL);
	ASSERT(size == 16);

	/**** Error/Bad param cases ****/
	// NULL name
	rc = opal_secvar_read(NULL, vendor, &attributes, &size, data);
	ASSERT(rc == OPAL_PARAMETER);
	// NULL vendor
	rc = opal_secvar_read(name, NULL, &attributes, &size, data);
	ASSERT(rc == OPAL_PARAMETER);
	// NULL size
	rc = opal_secvar_read(name, vendor, &attributes, NULL, data);
	ASSERT(rc == OPAL_PARAMETER);
	// nonzero size, but NULL data
	size = 16;
	rc = opal_secvar_read(name, vendor, &attributes, &size, NULL);
	ASSERT(rc == OPAL_PARAMETER);
	// zero size, NULL data
	size = 0;
	rc = opal_secvar_read(name, vendor, &attributes, &size, data);
	ASSERT(rc == OPAL_PARAMETER);	

	list_del(&var.link); // Clean up manually because we used the stack here
	
	free(data);
	free(var.data);	

	return 0;
}

