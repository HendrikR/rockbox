#             __________               __   ___.
#   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
#   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
#   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
#   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
#                     \/            \/     \/    \/            \/
# $Id$
#

# libxmp
XMPLIB := $(CODECDIR)/libxmp.a
XMPLIB_SRC := $(call preprocess, $(RBCODECLIB_DIR)/codecs/libxmp/SOURCES)
XMPLIB_OBJ := $(call c2obj, $(XMPLIB_SRC))
OTHER_SRC += $(XMPLIB_SRC)
CODECLDFLAGS += -lm

$(XMPLIB): $(XMPLIB_OBJ)
	$(SILENT)$(shell rm -f $@)
	$(call PRINTS,AR $(@F))$(AR) rcs $@ $^ >/dev/null
