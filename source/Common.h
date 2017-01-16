#pragma once

#include <stdbool.h>
#include <stdint.h>

/* Driverlib Header files */
#include <driverlib/gpio.h> /* GPIO_PIN_x */
#include <inc/hw_memmap.h> /* GPIO_PORTx_BASE */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/SPI.h>

/* Board Header file */
#include "local_inc/Board.h"

#define v(fmt, ...) \
  if (verbose) \
    System_printf("%s(): " fmt, __func__, __VA_ARGS__); System_flush();

extern int verbose;
