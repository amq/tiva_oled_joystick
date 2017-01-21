#include "Common.h"
#include "OledTask.h"
#include "OledTaskImages.h"
#include "OledTaskFont.h"

#define SPEED 2
#define SENSITIVITY 12
#define START_X1 80
#define START_X2 95
#define START_Y1 80
#define START_Y2 95

static SPI_Handle spiHandle;
static SPI_Params spiParams;

static void OledTaskFxn(UArg arg0, UArg arg1);
static void SpiWrite(uint16_t data);
static void OledInit(void);
static void OledCommand(uint16_t command, uint16_t data);
static void OledData(uint16_t data);
static void OledDdramAccess(void);
static void OledMemorySize(uint16_t X1, uint16_t X2, uint16_t Y1, uint16_t Y2);
static void OledColor(uint16_t color, uint16_t X1, uint16_t X2, uint16_t Y1, uint16_t Y2);
static void OledClearWhite(void);
static void OledClearBlack(void);

void OledTask_init(Mailbox_Handle mailboxHandle) {
  Task_Handle taskHandle;
  Task_Params taskParams;
  Error_Block eb;

  Error_init(&eb);
  Task_Params_init(&taskParams);
  taskParams.stackSize = 2048;
  taskParams.priority = 10;
  taskParams.arg0 = (UArg)mailboxHandle;
  taskHandle = Task_create((Task_FuncPtr)OledTaskFxn, &taskParams, &eb);

  if (taskHandle == NULL) {
    System_abort("OledTask create failed\n");
  }
}

static void OledTaskFxn(UArg arg0, UArg arg1) {
  int8_t coordinates[2];
  int8_t x1, x2, y1, y2;

  SPI_Params_init(&spiParams);
  spiParams.bitRate = 30*1000*1000;
  spiParams.dataSize = 16;
  spiHandle = SPI_open(SPI_DESC, &spiParams);

  if (spiHandle == NULL) {
    System_abort("SPI open failed\n");
  }

  OledInit();

  OledMemorySize(0, 95, 0, 95);
  OledDdramAccess();
  for (int i = 0; i < 9216; i++) {
    OledData(image_data_hal[i]);
  }

  Task_sleep(2000);

  OledClearBlack();

  /* draw a square */
  x1 = START_X1; x2 = START_X2; y1 = START_Y1; y2 = START_Y2;
  OledColor(0x00FF, x1, x2, y1, y2);

  while (1) {
    if (Mailbox_pend((Mailbox_Handle)arg0, coordinates, BIOS_WAIT_FOREVER)) {

      /* left */
      if (coordinates[0] > SENSITIVITY) {
        OledColor(0x0000, x1, x2, y1, y2);
        x1 = x1 + SPEED;
        x2 = x2 + SPEED;
        if (x1 > 95 || x2 > 95) {
          OledClearBlack();
          x1 = 0;
          x2 = START_X2 - START_X1;
        }
        OledColor(0x00FF, x1, x2, y1, y2);
      }

      /* right */
      if (coordinates[0] < -SENSITIVITY) {
        OledColor(0x0000, x1, x2, y1, y2);
        x1 = x1 - SPEED;
        x2 = x2 - SPEED;
        if (x1 < 0 || x2 < 0) {
          OledClearBlack();
          x1 = START_X1;
          x2 = START_X2;
        }
        OledColor(0x00FF, x1, x2, y1, y2);
      }

      /* down */
      if (coordinates[1] < -SENSITIVITY) {
        OledColor(0x0000, x1, x2, y1, y2);
        y1 = y1 + SPEED;
        y2 = y2 + SPEED;
        if (y1 > 95 || y2 > 95) {
          OledClearBlack();
          y1 = 0;
          y2 = START_X2 - START_X1;
        }
        OledColor(0x00FF, x1, x2, y1, y2);
      }

      /* up */
      if (coordinates[1] > SENSITIVITY) {
        OledColor(0x0000, x1, x2, y1, y2);
        y1 = y1 - SPEED;
        y2 = y2 - SPEED;
        if (y1 < 0 || y2 < 0) {
          OledClearBlack();
          y1 = START_X1;
          y2 = START_X2;
        }
        OledColor(0x00FF, x1, x2, y1, y2);
      }
    }
  }
}

static void SpiWrite(uint16_t data) {
  SPI_Transaction spiTransaction;
  spiTransaction.count = 1;
  spiTransaction.txBuf = &data;
  spiTransaction.rxBuf = NULL;

  (void)SPI_transfer(spiHandle, &spiTransaction);
}

/*
 * @brief SEPS114A standard startup procedure
 */
static void OledInit(void) {
  GPIOPinTypeGPIOOutput(GPIO_OLED_BASE_DC, GPIO_OLED_PIN_DC);
  GPIOPinTypeGPIOOutput(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS);
  GPIOPinTypeGPIOOutput(GPIO_OLED_BASE_RST, GPIO_OLED_PIN_RST);
  GPIOPinTypeGPIOOutput(GPIO_OLED_BASE_RW, GPIO_OLED_PIN_RW);

  GPIOPinWrite(GPIO_OLED_BASE_RW, GPIO_OLED_PIN_RW, 0);
  GPIOPinWrite(GPIO_OLED_BASE_RST, GPIO_OLED_PIN_RST, 0);
  Task_sleep(10);
  GPIOPinWrite(GPIO_OLED_BASE_RST, GPIO_OLED_PIN_RST, GPIO_OLED_PIN_RST);
  Task_sleep(10);

  OledCommand(SEPS114A_SOFT_RESET, 0x00);
  OledCommand(SEPS114A_STANDBY_ON_OFF, 0x01);
  Task_sleep(5);
  OledCommand(SEPS114A_STANDBY_ON_OFF, 0x00);
  Task_sleep(5);
  OledCommand(SEPS114A_DISPLAY_ON_OFF, 0x00);
  OledCommand(SEPS114A_ANALOG_CONTROL, 0x40);
  OledCommand(SEPS114A_OSC_ADJUST, 0x03);
  OledCommand(SEPS114A_DISPLAY_X1, 0x00);
  OledCommand(SEPS114A_DISPLAY_X2, 0x5F);
  OledCommand(SEPS114A_DISPLAY_Y1, 0x00);
  OledCommand(SEPS114A_DISPLAY_Y2, 0x5F);
  OledCommand(SEPS114A_RGB_IF, 0x10);
  OledCommand(SEPS114A_RGB_POL, 0x00);
  OledCommand(SEPS114A_DISPLAY_MODE_CONTROL, 0x80);
  OledCommand(SEPS114A_CPU_IF, 0x00);
  OledCommand(SEPS114A_MEMORY_WRITE_READ, 0x00);
  OledCommand(SEPS114A_ROW_SCAN_DIRECTION, 0x00);
  OledCommand(SEPS114A_ROW_SCAN_MODE, 0x00);
  OledCommand(SEPS114A_COLUMN_CURRENT_R, 0x6E);
  OledCommand(SEPS114A_COLUMN_CURRENT_G, 0x4F);
  OledCommand(SEPS114A_COLUMN_CURRENT_B, 0x77);
  OledCommand(SEPS114A_ROW_OVERLAP, 0x00);
  OledCommand(SEPS114A_DISCHARGE_TIME, 0x01);
  OledCommand(SEPS114A_PEAK_PULSE_DELAY, 0x00);
  OledCommand(SEPS114A_PEAK_PULSE_WIDTH_R, 0x02);
  OledCommand(SEPS114A_PEAK_PULSE_WIDTH_G, 0x02);
  OledCommand(SEPS114A_PEAK_PULSE_WIDTH_B, 0x02);
  OledCommand(SEPS114A_PRECHARGE_CURRENT_R, 0x14);
  OledCommand(SEPS114A_PRECHARGE_CURRENT_G, 0x50);
  OledCommand(SEPS114A_PRECHARGE_CURRENT_B, 0x19);
  OledCommand(SEPS114A_ROW_SCAN_ON_OFF, 0x00);
  OledCommand(SEPS114A_SCAN_OFF_LEVEL, 0x04);
  OledCommand(SEPS114A_DISPLAYSTART_X, 0x00);
  OledCommand(SEPS114A_DISPLAYSTART_Y, 0x00);
  Task_sleep(100);
  OledCommand(SEPS114A_DISPLAY_ON_OFF, 0x01);
}

static void OledCommand(uint16_t command, uint16_t data) {
  /* select index */
  GPIOPinWrite(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS, 0);
  GPIOPinWrite(GPIO_OLED_BASE_DC, GPIO_OLED_PIN_DC, 0);
  SpiWrite(command);
  GPIOPinWrite(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS, GPIO_OLED_PIN_CS);
  /* write data */
  GPIOPinWrite(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS, 0);
  GPIOPinWrite(GPIO_OLED_BASE_DC, GPIO_OLED_PIN_DC, GPIO_OLED_PIN_DC);
  SpiWrite(data);
  GPIOPinWrite(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS, GPIO_OLED_PIN_CS);
}

static void OledData(uint16_t data) {
  GPIOPinWrite(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS, 0);
  GPIOPinWrite(GPIO_OLED_BASE_DC, GPIO_OLED_PIN_DC, GPIO_OLED_PIN_DC);
  SpiWrite(data);
  GPIOPinWrite(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS, GPIO_OLED_PIN_CS);
}

static void OledDdramAccess(void) {
  GPIOPinWrite(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS, 0);
  GPIOPinWrite(GPIO_OLED_BASE_DC, GPIO_OLED_PIN_DC, 0);
  SpiWrite(0x08);
  GPIOPinWrite(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS, GPIO_OLED_PIN_CS);
}

static void OledMemorySize(uint16_t X1, uint16_t X2, uint16_t Y1, uint16_t Y2) {
  OledCommand(SEPS114A_MEM_X1, X1);
  OledCommand(SEPS114A_MEM_X2, X2);
  OledCommand(SEPS114A_MEM_Y1, Y1);
  OledCommand(SEPS114A_MEM_Y2, Y2);
}


static void OledColor(uint16_t color, uint16_t X1, uint16_t X2, uint16_t Y1, uint16_t Y2) {
  static uint16_t buffer[1024];
  int size = (X2 - X1 + 1) * (Y2 - Y1 + 1);
  int slice;

  SPI_Transaction spiTransaction;
  spiTransaction.txBuf = buffer;
  spiTransaction.rxBuf = NULL;

  OledMemorySize(X1, X2, Y1, Y2);
  OledDdramAccess();

  for (int i = 0; i < 1024; i++) {
    buffer[i] = color;
  }

  do {
    slice = size - 1024;

    if (slice >= 1024) {
      spiTransaction.count = 1024;
      size = slice;
    } else {
      spiTransaction.count = size;
    }

    GPIOPinWrite(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS, 0);
    GPIOPinWrite(GPIO_OLED_BASE_DC, GPIO_OLED_PIN_DC, GPIO_OLED_PIN_DC);
    (void)SPI_transfer(spiHandle, &spiTransaction);
    GPIOPinWrite(GPIO_OLED_BASE_CS, GPIO_OLED_PIN_CS, GPIO_OLED_PIN_CS);

  } while (slice > 0);
}

static void OledClearWhite(void) {
  OledColor(0xFFFF, 0, 95, 0, 95);
}

static void OledClearBlack(void) {
  OledColor(0x0000, 0, 95, 0, 95);
}
