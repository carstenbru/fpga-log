################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/src_PC/compare_funcs.c \
../source/src_PC/fatFS_linux_blk.c \
../source/src_PC/i2c_funcs.c \
../source/src_PC/long_int.c \
../source/src_PC/pc_compatibility.c \
../source/src_PC/pwm.c \
../source/src_PC/spi_funcs.c \
../source/src_PC/timer_funcs.c \
../source/src_PC/uart_light_funcs.c 

OBJS += \
./source/src_PC/compare_funcs.o \
./source/src_PC/fatFS_linux_blk.o \
./source/src_PC/i2c_funcs.o \
./source/src_PC/long_int.o \
./source/src_PC/pc_compatibility.o \
./source/src_PC/pwm.o \
./source/src_PC/spi_funcs.o \
./source/src_PC/timer_funcs.o \
./source/src_PC/uart_light_funcs.o 

C_DEPS += \
./source/src_PC/compare_funcs.d \
./source/src_PC/fatFS_linux_blk.d \
./source/src_PC/i2c_funcs.d \
./source/src_PC/long_int.d \
./source/src_PC/pc_compatibility.d \
./source/src_PC/pwm.d \
./source/src_PC/spi_funcs.d \
./source/src_PC/timer_funcs.d \
./source/src_PC/uart_light_funcs.d 


# Each subdirectory must supply rules for building sources it contributes
source/src_PC/%.o: ../source/src_PC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DPC_NATIVE -I"${SPARTANMC_ROOT}/spartanmc/include" -I"/home/carsten/Uni/ba/fpga-log/source/include" -I"/home/carsten/Uni/ba/fpga-log/testsystem" -I"/home/carsten/Uni/ba/fpga-log/testsystem/firmware/include" -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


