################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../conexiones.c \
../main.c \
../primitivas.c 

OBJS += \
./conexiones.o \
./main.o \
./primitivas.o 

C_DEPS += \
./conexiones.d \
./main.d \
./primitivas.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2017-1c-Blue-4/parser" -I"/home/utnso/workspace/tp-2017-1c-Blue-4/commons" -I"/home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


