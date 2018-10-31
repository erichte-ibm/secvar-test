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


tests = $(patsubst %.c, %, $(wildcard test_*.c))


all: $(tests)

%: %.c $(LIBSTB)/secboot_part.c $(LIBSTB)/secboot_part.h $(LIBSTB)/keystore.c $(LIBSTB)/keystore.h test.c
	@gcc -o $@ $< -g -I$(SKIBOOT_PATH) -I$(SKIBOOT_PATH)/include -I$(LIBSTB) -DHAVE_$(ENDIAN)_ENDIAN $(BUILD_COLOR)

run: $(addprefix run_, $(tests))

run_%: %
	@dd if=/dev/zero of=secboot.img bs=128k count=1 2> /dev/null
	@./$< $<.log
	@rm secboot.img

valgrind: $(addprefix valgrind_, $(tests))

valgrind_%: %
	@dd if=/dev/zero of=secboot.img bs=128k count=1 2> /dev/null
	@valgrind --log-file=$<_valgrind.log --error-exitcode=1 ./$< $<.log >/dev/null && echo "$< passed valgrind"
	@rm secboot.img

clean:
	rm -f secboot.img $(tests) *.log
