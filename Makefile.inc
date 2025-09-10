mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS += \
	Walker.o \
	LineTracer.o \
	ScenarioTracer.o \
	LineMonitor.o \
	Starter.o \
	SimpleTimer.o \
	Test.o \
	PIDControl.o\
	LineTracer_8.o\
	DistanceMeter.o\
	BottleAvoidance.o\

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile

endif

APPL_DIRS += $(mkfile_path)app $(mkfile_path)unit

INCLUDES += \
	-I$(mkfile_path)app \
	-I$(mkfile_path)unit
