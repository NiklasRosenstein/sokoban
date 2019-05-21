CC         :=  clang

FLAGS      :=  -std=c99 -g
FLAGS      +=  -Wall -Wextra -Wpedantic
FLAGS      +=  -Wno-overlength-strings

SOURCEDIR  :=  src
BUILDDIR   :=  build
SCRIPTDIR  :=  scripts
RESDIR     :=  resources

LIBRARIES  :=  sdl2
LIBRARIES  +=  SDL2_image

CFLAGS     :=  $(FLAGS) -x c
CFLAGS     +=  $(shell pkg-config --cflags $(LIBRARIES))

LDFLAGS    :=  $(FLAGS)
LDFLAGS    +=  $(shell pkg-config --libs $(LIBRARIES))

TARGET     :=  sokoban
IMGSCRIPT  :=  b64encode
PRINTSCRIPT := print.sh

IMAGES     :=  player.png
IMAGES     +=  wall.png
IMAGES     +=  box.png

GENSOURCES := $(addprefix image_, $(IMAGES:.png=.c))
GENHEADERS := $(addprefix image_, $(IMAGES:.png=.h))

SOURCES    :=  $(notdir $(wildcard $(SOURCEDIR)/*.c))
SOURCES    +=  $(GENSOURCES)

HEADERS    :=  $(notdir $(wildcard $(SOURCEDIR)/*.h))
HEADERS    +=  $(GENHEADERS)

OBJECTS    :=  $(SOURCES:.c=.o)

TAGS       :=  $(subst $(SOURCEDIR),$(BUILDDIR), $(SOURCES:.c=.tags))
TAGS+      :=  $(subst $(SOURCEDIR),$(BUILDDIR), $(HEADERS:.h=.tags))
TAGFILE    :=  tags


.PHONY: all clean

all: $(BUILDDIR)/$(TARGET) $(addprefix $(SOURCEDIR)/, $(SOURCES)) $(addprefix $(SOURCEDIR)/, $(HEADERS))



$(BUILDDIR)/$(TARGET): $(addprefix $(BUILDDIR)/, $(OBJECTS))
	@#echo " [LD]      $@"
	@/bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [LD]      $@"
	@$(CC) $(LDFLAGS) $^ -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c $(addprefix $(SOURCEDIR)/, $(HEADERS))
	@#echo " [CC]      $@"
	@/bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [CC]      $@"
	@$(CC) $(CFLAGS) -c $< -o $@


$(SCRIPTDIR)/$(IMGSCRIPT): $(SCRIPTDIR)/$(IMGSCRIPT).c
	@#echo " [CC]      $@"
	@/bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [CC]      $@"
	@$(CC) -Wall -Wextra -Wpedantic $^ -o $@
	
$(SOURCEDIR)/image_%.c: $(RESDIR)/%.png $(SCRIPTDIR)/$(IMGSCRIPT)
	@#echo " [GEN]     $@"
	@/bin/sh $(SCRIPTDIR)/$(PRINTSCRIPT) " [GEN]     $@"
	@$(SCRIPTDIR)/$(IMGSCRIPT) $< -n $* -c $@

$(SOURCEDIR)/image_%.h: $(SCRIPTDIR)/$(IMGSCRIPT)
	@#echo " [GEN]     $@"
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
	
