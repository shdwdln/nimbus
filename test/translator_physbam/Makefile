
TARGET = ldo_serialization
all: check $(TARGET)

NIMBUS_ROOT = ../..
include ${NIMBUS_ROOT}/Makeinclude

IFLAGS += -I$(LDIR) -I$(NIMBUS_ROOT)/application/physbam-lib/Public_Library
H_FILES = $(wildcard *.h)
CPP_FILES = $(wildcard *.cc)
OBJECT_FILES = $(subst .cc,.o,$(CPP_FILES))

LFLAGS += -lpthread -lprotobuf -lprotoc -lnimbus -lboost_thread-mt -lboost_system-mt
LFLAGS += -lPhysBAM_Dynamics -lPhysBAM_Fluids -lPhysBAM_Geometry -lPhysBAM_Solids
LFLAGS += -lPhysBAM_Tools -lz
LDFLAGS += -L. -L/usr/local/lib
LDFLAGS += -L$(NIMBUS_ROOT)/application/physbam-lib/Public_Library/Build/Release
LDFLAGS += -L$(NIMBUS_ROOT)/application/physbam-lib/External_Libraries/lib/

# Add the dependencies here
%.o: %.cc
	$(CPP) $(CFLAGS) $(IFLAGS) -c $< -o $@ 

$(TARGET): $(CPP_FILES) $(H_FILES) $(OBJECT_FILES)
	$(CPP) $(CFLAGS) $(IFLAGS) $(LDFLAGS) $(LFLAGS) $(OBJECT_FILES) -o $(TARGET) $(LFLAGS)

clean: clean-files
	\rm -f $(TARGET)
