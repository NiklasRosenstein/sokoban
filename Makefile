FLAGS      :=  -std=c99
FLAGS      +=  -Wall -Wextra -Wpedantic
FLAGS      +=  -Wno-overlength-strings

SOURCEDIR  :=  src
BUILDDIR   :=  build
SCRIPTDIR  :=  scripts
RESDIR     :=  resources
DOCDIR     :=  doc/doc

DOCCONFIG  :=  doc/Doxygen.config

LIBRARIES  :=  sdl2
LIBRARIES  +=  SDL2_image

CFLAGS     :=  $(FLAGS) -x c
CFLAGS     +=  $(shell pkg-config --cflags $(LIBRARIES))

LDFLAGS    :=  $(FLAGS)
LDFLAGS    +=  $(shell pkg-config --libs $(LIBRARIES))

TARGET     :=  sokoban
IMGSCRIPT  :=  b64encode
PRINTSCRIPT := print.sh

IMAGES     :=  free.png
IMAGES     +=  box.png
IMAGES     +=  boxtarget.png
IMAGES     +=  player.png
IMAGES     +=  target.png
IMAGES     +=  wall.png

GENSOURCES := $(addprefix Image_, $(IMAGES:.png=.c))
GENHEADERS := $(addprefix Image_, $(IMAGES:.png=.h))

SOURCES    :=  $(notdir $(wildcard $(SOURCEDIR)/*.c))
SOURCES    +=  $(GENSOURCES)

HEADERS    :=  $(notdir $(wildcard $(SOURCEDIR)/*.h))
HEADERS    +=  $(GENHEADERS)

OBJECTS    :=  $(SOURCES:.c=.o)

TAGS       :=  $(subst $(SOURCEDIR),$(BUILDDIR), $(SOURCES:.c=.tags))
TAGS+      :=  $(subst $(SOURCEDIR),$(BUILDDIR), $(HEADERS:.h=.tags))
TAGFILE    :=  tags


.PHONY: all clean doc debug optimized

all: $(BUILDDIR)/$(TARGET) $(addprefix $(SOURCEDIR)/, $(SOURCES)) $(addprefix $(SOURCEDIR)/, $(HEADERS))
	@#

debug: CFLAGS += -g
debug: LDFLAGS += -g
debug: all

optimized: CFLAGS += -O3
optimized: LDFLAGS += -O3
optimized: all

profiling: CFLAGS += -pg
profiling: LDFLAGS += -pg
profiling: all

doc:
	doxygen $(DOCCONFIG)

$(BUILDDIR)/$(TARGET): $(addprefix $(BUILDDIR)/, $(OBJECTS))
	@/bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [LD]      $@"
	@$(CC) $(LDFLAGS) $^ -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c $(addprefix $(SOURCEDIR)/, $(HEADERS))
	@/bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [CC]      $@"
	@$(CC) $(CFLAGS) -c $< -o $@


$(SCRIPTDIR)/$(IMGSCRIPT): $(SCRIPTDIR)/$(IMGSCRIPT).c
	@/bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [CC]      $@"
	@$(CC) -g -Wall -Wextra -Wpedantic $^ -o $@
	
$(SOURCEDIR)/Image_%.c: $(RESDIR)/%.png $(SCRIPTDIR)/$(IMGSCRIPT)
	@/bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [GEN]     $@"
	@$(SCRIPTDIR)/$(IMGSCRIPT) $< -n $* -c $@

$(SOURCEDIR)/Image_%.h: $(SCRIPTDIR)/$(IMGSCRIPT)
	@/bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [GEN]     $@"
	@$(SCRIPTDIR)/$(IMGSCRIPT) -n $* -h $@



clean: 
	@# Remove script
	@if [[ -f $(SCRIPTDIR)/$(IMGSCRIPT) ]]; then \
	  /bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [CLEAN]   $(SCRIPTDIR)/$(IMGSCRIPT)" ; \
	  rm -f "$(SCRIPTDIR)/$(IMGSCRIPT)" ; \
	fi

	@# Remove generated .h files
	@for header in $(addprefix $(SOURCEDIR)/, $(GENHEADERS)); do \
	  if [[ -f $$header ]]; then \
	    /bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [CLEAN]   $$header" ; \
	    rm -f $$header ; \
	  fi \
	done

	@# Remove generated .c files
	@for source in $(addprefix $(SOURCEDIR)/, $(GENSOURCES)); do \
	  if [[ -f $$source ]]; then \
	    /bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [CLEAN]   $$source" ; \
	    rm -f $$source ; \
	  fi \
	done

	@# Remove executable
	@if [[ -f $(BUILDDIR)/$(TARGET) ]]; then \
	  /bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [CLEAN]   $(BUILDDIR)/$(TARGET)" ; \
	  rm -f $(BUILDDIR)/$(TARGET) ; \
	fi
	
	@# Remove object files
	@for obj in $(wildcard $(BUILDDIR)/*.o); do \
	  /bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [CLEAN]   $$obj" ; \
	  rm -f $$obj ; \
	done
	
