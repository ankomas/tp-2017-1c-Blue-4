################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../capaKernel.c \
../capaMemoria.c \
../conexiones.c \
../main.c \
../pcb.c \
../primitivas.c \
../primitivasKernel.c \
../test.c 

OBJS += \
./capaKernel.o \
./capaMemoria.o \
./conexiones.o \
./main.o \
./pcb.o \
./primitivas.o \
./primitivasKernel.o \
./test.o 

C_DEPS += \
./capaKernel.d \
./capaMemoria.d \
./conexiones.d \
./main.d \
./pcb.d \
./primitivas.d \
./primitivasKernel.d \
./test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2017-1c-Blue-4/parser" -I"/home/utnso/workspace/tp-2017-1c-Blue-4/commons" -I"/home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


