#include "CANDrive.h"

// STM32F407 Discovery CAN1 Pin Map:
// PD0 -> CAN1_RX
// PD1 -> CAN1_TX
// Alt Function: AF8 (CAN1)

extern CAN_HandleTypeDef
    hcan1; // Defined in main.c by CubeMX usually, or we define it here if not.
// Assuming checking if user uses CubeMX generated handle or my previous one.
// Since user is configuring CubeMX, it will generate 'hcan1' in main.c or
// can.c. To satisfy both "library" approach and "CubeMX" approach, I will
// define it if not extern, but to be safe with CubeMX generated code, I'll rely
// on the user passing it or using global. For now, I will keep defining it here
// static to avoid conflict IF we are Standalone. BUT user showed CubeMX
// screenshots. CubeMX generates 'CanHandle' or 'hcan1' in main.c. Let's stick
// to my previous implementation which was self-contained, but add Interrupt
// support.

// CAN_HandleTypeDef hcan1; // Removed to avoid multiple definition (defined in
// main.c)

// Internal Buffer for Interrupt
static CanReceiver rxIntBuffer;
static volatile uint8_t isDataReceived = 0;

/**
 * @brief  Initializes CAN1 Peripheral.
 *         Configures GPIO, CAN Speed (500kbps), Filters, and Interrupts.
 * @retval None
 */
void CANDrive_Init(void) {
  // 1. Enable Clocks
  __HAL_RCC_CAN1_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  // 2. Configure GPIO (PD0=Rx, PD1=Tx)
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  // 3. Configure CAN Peripheral
  hcan1.Instance = CAN1;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.Prescaler = 4;
  hcan1.Init.TimeSeg1 = CAN_BS1_15TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_5TQ;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;

  if (HAL_CAN_Init(&hcan1) != HAL_OK) {
    while (1)
      ;
  }

  // 4. Configure Filter (Accept All)
  CAN_FilterTypeDef sFilterConfig;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK) {
    while (1)
      ;
  }

  // 5. Start CAN
  if (HAL_CAN_Start(&hcan1) != HAL_OK) {
    while (1)
      ;
  }

  // 6. Enable Interrupts
  // Activate FIFO 0 Message Pending Notification
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) !=
      HAL_OK) {
    while (1)
      ;
  }

  // 7. Configure NVIC
  HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
}

/**
 * @brief  Transmits a message via CAN1.
 *         Supports both Standard and Extended IDs.
 * @param  id: Identifier (Standard or Extended)
 * @param  pData: Pointer to the 8-byte data array
 * @param  len: Data Length Code (DLC), max 8 bytes
 * @param  ide: Identifier Extension (CAN_ID_STD or CAN_ID_EXT)
 * @retval None
 */
void CANDrive_Transmit(uint32_t id, uint8_t *pData, uint8_t len, uint32_t ide) {
  if (len > 8)
    len = 8;
  CAN_TxHeaderTypeDef TxHeader;
  uint32_t TxMailbox;

  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = ide;
  TxHeader.DLC = len;
  TxHeader.TransmitGlobalTime = DISABLE;

  if (ide == CAN_ID_STD) {
    TxHeader.StdId = id;
    TxHeader.ExtId = 0x00;
  } else {
    TxHeader.StdId = 0x00; // Unused in Ext mode usually, but safe to zero
    TxHeader.ExtId = id;
  }

  if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) {
    HAL_CAN_AddTxMessage(&hcan1, &TxHeader, pData, &TxMailbox);
  }
}

/**
 * @brief  Checks and returns data received by Interrupt.
 *         Use this in main loop to poll for new data if needed,
 *         though OnReceive callback is preferred.
 * @param  rxMsg: Pointer to CanReceiver struct to store data
 * @retval 1 if new data exists, 0 if empty
 */
uint8_t CANDrive_Receive(CanReceiver *rxMsg) {
  if (isDataReceived) {
    // Copy processed data from ISR buffer to User buffer
    *rxMsg = rxIntBuffer;

    // Reset flag
    isDataReceived = 0;
    return 1;
  }
  return 0;
}

/**
 * @brief  Weak implementation of callback for CAN Reception.
 *         Called automatically when data is received via Interrupt.
 *         User should override this in main.c to handle data immediately.
 * @param  rxMsg: Pointer to received CanReceiver struct
 * @retval None
 */
__weak void CANDrive_OnReceive(CanReceiver *rxMsg) {
  // unintended: User should override this in main.c
}

/**
 * @brief  HAL CAN Rx FIFO 0 Msg Pending Callback.
 *         Handles the low-level HAL interrupt and calls CANDrive_OnReceive.
 * @param  hcan: Pointer to CAN handle
 * @retval None
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  if (hcan->Instance == CAN1) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t rxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, rxData) == HAL_OK) {
      rxIntBuffer.StdId = RxHeader.StdId;
      rxIntBuffer.ExtId = RxHeader.ExtId;
      rxIntBuffer.IDE = RxHeader.IDE;
      rxIntBuffer.RTR = RxHeader.RTR;
      rxIntBuffer.DLC = RxHeader.DLC;

      for (int i = 0; i < 8; i++) {
        if (i < RxHeader.DLC) {
          rxIntBuffer.data[i] = rxData[i];
        } else {
          rxIntBuffer.data[i] = 0;
        }
      }
      isDataReceived = 1;

      // Trigger user callback immediately
      CANDrive_OnReceive(&rxIntBuffer);
    }
  }
}

/**
 * @brief  This function handles CAN1 RX0 interrupt request.
 *         Forwards to HAL_CAN_IRQHandler.
 * @retval None
 */
void CAN1_RX0_IRQHandler(void) { HAL_CAN_IRQHandler(&hcan1); }
