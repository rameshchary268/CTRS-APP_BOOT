################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../srrc/BOOT.c \
../srrc/Crc.c \
../srrc/EEPROM.c \
../srrc/FileX.c \
../srrc/mymain.c 

OBJS += \
./srrc/BOOT.o \
./srrc/Crc.o \
./srrc/EEPROM.o \
./srrc/FileX.o \
./srrc/mymain.o 

C_DEPS += \
./srrc/BOOT.d \
./srrc/Crc.d \
./srrc/EEPROM.d \
./srrc/FileX.d \
./srrc/mymain.d 


# Each subdirectory must supply rules for building sources it contributes
srrc/%.o srrc/%.su srrc/%.cyclo: ../srrc/%.c srrc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DFX_INCLUDE_USER_DEFINE_FILE -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -DUSE_HAL_DRIVER -DSTM32U5G7xx -DUX_INCLUDE_USER_DEFINE_FILE -c -I../FileX/App -I../FileX/Target -I../Core/Inc -I../AZURE_RTOS/App -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/threadx/common/inc -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Middlewares/ST/filex/common/inc -I../Middlewares/ST/filex/ports/generic/inc -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -I../Drivers/CMSIS/Include -I../USBX/App -I../USBX/Target -I../Middlewares/ST/usbx/common/core/inc -I../Middlewares/ST/usbx/ports/generic/inc -I../Middlewares/ST/usbx/common/usbx_stm32_device_controllers -I../Middlewares/ST/usbx/common/usbx_device_classes/inc -I"C:/Users/RameshcharyVadla/Downloads/Boot26-11-2025 (1)/Boot24-11-2025/srrc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-srrc

clean-srrc:
	-$(RM) ./srrc/BOOT.cyclo ./srrc/BOOT.d ./srrc/BOOT.o ./srrc/BOOT.su ./srrc/Crc.cyclo ./srrc/Crc.d ./srrc/Crc.o ./srrc/Crc.su ./srrc/EEPROM.cyclo ./srrc/EEPROM.d ./srrc/EEPROM.o ./srrc/EEPROM.su ./srrc/FileX.cyclo ./srrc/FileX.d ./srrc/FileX.o ./srrc/FileX.su ./srrc/mymain.cyclo ./srrc/mymain.d ./srrc/mymain.o ./srrc/mymain.su

.PHONY: clean-srrc

