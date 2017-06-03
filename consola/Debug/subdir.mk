################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../conexiones.c \
../consola.c \
../funcionesAuxiliares.c \
../hilos.c \
../operacionesDeConsola.c 

OBJS += \
./conexiones.o \
./consola.o \
./funcionesAuxiliares.o \
./hilos.o \
./operacionesDeConsola.o 

C_DEPS += \
./conexiones.d \
./consola.d \
./funcionesAuxiliares.d \
./hilos.d \
./operacionesDeConsola.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib" -I"/home/utnso/workspace/tp-2017-1c-Blue-4/commons" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


