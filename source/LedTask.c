#include "Common.h"
#include "LedTask.h"

static void LedTaskFxn(UArg arg0, UArg arg1);

void LedTask_init() {
  Task_Handle LedTaskHandle;
  Task_Params LedTaskParams;
  Error_Block eb;

  Error_init(&eb);
  Task_Params_init(&LedTaskParams);
  LedTaskParams.stackSize = 512;
  LedTaskParams.priority = 10;
  LedTaskParams.arg0 = 1000;
  LedTaskHandle = Task_create((Task_FuncPtr)LedTaskFxn, &LedTaskParams, &eb);

  if (LedTaskHandle == NULL) {
    System_abort("LedTask create failed");
  }
}

static void LedTaskFxn(UArg arg0, UArg arg1) {
  while (1) {
    Task_sleep((unsigned int)arg0);
    GPIO_toggle(Board_LED0);
  }
}
