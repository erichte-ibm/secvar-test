# Override this to set the skiboot root directory somewhere else
SKIBOOT_PATH=../skiboot

# Override this only if libstb isn't where it should be...
LIBSTB=$(SKIBOOT_PATH)/libstb

# Override to "BIG" for testing on big endian
ENDIAN=LITTLE


# Set to 1 to disable the color output
NO_COLOR=0
ifeq ($(NO_COLOR),1)
  BUILD_COLOR=-DNO_COLOR
endif

# Set to empty to build without gcov support
ifneq ($(GCOV), "")
  GCOV_FLAGS=-fprofile-arcs -ftest-coverage
endif


GCOVR=$(shell which gcovr)
ifneq ($(GCOVR), "")
  GCOVR += -r . -s
  GCOVR_HTML=--html --html-details -o html/coverage.html
else
  GCOVR=echo "'gcovr' not installed, not displaying output. Install gcovr or run basic gcov manually."
endif

tests = $(patsubst %.c, %, $(wildcard test_*.c))



all: $(tests)

%: %.c $(LIBSTB)/secboot_p9.c $(LIBSTB)/secboot_p9.h $(LIBSTB)/secvar.c $(LIBSTB)/secvar.h $(LIBSTB)/secvar_api.c test.c
	@echo Building $@...
	@gcc -o $@ $< -g $(GCOV_FLAGS) -I$(SKIBOOT_PATH) -I$(SKIBOOT_PATH)/include -I$(LIBSTB) -DHAVE_$(ENDIAN)_ENDIAN $(BUILD_COLOR)

run: $(addprefix run_, $(tests))

run_%: %
	@dd if=/dev/zero of=secboot.img bs=128k count=1 2> /dev/null
	@mkdir -p log/$</
	@./$< log/$</$<.log
	@rm secboot.img

valgrind: $(addprefix valgrind_, $(tests))

valgrind_%: %
	@dd if=/dev/zero of=secboot.img bs=128k count=1 2> /dev/null
	@mkdir -p log/$</
	@valgrind --log-file=log/$</$<_valgrind.log --error-exitcode=1 ./$< log/$</$<.log >/dev/null && echo "$< passed valgrind"
	@rm secboot.img

# For debug or running individual cases in gdb
secboot:
	@dd if=/dev/zero of=secboot.img bs=128k count=1 2> /dev/null

purge_coverage:
	rm -rf html/ *.gcov *.gcda

coverage: purge_coverage run
	@$(GCOVR)

coverage_html: purge_coverage run
	@mkdir -p html
	@$(GCOVR) $(GCOVR_HTML)


clean: purge_coverage
	rm -rf secboot.img log/ *.gcno
	rm -f $(tests)
