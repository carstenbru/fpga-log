################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/fatFS_sdcard.c \
../source/sink_sd_card.c 

OBJS += \
./source/fatFS_sdcard.o \
./source/sink_sd_card.o 

C_DEPS += \
./source/fatFS_sdcard.d \
./source/sink_sd_card.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DPC_NATIVE -DPERI_CLOCK=16000000 -I"${SPARTANMC_ROOT}/spartanmc/include" -I../testsystem -I../source/include -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


