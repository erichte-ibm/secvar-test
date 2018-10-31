# Override this to set the skiboot root directory somewhere else
SKIBOOT_PATH=../skiboot

# Override this only if libstb isn't where it should be...
LIBSTB=$(SKIBOOT_PATH)/libstb

tests = $(patsubst %.c, %, $(wildcard tests/test_*.c))


all: $(tests)

%: %.c $(LIBSTB)/secboot_part.c $(LIBSTB)/secboot_part.h $(LIBSTB)/keystore.c $(LIBSTB)/keystore.h test.c
	@gcc -o $@ $< -g -I$(SKIBOOT_PATH) -I$(SKIBOOT_PATH)/include -I$(LIBSTB) -DHAVE_LITTLE_ENDIAN

run: $(addprefix run_, $(tests))

run_%: %
# TODO: do we actually want to zero out the secboot.img for each test case? -> probably, might as well
	@dd if=/dev/zero of=secboot.img bs=128k count=1 2> /dev/null
	@./$< $<.log
	@rm secboot.img

clean:
	rm -f secboot.img $(tests) *.log
