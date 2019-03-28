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

char *test_name = "secboot_p9";


// TODO: excercise the pnor storage load/write here?

int run_test()
{
	struct platform *platform_copy;

	// Duplicate the platform struct, so we can excercise all the weird failure cases
	platform_copy = malloc(sizeof(platform));
	memcpy(platform_copy, &platform, sizeof(platform));

	ASSERT(0 == secvar_init()); // Ideally this shouldn't crash
	ASSERT(1 == secvar_enabled);

	free(secboot_image);
	secboot_image = NULL;

	proc_gen = 0;
	ASSERT(1 == secvar_init());
	ASSERT(0 == secvar_enabled);
	proc_gen = proc_gen_p9;

	platform.secboot_info = NULL;

	ASSERT(-1 == secvar_init());
	ASSERT(0 == secvar_enabled);

	memcpy(&platform, platform_copy, sizeof(platform));

	free(platform_copy);

	return 0;
}
