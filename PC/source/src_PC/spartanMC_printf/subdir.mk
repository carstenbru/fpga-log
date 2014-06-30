################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/src_PC/spartanMC_printf/printf.c \
../source/src_PC/spartanMC_printf/printf_add_padding.c \
../source/src_PC/spartanMC_printf/printf_format_arg.c \
../source/src_PC/spartanMC_printf/printf_format_bits.c \
../source/src_PC/spartanMC_printf/printf_format_int.c \
../source/src_PC/spartanMC_printf/printf_format_uint.c \
../source/src_PC/spartanMC_printf/putchar.c \
../source/src_PC/spartanMC_printf/puts.c \
../source/src_PC/spartanMC_printf/putsn.c 

OBJS += \
./source/src_PC/spartanMC_printf/printf.o \
./source/src_PC/spartanMC_printf/printf_add_padding.o \
./source/src_PC/spartanMC_printf/printf_format_arg.o \
./source/src_PC/spartanMC_printf/printf_format_bits.o \
./source/src_PC/spartanMC_printf/printf_format_int.o \
./source/src_PC/spartanMC_printf/printf_format_uint.o \
./source/src_PC/spartanMC_printf/putchar.o \
./source/src_PC/spartanMC_printf/puts.o \
./source/src_PC/spartanMC_printf/putsn.o 

C_DEPS += \
./source/src_PC/spartanMC_printf/printf.d \
./source/src_PC/spartanMC_printf/printf_add_padding.d \
./source/src_PC/spartanMC_printf/printf_format_arg.d \
./source/src_PC/spartanMC_printf/printf_format_bits.d \
./source/src_PC/spartanMC_printf/printf_format_int.d \
./source/src_PC/spartanMC_printf/printf_format_uint.d \
./source/src_PC/spartanMC_printf/putchar.d \
./source/src_PC/spartanMC_printf/puts.d \
./source/src_PC/spartanMC_printf/putsn.d 


# Each subdirectory must supply rules for building sources it contributes
source/src_PC/spartanMC_printf/%.o: ../source/src_PC/spartanMC_printf/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DPC_NATIVE -I"${SPARTANMC_ROOT}/spartanmc/include" -I"/home/carsten/Uni/ba/fpga-log/source/include" -I"/home/carsten/Uni/ba/fpga-log/testsystem" -I"/home/carsten/Uni/ba/fpga-log/testsystem/firmware/include" -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


