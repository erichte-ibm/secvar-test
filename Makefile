# Override this to set the skiboot root directory somewhere else
SKIBOOT_PATH=../skiboot

# Override this only if libstb isn't where it should be...
LIBSTB=$(SKIBOOT_PATH)/libstb

# Override to "BIG" for testing on big endian
ENDIAN=LITTLE

# Set to literally anything else to disable the color output
COLOR=1
ifeq ($(COLOR),1)
  BUILD_COLOR=
else
  BUILD_COLOR=-DNO_COLOR
endif


tests = $(patsubst %.c, %, $(wildcard test_*.c))


all: $(tests)

%: %.c $(LIBSTB)/secboot_part.c $(LIBSTB)/secboot_part.h $(LIBSTB)/keystore.c $(LIBSTB)/keystore.h test.c
	@gcc -o $@ $< -g -I$(SKIBOOT_PATH) -I$(SKIBOOT_PATH)/include -I$(LIBSTB) -DHAVE_$(ENDIAN)_ENDIAN $(BUILD_COLOR)

run: $(addprefix run_, $(tests))

run_%: %
# TODO: do we actually want to zero out the secboot.img for each test case? -> probably, might as well
	@dd if=/dev/zero of=secboot.img bs=128k count=1 2> /dev/null
	@./$< $<.log
	@rm secboot.img

clean:
	rm -f secboot.img $(tests) *.log
