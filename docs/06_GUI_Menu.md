# Phân tích chi tiết: GUI_Menu.h và GUI_Menu.cpp

Tài liệu này giải thích logic điều hướng và cách vẽ các màn hình Menu, bao gồm Main Menu, Settings, Load/Save Game và màn hình chọn Tướng (Character Select).

## 1. Khái Quát

Hệ thống Menu trong Caro Battle hoạt động dựa trên máy trạng thái (State Machine). Biến `ui.currentScreen` quyết định màn hình nào đang hiển thị:
*   `0`: Main Menu
*   `2`: Settings (Cài đặt)
*   `5`: Load Game (Tải ván cờ cũ)
*   `6`: Save Game (Lưu ván cờ)
*   `8`: Character Select (Màn hình chọn Tướng)

File này chứa 2 hàm lớn: `UpdateMenuScreens` và `DrawMenuScreens`, trong đó sử dụng cấu trúc `if... else if...` theo biến `currentScreen`.

---

## 2. Hàm `UpdateMenuScreens` (Xử lý Logic, Bàn phím, Chuột)

### Screen 0: Main Menu
*   Cập nhật `ui.menuScrollY` (Hiệu ứng cuộn mượt cho thanh highlight).
*   **Hạt tàn lửa (Embers):** Tính toán vật lý cho 30 hạt lửa bay từ dưới lên màn hình. Hết tuổi thọ thì reset vị trí ở đáy màn hình.
*   Lắng nghe phím W/S hoặc chuột để thay đổi biến `menuSelection` (từ 0 đến 5).
*   Khi bấm Enter/Click: Chuyển `currentScreen` tương ứng (Ví dụ: 0 -> Screen 8 (New Game), 1 -> Screen 5 (Load), 5 -> Exit).

### Screen 2: Settings
*   Sử dụng chuột hoặc phím W/S để chọn mục cài đặt (`settingSelection`).
*   **Thanh âm lượng (Music Volume):** Kiểm tra xem chuột có click và kéo (drag) trên thanh tọa độ âm lượng không. Nếu có, tính toán tỷ lệ `t` (từ 0.0 đến 1.0) và truyền vào `SetMusicVolume(ui.bgMusic, t)`.
*   Thay đổi Input Type: Chuột (0) hoặc Bàn phím (1). Bật/tắt nhạc.

### Screen 5 & 6: Load/Save Game
*   **Load (Screen 5):** Di chuyển lên/xuống giữa 4 Slot (từ 0 đến 3). Bấm Enter để gọi `LoadGameSlot` (trong LogicControl). Bấm Delete (hoặc Backspace) để xóa file Save.
*   **Save (Screen 6):** Sử dụng hàm `GetCharPressed()` của raylib để đọc ký tự nhập từ bàn phím, lưu vào chuỗi `ui.nameInput` (Tên file Save do người chơi đặt). Phím Backspace để xóa ký tự. Enter để gọi `SaveGameSlot`.

### Screen 8: Character Select (Chọn Tướng)
Đây là màn hình phức tạp với 4 Phase (Giai đoạn):
*   **Phase -1 (Chọn Mode):** Ấn A/D để chọn đánh "VS Player" hoặc "VS Bot".
*   **Phase 0 (P1 Chọn Tướng):** Phím A/D để xoay vòng 5 Tướng (biến `ui.p1HeroSelection`).
*   **Phase 1 (P1 Nhập Tên):** Bàn phím để gõ tên. Nếu đánh với Bot, nhập tên P1 xong, máy sẽ ngẫu nhiên chọn Tướng cho Bot (khác tướng P1) và chuyển ngay vào ván game.
*   **Phase 2 (P2 Chọn Tướng):** Phím A/D để xoay vòng, nhưng code có vòng lặp `while` chặn không cho P2 chọn trùng Tướng với P1.
*   **Phase 3 (P2 Nhập Tên):** Nhập tên xong bấm Enter để gọi `InitGame(game, 0)` và chuyển vào màn hình chơi cờ (`currentScreen = 1`).

---

## 3. Hàm `DrawMenuScreens` (Vẽ Giao Diện)

### Screen 0: Main Menu
*   Vẽ `bgMenu` toàn màn hình. Áp dụng lớp đen (Gradient) nửa bên phải để làm nổi bật chữ.
*   **Hiệu ứng Chữ Tựa Game:** Tính toán sóng Sin `sinf(GetTime() * 2)` để tạo hiệu ứng "nhịp tim" (Glow Pulse). Chữ "CARO BATTLE" tỏa sáng vàng và đỏ ra xung quanh.
*   Vẽ các hạt tàn lửa bay lên.
*   **Thanh Highlight Mượt:** Vẽ nhiều hình chữ nhật mờ đè lên nhau ở tọa độ `ui.menuScrollY` để tạo thành một thanh sáng đẹp mắt bao lấy Menu item đang được chọn. Cập nhật size chữ to lên (52) khi selected, nhỏ đi (42) khi unselected.

### Screen 2: Settings
*   Sử dụng hiệu ứng "Gothic Gold" (màu Vàng Kim phong cách tối).
*   Đặc trưng: Có hàm Lambda `DrawOrnateLine` định nghĩa ngay bên trong hàm Draw. Nó tự động vẽ ra các đường kẻ ngang có viền hoa văn, kim cương, thánh giá (Diamond, Cross) dọc theo thanh ngang ở trên và dưới Menu.
*   Vẽ thanh kéo âm lượng với cục "Knob" hình chữ nhật hiển thị số %.

### Screen 5: Load Game
*   Chia đôi màn hình: Cột trái (Danh sách 4 Slot), Cột phải (Preview ván game được chọn).
*   Cột trái: Sử dụng hàm `PeekGameSlot` để đọc hờ thông tin của file save. Nếu có file, hiển thị Tên, Mode, Round, Thời gian bằng nhiều lớp màu và highlight.
*   Cột phải: Hiển thị một bàn cờ thu nhỏ (Miniature Board). Quét mảng `board` của file save và vẽ `ui.cell` với `ui.heroIcon` thu nhỏ ở giữa màn hình bên phải.

### Screen 8: Character Select
*   Vẽ 5 khung thẻ (Card) nằm dọc theo giữa màn hình.
*   Mỗi thẻ có một tông màu riêng biệt (Đỏ cho Fire Knight, Xanh lơ cho Archer, Vàng cho Assassin...). Lớp nền thẻ được tô Gradient theo tông màu đó.
*   Khi thẻ được P1 hoặc P2 trỏ vào (Selected), thẻ nổi lên trên, viền thẻ đổi màu sáng chói và phát ra vầng hào quang (Glow Rings).
*   Đáy của mỗi thẻ sử dụng `DrawCharAnim` để vẽ khung hình `IDLE` (Đứng thở) của Tướng.
*   **Hạt Rune (Rune Particles):** Phát sinh (Spawn) liên tục các hạt ánh sáng bay xung quanh thẻ tướng đang được chọn. Code dùng Sin/Cos để hạt bay tản ra hoặc xoay quanh chân Tướng.
*   Khi nhập tên (Phase 1, 3) hoặc chọn Game Mode (Phase -1), vẽ một lớp mờ đen (Black Overlay) toàn màn hình và hiển thị Box thông báo ở chính giữa để nhập liệu. Nhập liệu có con trỏ nhấp nháy (dấu `|`) mô phỏng terminal.
