/* Copyright 2019 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "test.c"

char *test_name = "getvar";

// Run tests on the less obvious features of secvar_get
// Includes:
//  - Partial reads
//  - Size queries (NULL buffer)
//int run_test_helper(uint64_t bank_enum)
int run_test(void)
{
	int64_t rc;
	uint64_t size = 4;

	struct secvar var = {0};
	wchar_t name[1024] = {0};
	char vendor[16] = {0}; // TODO: copy real vendor here for a real test
	uint32_t attributes = 0;
	char *data = zalloc(16);
	var.data = zalloc(16);
	memcpy(name, L"test", 4*2);

	// List should be empty at start
	rc = secvar_get(name, vendor, &attributes, &size, data);
	ASSERT(rc == OPAL_EMPTY);
	ASSERT(list_length(&variable_bank) == 0);

	// Manually add variables, and check get_variable call
	memcpy(var.name, name, sizeof(L"test"));
	memcpy(var.vendor, vendor, sizeof(vendor));
	var.attributes = 27;
	var.data_size = 16;
	memcpy(var.data, "foobar", sizeof("foobar"));
	list_add_tail(&variable_bank, &var.link);

	ASSERT(list_length(&variable_bank) == 1);

	// Test variable size query
	size = 0;
	rc = secvar_get(name, vendor, &attributes, &size, NULL);
	ASSERT(rc == OPAL_SUCCESS);
	ASSERT(size == 16);
	// TODO: assert attributes?

	// Test actual variable get
	rc = secvar_get(name, vendor, &attributes, &size, data);
	ASSERT(0 == memcmp(name, var.name, sizeof(var.name)));
	ASSERT(0 == memcmp(vendor, var.vendor, sizeof(var.vendor)));
	ASSERT(27 == attributes);
	ASSERT(16 == size);
	ASSERT(0 == memcmp(data, var.data, size));

	// Test buffer too small
	size = 14;
	rc = secvar_get(name, vendor, &attributes, &size, data);
	ASSERT(rc == OPAL_PARTIAL);
	ASSERT(size == 16);

	/**** Error/Bad param cases ****/
	// NULL name
	rc = secvar_get(NULL, vendor, &attributes, &size, data);
	ASSERT(rc == OPAL_PARAMETER);
	// NULL vendor
	rc = secvar_get(name, NULL, &attributes, &size, data);
	ASSERT(rc == OPAL_PARAMETER);
	// NULL size
	rc = secvar_get(name, vendor, &attributes, NULL, data);
	ASSERT(rc == OPAL_PARAMETER);
	// nonzero size, but NULL data
	size = 16;
	rc = secvar_get(name, vendor, &attributes, &size, NULL);
	ASSERT(rc == OPAL_PARAMETER);
	// zero size, NULL data
	size = 0;
	rc = secvar_get(name, vendor, &attributes, &size, data);
	ASSERT(rc == OPAL_PARAMETER);

	size = 16;
	secvar_enabled = 0;
	rc = secvar_get(name, vendor, &attributes, &size, data);
	ASSERT(rc == OPAL_HARDWARE);

	secvar_enabled = 1;
	memset(name, 1, sizeof(name));
	rc = secvar_get(name, vendor, &attributes, &size, data);


	list_del(&var.link); // Clean up manually because we used the stack here

	free(data);
	free(var.data);	

	return 0;
}

