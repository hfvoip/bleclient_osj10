################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/app_bass.c \
../code/app_init.c \
../code/app_process.c \
../code/ble_basc.c \
../code/ble_custom.c \
../code/ble_std.c \
../code/uart.c \
../code/ui_handler.c 

OBJS += \
./code/app_bass.o \
./code/app_init.o \
./code/app_process.o \
./code/ble_basc.o \
./code/ble_custom.o \
./code/ble_std.o \
./code/uart.o \
./code/ui_handler.o 

C_DEPS += \
./code/app_bass.d \
./code/app_init.d \
./code/app_process.d \
./code/ble_basc.d \
./code/ble_custom.d \
./code/ble_std.d \
./code/uart.d \
./code/ui_handler.d 


# Each subdirectory must supply rules for building sources it contributes
code/%.o: ../code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DRSL10_CID=101 -DOUTPUT_INTERFACE=1 -DCFG_BLE=1 -DCFG_ALLROLES=1 -DCFG_APP -DCFG_APP_BATT -DCFG_ATTC=1 -DCFG_CON=8 -DCFG_EMB=1 -DCFG_HOST=1 -DCFG_RF_ATLAS=1 -DCFG_ALLPRF=1 -DCFG_PRF=1 -DCFG_NB_PRF=2 -DCFG_CHNL_ASSESS=1 -DCFG_SEC_CON=1 -DCFG_EXT_DB -DCFG_PRF_BASC=1 -D_RTE_ -I"C:\work2024\bleclient_osj10\include" -I"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/include" -I"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/include/bb" -I"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/include/ble" -I"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/include/ble/profiles" -I"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/include/kernel" -I"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/source/firmware/printf" -I"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/source/firmware/rtt" -I"C:\work2024\bleclient_osj10/RTE" -I"C:\work2024\bleclient_osj10/RTE/Device/RSL10" -I"C:\work2024\bleclient_osj10/RTE/Utility" -isystem"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/include" -isystem"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/include/bb" -isystem"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/include/ble" -isystem"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/include/ble/profiles" -isystem"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/include/kernel" -isystem"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/source/firmware/printf" -isystem"C:/Users/xeeen/ON_Semiconductor/PACK/ONSemiconductor/RSL10/3.7.606/source/firmware/rtt" -isystem"C:\work2024\bleclient_osj10/RTE" -isystem"C:\work2024\bleclient_osj10/RTE/Device/RSL10" -isystem"C:\work2024\bleclient_osj10/RTE/Utility" -std=gnu11 -Wmissing-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


