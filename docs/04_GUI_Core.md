# Phân tích chi tiết: GUI.h và GUI.cpp

Tài liệu này giải thích hệ thống đồ họa cốt lõi, quản lý trạng thái giao diện và hoạt ảnh (Animation) của trò chơi Caro Battle.

## 1. File `GUI.h`

`GUI.h` định nghĩa các struct phục vụ cho việc hiển thị, âm thanh và trạng thái màn hình.

### Các Struct Con

#### 1. `CharAnim` (Quản lý Hoạt Ảnh Sprite Sheet)
Lưu trữ thông tin để cắt và chạy ảnh động từ một tấm ảnh lớn (Sprite sheet).
*   `spriteSheet`: Biến kiểu `Texture2D` chứa toàn bộ hình ảnh các khung hình nối tiếp nhau.
*   `frameWidth`, `frameHeight`: Chiều rộng và chiều cao của 1 khung hình (Cắt ra từ sprite sheet).
*   `frameCount`: Tổng số khung hình có trong animation này.
*   `currentFrame`: Khung hình đang được hiển thị.
*   `frameTimer`: Bộ đếm thời gian (giây) tích lũy.
*   `frameDuration`: Thời gian cần thiết để chuyển sang khung hình tiếp theo (Vd: 0.1s).

#### 2. `CellEffect` (Hiệu ứng Vòng Cầu Lửa/Phép Thuật trên ô cờ)
*   `r, c`: Tọa độ ô cờ.
*   `player`: Của Player 1 hay Player 2 (Xác định màu sắc hiệu ứng).
*   `timer`, `duration`: Quản lý thời gian tồn tại của hiệu ứng trước khi biến mất.

#### 3. Hệ thống Hạt (Particles)
*   `Ember`: Các hạt tàn lửa nhỏ bay lơ lửng ngẫu nhiên ở Main Menu. (Tọa độ x/y, vận tốc vx/vy, tuổi thọ `life`, độ mờ `alpha`, kích thước `size`).
*   `CharParticle`: Các hạt hiệu ứng đặc trưng của từng Tướng (Hero) xuất hiện ở màn hình Chọn Tướng.

### Cấu trúc `UIState` (Trạng thái UI toàn cục)
Quản lý toàn bộ tài nguyên hình ảnh, âm thanh và luồng điều hướng của giao diện.

| Trường | Mục đích |
| :--- | :--- |
| `currentScreen` | Quản lý việc chuyển màn hình. `0`: Main Menu, `1`: Gameplay, `2`: Settings, `3`: Credits, `5`: Load, `6`: Save, `8`: Character Select. |
| **Fonts & Audio** | `mainFont` (Font chữ Gothic), `bgMusic` (Nhạc nền), Các `Sound` hiệu ứng (đánh cờ, nổ bom, nút bấm). |
| **Textures (Background)**| `bgMenu`, `bgGame`, `bgSettings`, v.v. Hình nền cho từng màn hình. |
| **UI Elements** | Hình ảnh ô cờ (`cell`), khung bàn cờ (`boardFrame`). |
| **Hero Graphics** | Mảng `heroIdle[6]` và `heroAttack[6]` kiểu `CharAnim` (Ảnh động của Tướng). `heroIcon[6]` (Icon bé nằm trên bàn cờ). `heroProfile[6]` (Ảnh thẻ lớn). |
| **Animation State** | `activeAnimType` (Đang diễn hoạt ảnh Idle, Attack hay Death), `animTimer` (đếm ngược thời gian diễn), v.v. Dùng trong màn hình Game. |
| **Menu Variables** | `menuSelection`, `settingSelection`, `loadSelection`, `saveSelection`... Lưu vị trí người dùng đang chọn bằng phím Mũi tên. |
| **Input Buffers** | `nameInput`, `p1NameInput`, `p2NameInput` (Lưu chuỗi ký tự người dùng gõ từ bàn phím). |

### Hằng số `HERO_MAP`
```cpp
const int HERO_MAP[5] = {1, 2, 3, 4, 5};
```
Mảng 6 Hero nhưng Hero index 0 (`black_knight`) bị ẩn đi, chỉ dùng cho trùm cuối ẩn hoặc dự phòng. Các Hero có thể chọn nằm từ index 1 đến 5 (Fire Knight, Green Archer, Earth Assassin, Metal Blade, Water Mage).

---

## 2. File `GUI.cpp`

### Hàm `InitGUI(UIState& ui)`
Hàm này chạy một lần duy nhất lúc khởi động game. Nhiệm vụ:
1.  **Load Fonts & Audio:** Tải Font chữ `.ttf`. Khởi tạo Audio Device. Tải nhạc `.mp3` và thiết lập âm lượng mặc định.
2.  **Load Textures nền:** Gọi `LoadTexture` cho các tấm hình nền, ô cờ.
3.  **Khởi tạo Animation Tướng (Heroes):**
    Sử dụng vòng lặp duyệt qua các Hero. Gọi hàm tiện ích `LoadCharAnim` để nạp sprite sheet, tự động tính toán `frameWidth` bằng cách lấy `texture.width / frameCount`. Các tốc độ animation (frameDuration) được set từ 0.10s đến 0.15s tùy Hero.
4.  **Khởi tạo Mìn & Khói:** Load sprite sheet cho Mìn (Mine) nổ.

### Hàm `UnloadGUI(UIState& ui)`
Dọn dẹp RAM/VRAM khi thoát game.
Nguyên tắc: Giải phóng tất cả những gì đã `Load`. Gọi `UnloadTexture`, `UnloadSound`, `UnloadFont`, `CloseAudioDevice`. Phải làm cẩn thận để tránh lỗi rò rỉ bộ nhớ (Memory Leak).

### Các hàm `UpdateGUI` và `DrawGUI` (Hàm Điều Phối)
Hai hàm này rất ngắn. Chúng đóng vai trò là "Tổng đài viên" điều hướng (Router).
Dựa vào giá trị của biến `ui.currentScreen`, chúng sẽ gọi đúng hàm Update hoặc Draw của file tương ứng.
*   Nếu `currentScreen == 1`: Gọi `UpdateGUIGame(game, ui)` trong `GUI_Game.cpp`.
*   Nếu không phải 1: Gọi `UpdateMenuScreens(game, ui)` trong `GUI_Menu.cpp`.

Đặc biệt trong `DrawGUI`, sau khi vẽ xong các thành phần của game hoặc menu, hàm luôn vẽ thêm một **Custom Mouse Cursor** (Con trỏ chuột tùy chỉnh) tại vị trí `GetMousePosition()`.
