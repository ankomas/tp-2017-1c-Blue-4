################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: memoria

dependents:
	-cd /home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib/Debug && $(MAKE) clean all
	-cd /home/utnso/workspace/tp-2017-1c-Blue-4/commons/Debug && $(MAKE) clean all

# Tool invocations
memoria: $(OBJS) $(USER_OBJS) /home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib/Debug/libblue4-lib.so /home/utnso/workspace/tp-2017-1c-Blue-4/commons/Debug/libcommons.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/tp-2017-1c-Blue-4/commons/Debug" -L"/home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib/Debug" -o "memoria" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) memoria
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib/Debug/libblue4-lib.so:
/home/utnso/workspace/tp-2017-1c-Blue-4/commons/Debug/libcommons.so:

-include ../makefile.targets
