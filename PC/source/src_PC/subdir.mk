################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/src_PC/compare_funcs.c \
../source/src_PC/pc_compatibility.c \
../source/src_PC/timer_funcs.c \
../source/src_PC/uart_light_funcs.c 

OBJS += \
./source/src_PC/compare_funcs.o \
./source/src_PC/pc_compatibility.o \
./source/src_PC/timer_funcs.o \
./source/src_PC/uart_light_funcs.o 

C_DEPS += \
./source/src_PC/compare_funcs.d \
./source/src_PC/pc_compatibility.d \
./source/src_PC/timer_funcs.d \
./source/src_PC/uart_light_funcs.d 


# Each subdirectory must supply rules for building sources it contributes
source/src_PC/%.o: ../source/src_PC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DPC_NATIVE -I"${SPARTANMC_ROOT}/spartanmc/include" -I../testsystem -I../source/include -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

