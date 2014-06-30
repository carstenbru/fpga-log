################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/src/FatFs.c \
../source/src/control_action_simple.c \
../source/src/control_port.c \
../source/src/control_protocol.c \
../source/src/control_protocol_ascii.c \
../source/src/data_port.c \
../source/src/datastream_condition.c \
../source/src/datastream_condition_compare_float.c \
../source/src/datastream_condition_compare_int.c \
../source/src/datastream_condition_compare_long.c \
../source/src/datastream_condition_compare_val_name.c \
../source/src/datastream_object.c \
../source/src/datastream_source.c \
../source/src/device_hct99.c \
../source/src/device_pwm.c \
../source/src/device_uart_raw.c \
../source/src/dm_filter.c \
../source/src/dm_splitter_control.c \
../source/src/dm_splitter_data.c \
../source/src/dm_timer.c \
../source/src/dm_trigger.c \
../source/src/formatter.c \
../source/src/formatter_simple.c \
../source/src/lshrsi3.c \
../source/src/print_long.c \
../source/src/sink_sd_card.c \
../source/src/sink_uart.c \
../source/src/udivsi3.c 

OBJS += \
./source/src/FatFs.o \
./source/src/control_action_simple.o \
./source/src/control_port.o \
./source/src/control_protocol.o \
./source/src/control_protocol_ascii.o \
./source/src/data_port.o \
./source/src/datastream_condition.o \
./source/src/datastream_condition_compare_float.o \
./source/src/datastream_condition_compare_int.o \
./source/src/datastream_condition_compare_long.o \
./source/src/datastream_condition_compare_val_name.o \
./source/src/datastream_object.o \
./source/src/datastream_source.o \
./source/src/device_hct99.o \
./source/src/device_pwm.o \
./source/src/device_uart_raw.o \
./source/src/dm_filter.o \
./source/src/dm_splitter_control.o \
./source/src/dm_splitter_data.o \
./source/src/dm_timer.o \
./source/src/dm_trigger.o \
./source/src/formatter.o \
./source/src/formatter_simple.o \
./source/src/lshrsi3.o \
./source/src/print_long.o \
./source/src/sink_sd_card.o \
./source/src/sink_uart.o \
./source/src/udivsi3.o 

C_DEPS += \
./source/src/FatFs.d \
./source/src/control_action_simple.d \
./source/src/control_port.d \
./source/src/control_protocol.d \
./source/src/control_protocol_ascii.d \
./source/src/data_port.d \
./source/src/datastream_condition.d \
./source/src/datastream_condition_compare_float.d \
./source/src/datastream_condition_compare_int.d \
./source/src/datastream_condition_compare_long.d \
./source/src/datastream_condition_compare_val_name.d \
./source/src/datastream_object.d \
./source/src/datastream_source.d \
./source/src/device_hct99.d \
./source/src/device_pwm.d \
./source/src/device_uart_raw.d \
./source/src/dm_filter.d \
./source/src/dm_splitter_control.d \
./source/src/dm_splitter_data.d \
./source/src/dm_timer.d \
./source/src/dm_trigger.d \
./source/src/formatter.d \
./source/src/formatter_simple.d \
./source/src/lshrsi3.d \
./source/src/print_long.d \
./source/src/sink_sd_card.d \
./source/src/sink_uart.d \
./source/src/udivsi3.d 


# Each subdirectory must supply rules for building sources it contributes
source/src/%.o: ../source/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DPC_NATIVE -I"${SPARTANMC_ROOT}/spartanmc/include" -I"/home/carsten/Uni/ba/fpga-log/source/include" -I"/home/carsten/Uni/ba/fpga-log/testsystem" -I"/home/carsten/Uni/ba/fpga-log/testsystem/firmware/include" -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


