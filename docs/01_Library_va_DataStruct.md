# Phân tích chi tiết: Library.h và DataStruct.h

Tài liệu này giải thích chi tiết cấu trúc dữ liệu cốt lõi và các thư viện được sử dụng trong dự án Caro Battle.

## 1. File `Library.h`

**Mục đích:** Đóng vai trò là file trung tâm chứa tất cả các thư viện chuẩn (C/C++), thư viện đồ họa (raylib) và các file header tự định nghĩa của dự án. File này được include vào `main.cpp` và các file khác để đảm bảo mọi nơi đều có quyền truy cập vào tất cả các thư viện cần thiết mà không phải khai báo lại nhiều lần.

### Các thư viện được sử dụng:
*   `<stdio.h>`: Thư viện chuẩn C cho input/output (ví dụ: in ra console, đọc/ghi file như `fread`, `fwrite`).
*   `<stdlib.h>`: Thư viện chuẩn C quản lý bộ nhớ động, chuyển đổi kiểu dữ liệu và các hàm tiện ích (`rand`, `srand`).
*   `<string.h>`: Thư viện xử lý chuỗi C-string (ví dụ: `strcpy`, `strcmp`).
*   `<time.h>`: Thư viện xử lý thời gian, dùng để lấy thời gian thực (`time(0)`) làm seed cho hàm random (`srand`) hoặc lấy thời gian lưu game.
*   `<math.h>`: Thư viện toán học chuẩn (sử dụng cho các phép toán sin, cos trong hiệu ứng UI).
*   `"raylib.h"`: Thư viện đồ họa chính của game, chịu trách nhiệm vẽ cửa sổ, hình ảnh, âm thanh, bàn phím và chuột.
*   Các file của dự án: `"DataStruct.h"`, `"LogicControl.h"`, `"GUI.h"`, `"GUI_Game.h"`, `"GUI_Menu.h"`.

---

## 2. File `DataStruct.h`

**Mục đích:** Định nghĩa các hằng số và cấu trúc dữ liệu cốt lõi (struct) để lưu trữ trạng thái của toàn bộ trò chơi.

### Các Hằng Số (Constants / Macros)
*   `#define BOARD_SIZE 15`: Kích thước bàn cờ Caro là 15x15.
*   `#define WIN_CONDITION 5`: Điều kiện chiến thắng là có 5 quân cờ liên tiếp trên một hàng (ngang, dọc, chéo).

### Cấu trúc `Point` (Một ô trên bàn cờ)
Lưu trữ trạng thái của từng ô lưới trên bàn cờ 15x15.

| Trường | Kiểu dữ liệu | Ý nghĩa & Giá trị hợp lệ |
| :--- | :--- | :--- |
| `c` | `int` | Trạng thái ô cờ: `0` (Ô trống), `1` (Quân X của Player 1), `2` (Quân O của Player 2). |
| `isMine` | `bool` | Xác định xem ô này có chứa mìn (bom) ẩn hay không (Dùng cho chế độ Booming Caro). `true` (có mìn), `false` (không có). |
| `isScanned` | `bool` | Xác định xem ô mìn này đã bị người chơi dùng kỹ năng "Scan" (Quét mìn) phát hiện hay chưa. `true` (đã quét), `false` (chưa quét). |

### Cấu trúc `Player` (Người chơi)
Lưu trữ toàn bộ thông tin của một người chơi trong trận đấu.

| Trường | Kiểu dữ liệu | Ý nghĩa |
| :--- | :--- | :--- |
| `name` | `char[36]` | Tên hiển thị của người chơi (Tối đa 35 ký tự + ký tự kết thúc chuỗi `\0`). |
| `winCount` | `int` | Số ván thắng của người chơi này trong phiên chơi hiện tại. |
| `loseCount` | `int` | Số ván thua. |
| `stepCount` | `int` | Số bước (số quân cờ) mà người chơi này đã đi trong ván đấu hiện tại. |
| `hp` | `int` | Máu (Hit Points) của người chơi. Dùng trong chế độ Booming Caro. Mặc định là 3. Trúng mìn bị trừ 1. Mất hết HP thì thua. |
| `scansLeft` | `int` | Số lần dùng kỹ năng quét radar (tìm mìn). Khởi tạo là 3 lần mỗi ván. Dùng trong chế độ Booming Caro. |

### Cấu trúc `GameState` (Trạng thái tổng thể của Game)
Đây là "trái tim" của phần logic. Một biến kiểu `GameState` chứa toàn bộ dữ liệu hiện tại của ván đấu.

| Trường | Kiểu dữ liệu | Ý nghĩa |
| :--- | :--- | :--- |
| `board` | `Point[BOARD_SIZE][BOARD_SIZE]` | Mảng 2 chiều 15x15 đại diện cho bàn cờ. |
| `player1`, `player2` | `Player` | Dữ liệu của người chơi 1 (X) và người chơi 2 (O). |
| `isPlayer1Turn` | `bool` | `true`: Đến lượt Player 1 (X). `false`: Đến lượt Player 2 (O). |
| `moveCount` | `int` | Tổng số nước đã đi trên bàn cờ của cả hai người. Dùng để kiểm tra hòa. |
| `roundCount` | `int` | Số hiệp đấu hiện tại. |
| `gameMode` | `int` | Chế độ chơi: `0` (Classic - Truyền thống), `1` (Booming Caro - Chế độ có mìn). |
| `matchStatus` | `int` | Trạng thái ván cờ: `0` (Đang chơi), `1` (P1 Thắng), `2` (P2 Thắng), `3` (Hòa). |
| `cursorRow`, `cursorCol` | `int` | Tọa độ (hàng, cột) của con trỏ ảo trên bàn cờ khi người chơi dùng bàn phím. |
| `inputType` | `int` | Kiểu điều khiển: `0` (Dùng Chuột - Mouse), `1` (Dùng Bàn phím). |
| `isVsBot` | `bool` | `true` nếu chơi PvE (Với máy), `false` nếu PvP. |
| `botPlayer` | `int` | Cho biết Bot cầm quân nào (Thường là Player 2, `botPlayer = 2`). |
| `botThinkTimer` | `float` | Bộ đếm thời gian (giây) để tạo độ trễ giả khi Bot "suy nghĩ". |
| `lastMoveRow`, `lastMoveCol`| `int` | Tọa độ của nước đi gần nhất vừa được đánh xuống. |
| `winLine` | `int[5][2]` | Mảng lưu tọa độ của 5 ô tạo thành đường chiến thắng. |
| `p1HeroSelection`, `p2HeroSelection`| `int` | Chỉ số (index) của Tướng (Hero) mà P1 và P2 đã chọn. |
| `saveName` | `char[30]` | Tên của file save game do người chơi đặt khi lưu game. |
| `saveTime` | `char[30]` | Chuỗi ký tự biểu diễn thời gian thực lúc bấm lưu game. |
