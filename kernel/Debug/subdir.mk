################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../capaFs.c \
../capaMemoria.c \
../consolaKernel.c \
../heap.c \
../heapNico.c \
../main.c \
../pcb.c \
../planificador.c \
../sockets.c 

OBJS += \
./capaFs.o \
./capaMemoria.o \
./consolaKernel.o \
./heap.o \
./heapNico.o \
./main.o \
./pcb.o \
./planificador.o \
./sockets.o 

C_DEPS += \
./capaFs.d \
./capaMemoria.d \
./consolaKernel.d \
./heap.d \
./heapNico.d \
./main.d \
./pcb.d \
./planificador.d \
./sockets.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


