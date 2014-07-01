################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../testsystem/firmware/src/logger_config.c \
../testsystem/firmware/src/main.c 

OBJS += \
./testsystem/firmware/src/logger_config.o \
./testsystem/firmware/src/main.o 

C_DEPS += \
./testsystem/firmware/src/logger_config.d \
./testsystem/firmware/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
testsystem/firmware/src/%.o: ../testsystem/firmware/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DPC_NATIVE -I"${SPARTANMC_ROOT}/spartanmc/include" -I"/home/carsten/Uni/ba/fpga-log/source/include" -I"/home/carsten/Uni/ba/fpga-log/testsystem" -I"/home/carsten/Uni/ba/fpga-log/testsystem/firmware/include" -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


