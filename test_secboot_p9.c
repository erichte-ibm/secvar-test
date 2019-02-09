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

	free(secboot_image);
	secboot_image = NULL;

	proc_gen = 0;
	ASSERT(1 == secvar_init());
	proc_gen = proc_gen_p9;

	platform.secboot_info = NULL;

	ASSERT(-1 == secvar_init());

	memcpy(&platform, platform_copy, sizeof(platform));

	free(platform_copy);

	return 0;
}
