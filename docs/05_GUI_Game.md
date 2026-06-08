# Phân tích chi tiết: GUI_Game.h và GUI_Game.cpp

Tài liệu này giải thích cách vẽ và xử lý tương tác của màn hình chơi game (Gameplay Screen) trong Caro Battle. Màn hình này là nơi phức tạp nhất vì nó chứa bảng cờ, animation 2D của Tướng, hiệu ứng cháy nổ và giao diện Booming Mode.

## 1. File `GUI_Game.h`

Chứa khai báo 2 hàm chính:
*   `void UpdateGUIGame(GameState& game, UIState& ui);`
*   `void DrawGUIGame(const GameState& game, const UIState& ui);`

## 2. File `GUI_Game.cpp`

### Hàm Phụ Trợ: `CheckBlockVariant`
**Mục đích:** Hàm heuristic thông minh dùng riêng cho Animation đồ họa. Nó không ảnh hưởng tới kết quả ván cờ, mà chỉ kiểm tra xem nước cờ vừa đánh có phải là một "nước chặn đối thủ" hay không.
*   **Cách hoạt động:** Nó mô phỏng việc đặt cờ của *đối thủ* vào ô vừa đánh, sau đó quét 4 hướng xem nếu để đối thủ đánh thì có tạo thành chuỗi 3 hoặc 4 không.
*   **Kết quả:** Nếu có, nó đánh dấu đây là một nước "Block" (chặn nguy hiểm) và sẽ trigger animation đánh `Attack S2` (chiêu thức thứ 2 - phòng thủ) thay vì `Attack S1` (đánh thường).

### Hàm `UpdateGUIGame(GameState& game, UIState& ui)`
Xử lý logic 60 khung hình/giây của màn hình Gameplay. Gồm các State (trạng thái) chính:

#### 1. Xử lý State Kết Thúc Trận Đấu (`game.matchStatus != 0`)
*   Nếu `ui.endGamePhase == 0`: Khi vừa có người thắng, chuyển sang Phase 1. Lấy chỉ số Tướng thua (`loserHeroIdx`) và đổi hoạt ảnh của Tướng đó sang trạng thái `DEATH` (chết). Kích hoạt đồng hồ đếm ngược `endGameTimer`.
*   Nếu `ui.endGamePhase == 1`: Chờ khoảng 1.5 giây (để animation chết diễn ra và người xem nhận thức được kết quả). Sau đó chuyển sang Phase 2 (hiện màn hình Win).
*   Nếu `ui.endGamePhase == 2`: Chờ người chơi bấm phím.
    *   Bấm Enter (Play Again): Chơi lại vòng mới (`ResetRound`), reset các biến trạng thái, đặt lại animation về `IDLE`.
    *   Bấm M (Main Menu): Trở về màn hình chính, khởi tạo lại toàn bộ game.

#### 2. Xử lý State Hoạt Ảnh Tấn Công (`activeAnimType == 1`)
*   Khi có người đánh cờ, game tạm khóa tương tác (Lock) và chạy `Attack Animation`.
*   Cập nhật `frameTimer` của sprite sheet tấn công (của người vừa đánh).
*   Khi chạy đến khung hình cuối cùng của animation tấn công, state được reset về `IDLE`, chuyển lượt cho người tiếp theo, và hiệu ứng khói/vòng tròn phép thuật được tạo ra tại ô vừa đánh.

#### 3. Xử lý Đánh Cờ (Khi đang `IDLE`)
*   **Đánh bằng Chuột (`inputType == 0`):** Lấy vị trí `GetMousePosition()`. Tính toán ô cờ (r, c) dựa trên tọa độ chuột. Nếu click hợp lệ, bắt đầu Animation Tấn Công.
*   **Đánh bằng Bàn Phím (`inputType == 1`):** Dùng các phím W, A, S, D để di chuyển `cursorRow`, `cursorCol` trên bàn cờ. Bấm Enter để đánh.
*   **Kỹ năng Scan Mìn:** Nếu ấn phím 1 hoặc 2 (tùy lượt P1 hay P2), và ở chế độ Booming, trừ đi số lần scan (`scansLeft`) và đổi màu các ô xung quanh con trỏ thành màu vàng nhạt.

#### 4. Xử lý Bot
*   Nếu là lượt của Bot (`isVsBot` và đến lượt `botPlayer`), bắt đầu đếm ngược `botThinkTimer`. Khi đếm hết 0.5s, gọi hàm `BotMove()` từ LogicControl. Sau đó Bot cũng kích hoạt Animation Tấn Công y như người chơi.

### Hàm `DrawCharAnim` (Vẽ Ảnh Động)
Hàm tiện ích nội bộ để vẽ sprite. Nó xác định Tướng nào đang diễn hoạt cảnh nào, lấy đúng tọa độ `currentFrame` từ sprite sheet và vẽ ra.
Điểm đặc biệt: Tướng của Player 2 sẽ bị lật ngược theo trục X (`dest.width *= -1`) để quay mặt vào trong màn hình đối đầu với Player 1.

### Hàm `DrawGUIGame(const GameState& game, const UIState& ui)`
Hàm vẽ (Render) của gameplay. Có nhiều lớp (Layers):

1.  **Vẽ Nền (Background):** Vẽ ảnh nền tối. Thêm một lớp gradient màu đen phủ lên trên để làm dịu mắt người chơi, giúp bàn cờ nổi bật.
2.  **Vẽ Bàn Cờ (Board):** Vẽ khung gỗ (`ui.boardFrame`) ở giữa màn hình. Dùng vòng lặp kép 15x15 để vẽ 225 ô cờ (`ui.cell`).
3.  **Vẽ Quân Cờ (Pieces) & Hiệu Ứng Nổ:**
    *   Nếu ô có cờ `c == 1` hoặc `c == 2`, vẽ Icon của Tướng tương ứng.
    *   Nếu ô đó nằm trong danh sách `CellEffect`, thay vì vẽ Icon tĩnh, nó vẽ animation vòng xoáy phép thuật / tàn lửa để có cảm giác Tướng vừa "chưởng" vào ô đó.
    *   Nếu ô `c == 3` (Mìn nổ), vẽ hoạt ảnh lửa nổ (`mineExplosion`).
    *   Hiển thị Scan: Nếu ô `isScanned`, tô viền vàng. Nếu có mìn, tô đỏ mờ.
4.  **Vẽ Con Trỏ:** Nếu dùng bàn phím, vẽ một hình chữ nhật nhấp nháy tại vị trí `cursor`.
5.  **Vẽ Giao Diện 2 Bên (HUD):**
    *   Player 1 bên trái, Player 2 bên phải.
    *   Sử dụng màu "Theme Color" đặc trưng của từng Tướng (Ví dụ: Fire Knight thì màu viền Đỏ, Water Mage màu viền Xanh dương).
    *   Vẽ bục đứng 3D (Hàm `drawSpotlight`) chiếu sáng từ dưới lên.
    *   Gọi `DrawCharAnim` để vẽ Tướng đang đứng (hoặc đang đánh/chết).
    *   Vẽ Bảng Tên: tên người chơi, số hiệp, chế độ chơi.
    *   **Booming HUD:** Vẽ biểu tượng Trái Tim (HP) và Kính Lúp (Scans Left) nếu đang chơi Booming Mode.
6.  **Vẽ Màn Hình Kết Thúc (End Screen):**
    *   Khi thắng, làm tối toàn bộ màn hình xung quanh (Vignette), chiếu đèn sáng vào 5 ô chiến thắng.
    *   Vẽ chữ "PLAYER X WINS!" nhấp nháy bằng màu Vàng (Gold) và Đỏ (Red).
    *   Hiển thị 2 nút [ENTER] Chơi tiếp, [M] Trở về Menu.
