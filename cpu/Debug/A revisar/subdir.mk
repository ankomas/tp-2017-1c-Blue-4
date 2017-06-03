################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../A\ revisar/pcb.c \
../A\ revisar/primitivas.c 

OBJS += \
./A\ revisar/pcb.o \
./A\ revisar/primitivas.o 

C_DEPS += \
./A\ revisar/pcb.d \
./A\ revisar/primitivas.d 


# Each subdirectory must supply rules for building sources it contributes
A\ revisar/pcb.o: ../A\ revisar/pcb.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2017-1c-Blue-4/commons" -I"/home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"A revisar/pcb.d" -MT"A\ revisar/pcb.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

A\ revisar/primitivas.o: ../A\ revisar/primitivas.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2017-1c-Blue-4/commons" -I"/home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"A revisar/primitivas.d" -MT"A\ revisar/primitivas.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


