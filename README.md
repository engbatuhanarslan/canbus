# STM32 CAN Bus

This project implements a modular CAN Bus communication system for STM32 microcontrollers, featuring J1939 support, interrupt-driven reception, and a central signal database.

## System Architecture

The project is structured into three main layers:
1.  **Driver Layer (`CANDrive`)**: Handles low-level hardware interactions and interrupts.
2.  **Protocol Layer (`J1939`, `can.c`)**: Decodes Raw CAN frames into J1939 messages.
3.  **Application Layer (`Database`, `Demo`)**: Manages signal storage and high-level application logic.

---

## 1. Interrupt-Driven CAN Reception

The system uses STM32 hardware interrupts to receive CAN messages efficiently without blocking the main CPU loop.

### How it Works:
1.  **Initialization**: `CANDrive_Init()` configures the CAN hardware and enables the `FIFO0 Message Pending` interrupt.
2.  **ISR (Interrupt Service Routine)**: When a message arrives, the STM32 hardware triggers the ISR.
3.  **Callback**: The ISR calls `CANDrive_OnReceive()`, which reads the message from the mailbox.
4.  **Processing**: The message is passed to `CAN_ProcessMessage()`, which routes it to the specific decoder based on the Identifier type (Standard or Extended).

**Key File:** `src/CANDrive.c`

```c
// Example Interrupt Flow
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    // 1. Read Message
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData);
    
    // 2. Pass to Application
    CanReceiver rxMsg = { ... };
    CAN_ProcessMessage(&rxMsg);
}
```

---

## 2. Central Database System

The **Database** module acts as the single source of truth for all vehicle signals. It decouples the protocol parsing from the application usage.

### Structure (`DBELEMENT_T`)
Each signal in the database is stored as a `DBELEMENT_T` structure containing:
*   `Data`: The value (Union of float, u32, etc.).
*   `IsValid`: Validity flag (1 = Data Received, 0 = Invalid/Stale).
*   `CurrentTime`: Timestamp of the last update.
*   `PreviousTime`: Timestamp of the update before last.

### Timeout Mechanism
To prevent the application from using stale data (e.g., if a sensor stops sending), the system includes a timeout check.

*   **Setting Data**: `DataBase_Set(ID, Value)` updates the value and timestamp.
*   **Checking Timeout**: `DataBase_CanMessageTimeoutCheck(ID, Timeout_ms)` checks if the signal has not been updated for the specified time. If it has timed out, `IsValid` is set to `false`.

**Usage Example:**
```c
// In Application Loop
void Demo_Update(void) {
    // 1. Check for timeout (e.g., 1000ms)
    if (DataBase_CanMessageTimeoutCheck(db_FEE5_TestSignal, 1000)) {
        // Signal lost! Handle error
    }

    // 2. Get Data
    DataBase_Get(&mySignal, db_FEE5_TestSignal, 0);
    
    if (mySignal.IsValid) {
        // Safe to use mySignal.Data.flt
    }
}
```

---

## 3. J1939 Integration

The project includes parsers for standard J1939 PGNs.

*   **CCVS1 (0xFEF1)**: Vehicle Speed, Parking Brake, Cruise Control.
*   **EBC1 (0xF001)**: ABS and ASR status.
*   **FEE5**: Example OEM specific message.

New messages can be added by defining a handler in `src/can.c` and a parsing function in `J1939/`.

### Example Flow:
1.  **CAN Frame Received**: ID `0x18FEF100` (Extended).
2.  **Dispatcher**: `CANDecodeRxExt` identifies PGN `0xFEF1`.
3.  **Parser**: Calls `J1939_PGN_FEF1_CCVS1()`.
4.  **Database Update**: Parser extracts "Wheel Based Speed" and calls `DataBase_Set(db_CCVS1_WheelBasedVehicleSpeed, speed)`.
