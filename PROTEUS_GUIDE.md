# HƯỚNG DẪN MÔ PHỎNG PROTEUS 8 VÀ CẤU HÌNH STM32CUBEIDE
## Môn học: CO3053 - Hệ thống nhúng (Embedded Systems) - HCMUT
## Đề bài: Assignment 2 - Điều khiển máy giặt (Washing Machine Control Unit)

---

## 1. Danh sách linh kiện trong Proteus 8 (Pick Devices)

| Tên linh kiện trong Proteus | Mục đích | Số lượng | Ghi chú |
| :--- | :--- | :--- | :--- |
| **STM32F103C8** | Vi điều khiển chính ARM Cortex-M3 | 1 | Chip Blue Pill |
| **BUTTON** | Nút bấm điều khiển & nạp xu | 7 | Nút bấm thường hở |
| **LED-RED** | Đèn Red LED (RLED) | 1 | Báo Standby / Error |
| **LED-BLUE** | Đèn Blue LED (BLED) | 1 | Báo Ready / Running |
| **LED-YELLOW** hoặc **MOTOR-DC** | Động cơ giặt (Washing Motor) | 1 | Hoặc dùng LED Vàng tượng trưng |
| **BUZZER** | Còi báo kết thúc giặt / báo lỗi | 1 | Tùy chọn |
| **RES** | Điện trở hạn dòng cho LED (220Ω / 330Ω) | 3 | Bảo vệ LED |
| **VIRTUAL TERMINAL** | Màn hình Serial UART hiển thị Log FSM | 1 | Rất hữu ích để chấm điểm |

---

## 2. Sơ đồ kết nối chân (Wiring Pinout Table)

### 2.1. Khối Nút bấm (Inputs - Cổng GPIOA)
*Lưu ý: Firmware đã cấu hình `GPIO_PULLUP` nội bên trong chip STM32, nên một đầu nút bấm nối vào chân STM32, đầu còn lại chỉ cần nối xuống **GND**.*

| Chân STM32 | Tên nút | Chức năng trong bài | Kết nối trên Proteus |
| :--- | :--- | :--- | :--- |
| **PA0** | `BTN_STOP` | Dừng chu trình / Nhấn 2 lần để Force Stop | Nối 1 chân nút bấm, chân kia nối GND |
| **PA1** | `BTN_RUN` | Bắt đầu giặt (khi đủ tiền) / Tiếp tục giặt (khi Pause) | Nối 1 chân nút bấm, chân kia nối GND |
| **PA2** | `BTN_PAUSE`| Tạm dừng giặt (Timer vẫn đếm ngược) | Nối 1 chân nút bấm, chân kia nối GND |
| **PA3** | `BTN_COIN_10`| Mô phỏng nạp xu **10-cents** | Nối 1 chân nút bấm, chân kia nối GND |
| **PA4** | `BTN_COIN_20`| Mô phỏng nạp xu **20-cents** | Nối 1 chân nút bấm, chân kia nối GND |
| **PA5** | `BTN_COIN_50`| Mô phỏng nạp xu **50-cents** | Nối 1 chân nút bấm, chân kia nối GND |
| **PA6** | `BTN_ERROR` | Nút kích hoạt / xóa lỗi máy giặt | Nối 1 chân nút bấm, chân kia nối GND |

### 2.2. Khối Cơ cấu chấp hành & Hiển thị (Outputs - Cổng GPIOB)

| Chân STM32 | Tên linh kiện | Màu sắc / Tác vụ | Hành vi theo mô hình Moore |
| :--- | :--- | :--- | :--- |
| **PB0** | `RLED` (Red LED) | Đỏ | **Sáng đứng** khi `STANDBY`<br>**Nhấp nháy 500ms** khi `ERROR`<br>**Tắt** trong các trạng thái khác |
| **PB1** | `BLED` (Blue LED)| Xanh dương | **Sáng đứng** khi `READY` (đã đủ >= 50c)<br>**Nhấp nháy 500ms** khi `RUNNING` (đang giặt)<br>**Tắt** khi `STANDBY` / `PAUSED` |
| **PB8** | `MOTOR` (Relay/LED) | Vàng / Motor | **BẬT (HIGH)** khi `RUNNING`<br>**TẮT (LOW)** khi `PAUSED`, `STANDBY`, `READY`, `ERROR` |
| **PB9** | `BUZZER` | Còi | Kêu 1 tiếng bíp khi giặt xong hoặc khi Force Stop |

*Mạch LED: Chân PBx -> Điện trở 330Ω -> Anode (+) của LED -> Cathode (-) của LED nối xuống GND.*

### 2.3. Khối Giám sát UART (Virtual Terminal)
Để xem FSM nhảy trạng thái, số tiền và thời gian đếm lùi trực quan trên Proteus:
* Chân **PA9 (USART1_TX)** nối vào chân **RXD** của Virtual Terminal.
* Chân **PA10 (USART1_RX)** nối vào chân **TXD** của Virtual Terminal (tùy chọn).
* Thiết lập trong Virtual Terminal: **Baud Rate = 9600**, Data Bits = 8, Parity = None, Stop Bits = 1.

---

## 3. Cấu hình và Tạo file `.hex` trong STM32CubeIDE

1. **Tạo Project mới:**
   * Mở STM32CubeIDE -> `File` -> `New` -> `STM32 Project`.
   * Nhập Part Number: `STM32F103C8` -> Chọn `STM32F103C8Tx` -> Bấm `Next`.
   * Đặt tên Project: `WashingMachine_FSM` -> Chọn Target Language: `C` -> Bấm `Finish`.
2. **Copy mã nguồn vào Project:**
   * Copy toàn bộ các file trong thư mục `Core/Inc/` vào `Core/Inc/` của project:
     - [main.h](file:///c:/Users/ADMIN/Desktop/261/HTN/Core/Inc/main.h)
     - [fsm.h](file:///c:/Users/ADMIN/Desktop/261/HTN/Core/Inc/fsm.h)
     - [button.h](file:///c:/Users/ADMIN/Desktop/261/HTN/Core/Inc/button.h)
     - [led.h](file:///c:/Users/ADMIN/Desktop/261/HTN/Core/Inc/led.h)
   * Copy toàn bộ các file trong thư mục `Core/Src/` vào `Core/Src/` của project:
     - [main.c](file:///c:/Users/ADMIN/Desktop/261/HTN/Core/Src/main.c)
     - [fsm.c](file:///c:/Users/ADMIN/Desktop/261/HTN/Core/Src/fsm.c)
     - [button.c](file:///c:/Users/ADMIN/Desktop/261/HTN/Core/Src/button.c)
     - [led.c](file:///c:/Users/ADMIN/Desktop/261/HTN/Core/Src/led.c)
3. **Bật chế độ xuất file `.hex` trong STM32CubeIDE:**
   * Chuột phải vào tên Project trong thẻ *Project Explorer* -> Chọn **Properties**.
   * Vào `C/C++ Build` -> **Settings**.
   * Chọn tab `Tool Settings` -> vào mục `MCU/MPU Post build outputs`.
   * Tích chọn ô: **Convert to Intel Hex file (-O ihex)**.
   * Bấm **Apply and Close**.
4. **Build Project:**
   * Nhấn biểu tượng cây búa (Build) hoặc phím tắt `Ctrl + B`.
   * File `.hex` sẽ được sinh ra tại: `WashingMachine_FSM/Debug/WashingMachine_FSM.hex`.

---

## 4. Nạp file `.hex` và Chạy mô phỏng trên Proteus

1. Nhấp đúp chuột trái vào con chip **STM32F103C8** trên bản vẽ Proteus.
2. Trong cửa sổ thuộc tính (Edit Component):
   * Mục **Program File**: Bấm vào biểu tượng thư mục màu vàng, trỏ tới file `WashingMachine_FSM.hex`.
   * Mục **Crystal Frequency**: Nhập `8MHz` (hoặc `8000000`).
3. Bấm **OK**.
4. Bấm nút **Play (Run Simulation)** ở góc dưới bên trái Proteus.

---

## 5. Kịch bản kiểm thử (Test Scenarios) theo Lecture 4

### Test 1: Khởi động và Nạp tiền (Standby -> Ready)
* Khi vừa cấp điện: Màn hình Virtual Terminal hiển thị chào mừng. Đèn **RLED sáng đứng** (Standby), **BLED tắt**, Motor tắt.
* Nhấn nút `BTN_COIN_10` (PA3): Tiền = 10c, máy vẫn ở STANDBY.
* Nhấn nút `BTN_COIN_20` (PA4): Tiền = 30c, máy vẫn ở STANDBY.
* Nhấn nút `BTN_COIN_20` (PA4): Tiền = 50c -> FSM chuyển ngay sang **READY**.
* Quan sát: Đèn **RLED tắt**, đèn **BLED sáng đứng** (Ready to execute).
* Thử nhấn thêm `BTN_COIN_50` (PA5): Tiền = 100c, máy vẫn ở READY (nhận tiền dư nhưng không thối lại theo đề bài).

### Test 2: Bắt đầu chu trình giặt (Ready -> Running)
* Khi BLED đang sáng đứng (Ready), nhấn nút `BTN_RUN` (PA1).
* Quan sát:
  - Tiền bị xóa sạch về 0 (`money = 0`).
  - Timer 30 phút bắt đầu đếm lùi (mặc định cấu hình mô phỏng nhanh 30 giây để test trực quan).
  - Đèn **BLED chuyển sang nhấp nháy liên tục 500ms** (Running).
  - Đèn Vàng / Motor **sáng/quay (HIGH)**.

### Test 3: Tạm dừng giặt (Running -> Paused -> Running)
* Trong lúc máy đang giặt (BLED nhấp nháy, Motor chạy), nhấn nút `BTN_PAUSE` (PA2).
* Quan sát:
  - Máy chuyển sang trạng thái **PAUSED**.
  - Động cơ giặt tắt (`Motor = OFF`).
  - **Điểm mấu chốt của đề bài**: Màn hình Terminal vẫn hiển thị Timer **đang tiếp tục đếm lùi**!
* Nhấn nút `BTN_RUN` (PA1): Máy tiếp tục giặt, Motor chạy lại, BLED lại nhấp nháy.

### Test 4: Dừng cưỡng bức bằng cách nhấn STOP 2 lần (Force to Stop)
* Khi máy đang giặt (hoặc đang pause), nhấn nút `BTN_STOP` (PA0) lần 1.
* Máy cảnh báo: *"Press STOP again within 2000 ms to Force Stop"*.
* Nhấn nút `BTN_STOP` lần 2 ngay sau đó:
  - Máy lập tức buộc dừng (**Force Stop**).
  - Chu trình giặt bị hủy, timer về 0, Motor tắt.
  - Máy trở về trạng thái ban đầu **STANDBY (RLED sáng đứng, BLED tắt)**.
* *(Nếu bấm STOP lần 1 mà sau 2 giây không bấm lần 2, máy sẽ tự động quay lại chạy tiếp mà không bị dừng).*

### Test 5: Tự động kết thúc sau 30 phút (Cycle Termination)
* Để máy chạy hết chu trình (30s/30 phút).
* Khi Timer về `00:00`: Máy tự động dừng giặt, còi Buzzer phát âm thanh báo hoàn thành, máy tự động quay về trạng thái **STANDBY**.

### Test 6: Báo lỗi sự cố (Error State)
* Bấm nút `BTN_ERR` (PA6): Máy nhảy sang trạng thái **ERROR**.
* Quan sát: Đèn **RLED nhấp nháy liên tục** (báo lỗi theo đúng đề bài: *"RLED is blinking if the machine has errors"*). Động cơ bị ngắt.
* Bấm nút `BTN_STOP` hoặc `BTN_ERR` một lần nữa để reset lỗi: Máy quay lại **STANDBY (RLED sáng đứng)**.
