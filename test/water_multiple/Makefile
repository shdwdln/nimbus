
#################################
# ENTER application target here #
#################################

TARGET_WATER = worker

##########
# common #
##########

default: release

NIMBUS_ROOT = ../..
include ${NIMBUS_ROOT}/Makeinclude

H_FILES       = $(wildcard *.h)
CPP_FILES     = $(wildcard *.cc)
OBJECT_FILES  = $(subst .cc,.o,$(CPP_FILES))

APP_LIB        = -lwater_app
debug: APP_LIB = -lwater_app-debug-debug

####################################
# ENTER application directory here #
####################################

release: APP_DIR = $(NIMBUS_ROOT)/applications/physbam/water/Build/Release
debug:   APP_DIR = $(NIMBUS_ROOT)/applications/physbam/water/Build/Debug

####################
# ENTER flags here #
####################

LFLAGS  += $(APP_LIB)
# LFLAGS  += -Wl,-Bstatic $(APP_LIB) -Wl,-Bdynamic -lnimbus -lprotobuf -lboost_thread-mt -lboost_system-mt -lpthread
LDFLAGS += -L$(APP_DIR) -Wl,-rpath $(APP_DIR)

#############################
# add the dependencies here #
#############################

%.o: %.cc
	$(CPP) $(CFLAGS) $(IFLAGS) -c $< -o $@

####################################
# ENTER rules to build target here #
####################################

release: executable

debug: executable

executable: $(CPP_FILES) $(H_FILES) $(OBJECT_FILES)
	$(CPP) $(CFLAGS) $(IFLAGS) $(LDFLAGS) $(LFLAGS) $(OBJECT_FILES) -o $(TARGET_WATER) $(LFLAGS)

clean: clean-files
	\rm -f $(TARGET_WATER)

clean-logs:
	\rm -f *log*
	\rm -f *.txt
	\rm -rf _db_*

