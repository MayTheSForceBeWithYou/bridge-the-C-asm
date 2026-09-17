# Umbrella Makefile — auto-discover numbered exercise directories.

EXERCISES := $(sort $(patsubst %/,%,$(wildcard [0-9][0-9]-*/)))

.PHONY: all clean list $(EXERCISES)

all:
	@for d in $(EXERCISES); do \
		echo "==== $$d ===="; \
		$(MAKE) -C $$d || exit 1; \
	done

clean:
	@for d in $(EXERCISES); do \
		$(MAKE) -C $$d clean; \
	done

list:
	@printf '%s\n' $(EXERCISES)

$(EXERCISES):
	$(MAKE) -C $@

# Numeric shortcuts: make 32 / make 07 from discovered dirs
define MAKE_SHORTCUT
.PHONY: $(1)
$(1):
	@$(MAKE) -C $(2)
endef

$(foreach d,$(EXERCISES),$(eval $(call MAKE_SHORTCUT,$(firstword $(subst -, ,$(d))),$(d))))

# Unpadded only when different from padded (01->1, but 32->32 skip)
define MAKE_SHORTCUT_UNPAD
ifneq ($(1),$(2))
.PHONY: $(1)
$(1):
	@$(MAKE) -C $(3)
endif
endef

$(foreach d,$(EXERCISES),$(eval $(call MAKE_SHORTCUT_UNPAD,$(shell echo $(firstword $(subst -, ,$(d))) | sed 's/^0*\([0-9]\)/\1/;s/^$$/0/'),$(firstword $(subst -, ,$(d))),$(d))))
