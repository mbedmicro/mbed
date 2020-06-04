# SPDX-License-Identifier: Apache-2.0

# TODO: @mbed-os-tools All these should be autogenerated, so this can be removed later

# Placeholder, use the same command to overwrite provide target linker script file
set_property(GLOBAL PROPERTY MBED_OS_TARGET_LINKER_FILE "")

# TODO: @mbed-os-tools This needs to come from tools
# Build system provides "target" list.
# Note: This must be placed before subdirectories
# K64F
set_property(GLOBAL
  PROPERTY MBED_OS_TARGET_LABELS
    Freescale;
    MCUXpresso_MCUS;
    KSDK2_MCUS;
    FRDM;
    KPSDK_MCUS;
    KPSDK_CODE;
    MCU_K64F;
    Freescale_EMAC;
    PSA;
    CORTEX_M;
    RTOS_M4_M7;
    M4;
    LIKE_CORTEX_M4;
    CORTEX;
    K64F
)

# TODO: @mbed-os-tools Generate definitions
# K64F
add_definitions(
  -DARM_MATH_CM4
  -DCOMPONENT_FLASHIAP=1
  -DCOMPONENT_NSPE=1
  -DCOMPONENT_PSA_SRV_EMUL=1
  -DCOMPONENT_PSA_SRV_IMPL=1
  -DCOMPONENT_SD=1
  -DCPU_MK64FN1M0VMD12
  -DDEVICE_ANALOGIN=1
  -DDEVICE_ANALOGOUT=1
  -DDEVICE_CRC=1
  -DDEVICE_EMAC=1
  -DDEVICE_FLASH=1
  -DDEVICE_I2C=1
  -DDEVICE_I2CSLAVE=1
  -DDEVICE_INTERRUPTIN=1
  -DDEVICE_LPTICKER=1
  -DDEVICE_PORTIN=1
  -DDEVICE_PORTINOUT=1
  -DDEVICE_PORTOUT=1
  -DDEVICE_PWMOUT=1
  -DDEVICE_RESET_REASON=1
  -DDEVICE_RTC=1
  -DDEVICE_SERIAL=1
  -DDEVICE_SERIAL_ASYNCH=1
  -DDEVICE_SERIAL_FC=1
  -DDEVICE_SLEEP=1
  -DDEVICE_SPI=1
  -DDEVICE_SPISLAVE=1
  -DDEVICE_SPI_ASYNCH=1
  -DDEVICE_STDIO_MESSAGES=1
  -DDEVICE_TRNG=1
  -DDEVICE_USBDEVICE=1
  -DDEVICE_USTICKER=1
  -DDEVICE_WATCHDOG=1
  -DFSL_RTOS_MBED
  -DMBED_BUILD_TIMESTAMP=1582132175.9639401
  -DMBED_SPLIT_HEAP
  -DMBED_TICKLESS
  -DTARGET_CORTEX
  -DTARGET_CORTEX_M
  -DTARGET_FF_ARDUINO
  -DTARGET_FRDM
  -DTARGET_Freescale
  -DTARGET_Freescale_EMAC
  -DTARGET_K64F
  -DTARGET_KPSDK_CODE
  -DTARGET_KPSDK_MCUS
  -DTARGET_KSDK2_MCUS
  -DTARGET_LIKE_CORTEX_M4
  -DTARGET_LIKE_MBED
  -DTARGET_M4
  -DTARGET_MCUXpresso_MCUS
  -DTARGET_MCU_K64F
  -DTARGET_NAME=K64F
  -DTARGET_PSA
  -DTARGET_RELEASE
  -DTARGET_RTOS_M4_M7
  -DTOOLCHAIN_GCC
  -DTOOLCHAIN_GCC_ARM
  -DMBEDTLS_MD5_C
  -D__CMSIS_RTOS
  -D__CORTEX_M4
  -D__FPU_PRESENT=1
  -D__MBED_CMSIS_RTOS_CM
  -D__MBED__=1
)

# DISCO_L475VG_IOT01A
# set_property(GLOBAL
#   PROPERTY MBED_OS_TARGET_LABELS
#     STM;
#     STM32L4;
#     STM32L475xG;
#     STM32L475VG;
#     MX25R6435F;
#     DISCO_L475VG_IOT01A;
#     CORTEX_M;
#     M4;
#     LIKE_CORTEX_M4;
#     CORTEX;
#     RTOS_M4_M7;
# )

# # TODO: @mbed-os-tools Generate definitions
# # DISCO_L475VG_IOT01A
# add_definitions(
#   -DARM_MATH_CM4
#   -DCOMPONENT_FLASHIAP=1
#   -DCOMPONENT_NSPE=1
#   -DCOMPONENT_PSA_SRV_EMUL=1
#   -DCOMPONENT_PSA_SRV_IMPL=1
#   -DCOMPONENT_QSPIF=1
#   -DDEVICE_ANALOGIN=1
#   -DDEVICE_ANALOGOUT=1
#   -DDEVICE_CAN=1
#   -DDEVICE_CRC=1
#   -DDEVICE_FLASH=1
#   -DDEVICE_I2C=1
#   -DDEVICE_I2CSLAVE=1
#   -DDEVICE_I2C_ASYNCH=1
#   -DDEVICE_INTERRUPTIN=1
#   -DDEVICE_LPTICKER=1
#   -DDEVICE_MPU=1
#   -DDEVICE_PORTIN=1
#   -DDEVICE_PORTINOUT=1
#   -DDEVICE_PORTOUT=1
#   -DDEVICE_PWMOUT=1
#   -DDEVICE_QSPI=1
#   -DDEVICE_RESET_REASON=1
#   -DDEVICE_RTC=1
#   -DDEVICE_SERIAL=1
#   -DDEVICE_SERIAL_FC=1
#   -DDEVICE_SLEEP=1
#   -DDEVICE_SPI=1
#   -DDEVICE_SPISLAVE=1
#   -DDEVICE_SPI_ASYNCH=1
#   -DDEVICE_STDIO_MESSAGES=1
#   -DDEVICE_TRNG=1
#   -DDEVICE_USBDEVICE=1
#   -DDEVICE_USTICKER=1
#   -DDEVICE_WATCHDOG=1
#   -DEXTRA_IDLE_STACK_REQUIRED
#   -DMBED_BUILD_TIMESTAMP=1586337497.5047941
#   -DMBED_SPLIT_HEAP
#   -DMBED_TICKLESS
#   -DSTM32L475xx
#   -DTARGET_CORTEX
#   -DTARGET_CORTEX_M
#   -DTARGET_DISCO_L475VG_IOT01A
#   -DTARGET_FAMILY_STM32
#   -DTARGET_FF_ARDUINO
#   -DTARGET_LIKE_CORTEX_M4
#   -DTARGET_LIKE_MBED
#   -DTARGET_M4
#   -DTARGET_MX25R6435F
#   -DTARGET_NAME=DISCO_L475VG_IOT01A
#   -DTARGET_RELEASE
#   -DTARGET_RTOS_M4_M7
#   -DTARGET_STM
#   -DTARGET_STM32L4
#   -DTARGET_STM32L475VG
#   -DTARGET_STM32L475xG
#   -DTOOLCHAIN_GCC
#   -DTOOLCHAIN_GCC_ARM
#   -DTRANSACTION_QUEUE_SIZE_SPI=2
#   -DUSE_FULL_LL_DRIVER
#   -DUSE_HAL_DRIVER
#   -D__CMSIS_RTOS
#   -D__CORTEX_M4
#   -D__FPU_PRESENT=1
#   -D__MBED_CMSIS_RTOS_CM
#   -D__MBED__=1
# )
