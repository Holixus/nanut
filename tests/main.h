
#define PRINT_OK_MESSAGES (1)


#define SUCCESS_MSG  "    [OK] "
#define FAILED_MSG   "    [FAILED] "

#define SUCCESS(args...) { if (PRINT_OK_MESSAGES) printf(SUCCESS_MSG args); return 1; }
#define FAILED(args...)  { printf(FAILED_MSG args); return 0; }

typedef
struct {
	char const *sample;
	char const *result;
} str_str_t;

typedef
struct {
	char const *sample;
	int result;
} str_int_t;


#define TEST_PAIRS(name) \
	{ \
		printf("\nTest " #name "()\n"); \
		for (int i = 0, n = countof(name##_samples); i < n; ++i) { \
			if (!name##_test(name##_samples[i].sample, name##_samples[i].result)) \
				return 0; \
		} \
	}

#define TEST_STRUCT_PAIRS(name) \
	{ \
		printf("\nTest " #name "()\n"); \
		for (int i = 0, n = countof(name##_samples); i < n; ++i) { \
			if (!name##_test(&name##_samples[i].sample, name##_samples[i].result)) \
				return 0; \
		} \
	}


int str_tests();
int url_tests();

