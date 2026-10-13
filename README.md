# Bài 4: Điều khiển 3 LED với FreeRTOS trên STM32F10x

## 1. Giới thiệu

Bài tập thực hiện điều khiển 3 LED nhấp nháy với các tần số khác nhau trên vi điều khiển STM32F10x, sử dụng hệ điều hành thời gian thực **FreeRTOS** và thư viện **STM32 Standard Peripheral Library (STD)**.

Mỗi LED được điều khiển bởi một task độc lập, giúp thực hành lập trình đa nhiệm và quản lý thời gian thực trong hệ thống nhúng.

**Mục tiêu:**
- Làm quen với hệ điều hành FreeRTOS.
- Tạo và quản lý nhiều task bằng `xTaskCreate()`.
- Điều khiển GPIO bằng thư viện STM32 STD.
- Sử dụng `vTaskDelay()` để tạo thời gian trễ.
- Điều khiển 3 LED nhấp nháy với tần số độc lập.

## 2. Phần cứng và phần mềm

**Phần cứng:**
- Vi điều khiển thuộc dòng STM32F10x.
- 3 LED.
- 3 điện trở hạn dòng (ví dụ 220Ω–330Ω).
- Bộ nạp/debug ST-Link (nếu sử dụng mạch thật).

**Phần mềm và thư viện:**
- Môi trường phát triển hỗ trợ STM32F10x.
- STM32 Standard Peripheral Library.
- FreeRTOS.
- Trình biên dịch ARM phù hợp với vi điều khiển.

## 3. Cấu hình LED

| LED | Chân GPIO | Tần số | Chu kỳ | Thời gian đảo trạng thái |
|---|---|---|---|---|
| LED 1 | PA0 | 0.1 Hz | 10 giây | 5000 ms |
| LED 2 | PA1 | 1 Hz | 1 giây | 500 ms |
| LED 3 | PA2 | 10 Hz | 0.1 giây | 50 ms |

Các chân PA0, PA1, PA2 được cấu hình ở chế độ Output Push-Pull với tốc độ GPIO 2 MHz.

Với cách nối LED từ chân GPIO qua điện trở xuống GND:
- GPIO mức HIGH: LED sáng.
- GPIO mức LOW: LED tắt.

## 4. Nguyên lý hoạt động

### 4.1. Khởi tạo GPIO

Hàm `GPIO_Config()` thực hiện:
- Bật xung clock cho GPIOA.
- Cấu hình PA0, PA1, PA2 là chân đầu ra.
- Đặt mức LOW ban đầu cho cả 3 LED.

### 4.2. Tạo các task FreeRTOS

Chương trình sử dụng hàm `xTaskCreate()` để tạo 3 task điều khiển LED.

Các task đều sử dụng chung hàm `LED_BlinkTask()`, nhưng nhận tham số khác nhau thông qua cấu trúc `LED_Config_t`.

Cấu trúc này chứa:
- `port`: Cổng GPIO.
- `pin`: Chân điều khiển LED.
- `frequency`: Tần số nhấp nháy.

Mỗi task được tạo với stack depth bằng 128 và mức ưu tiên bằng 1.

### 4.3. Điều khiển nhấp nháy LED

Hàm `LED_BlinkTask()` thực hiện:

1. Nhận thông số của LED.
2. Tính nửa chu kỳ dựa trên tần số yêu cầu.
3. Chuyển thời gian từ mili giây sang tick FreeRTOS.
4. Đảo trạng thái chân GPIO.
5. Gọi `vTaskDelay()` để nhường CPU cho các task khác.
6. Lặp lại quá trình.

Công thức tính thời gian giữa hai lần đảo trạng thái:

**T = 1000 / (2 × f) (ms)**

Trong đó:
- T: Thời gian giữa hai lần đảo trạng thái, tính bằng mili giây.
- f: Tần số nhấp nháy LED (Hz).

Tần số thực tế có thể sai lệch nhỏ do độ phân giải tick và thời gian thực thi task.

### 4.4. Khởi động FreeRTOS

Sau khi tạo thành công 3 task, chương trình gọi `vTaskStartScheduler()` để khởi động bộ lập lịch FreeRTOS.

Scheduler quản lý việc thực thi các task, cho phép cả 3 LED nhấp nháy độc lập về mặt logic mà không cần sử dụng vòng lặp delay chặn CPU.

## 5. Kết quả mong đợi

Sau khi chương trình hoạt động:

- **LED 1 (PA0):** Nhấp nháy chậm với tần số 0.1 Hz, mỗi trạng thái sáng/tắt kéo dài khoảng 5 giây.
- **LED 2 (PA1):** Nhấp nháy với tần số 1 Hz, mỗi trạng thái kéo
- **LED 3 (PA2):** Nhấp nháy nhanh với tần số 10 Hz, mỗi trạng thái kéo dài khoảng 0.05 giây.

Ba LED hoạt động với tốc độ khác nhau nhờ ba task FreeRTOS riêng biệt.

6. Hướng dẫn chạy chương trình

Mở project STM32F10x trong môi trường phát triển.

Đảm bảo project đã cấu hình STM32 Standard Peripheral Library và FreeRTOS phù hợp với vi điều khiển.

Thêm mã nguồn chương trình vào project.

Kết nối LED với các chân PA0, PA1, PA2 qua điện trở hạn dòng.

Biên dịch chương trình.

Nạp chương trình xuống vi điều khiển.

Quan sát trạng thái nhấp nháy của 3 LED.

Lưu ý: Nếu sử dụng LED tích hợp trên board, cần kiểm tra chân GPIO thực tế của LED và điều chỉnh mã nguồn khi cần thiết.

7. Các hàm chính được sử dụng

Hàm

Chức năng

RCC_APB2PeriphClockCmd()

Bật clock cho GPIOA

GPIO_Init()

Khởi tạo GPIO

GPIO_SetBits()

Đặt GPIO lên mức HIGH

GPIO_ResetBits()

Đặt GPIO xuống mức LOW

GPIO_ReadOutputDataBit()

Đọc trạng thái đầu ra GPIO

xTaskCreate()

Tạo task FreeRTOS

pdMS_TO_TICKS()

Chuyển mili giây sang tick

vTaskDelay()

Tạm dừng task theo số tick

vTaskStartScheduler()

Khởi động bộ lập lịch

vTaskDelete()

Xóa task FreeRTOS

8. Kết luận

Bài tập minh họa cách sử dụng FreeRTOS để điều khiển nhiều thiết bị ngoại vi với các chu kỳ hoạt động khác nhau trên STM32F10x.

Thông qua việc tạo 3 task điều khiển LED, chương trình giúp hiểu rõ hơn về cơ chế lập lịch đa nhiệm, truyền tham số cho task, điều khiển GPIO và quản lý thời gian bằng FreeRTOS.

Bài thực hành buổi 4 – STM32F10x / FreeRTOS / GPIO LED Control
