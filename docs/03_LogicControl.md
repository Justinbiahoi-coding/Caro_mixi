# Phân tích chi tiết: LogicControl.h và LogicControl.cpp

Tài liệu này giải thích chi tiết các quy tắc và logic cốt lõi của trò chơi Caro, từ khởi tạo, kiểm tra điều kiện thắng, xử lý đánh mìn, đến AI (trí tuệ nhân tạo) của Bot.

## Khái Quát Logic Trò Chơi
`LogicControl` đóng vai trò là "Bộ não" của game. Nó KHÔNG chứa bất kỳ code nào liên quan đến đồ họa hay âm thanh. Toàn bộ input truyền vào đây (tọa độ người chơi đánh), và output trả ra là trạng thái game mới (`GameState`).

## Phân Tích Các Hàm Chi Tiết

### 1. Hàm `InitGame(GameState& game, int mode)`
**Mục đích:** Khởi tạo một ván đấu mới, đặt lại toàn bộ bàn cờ và thông số người chơi.
*   **Tham số:**
    *   `GameState& game`: Biến lưu trữ trạng thái game cần khởi tạo.
    *   `int mode`: 0 (Classic) hoặc 1 (Booming).
*   **Thuật toán:**
    1.  Xóa trắng bàn cờ bằng vòng lặp kép (`c = 0`, `isMine = false`, `isScanned = false`).
    2.  Nếu ở chế độ Booming (mode 1), gọi `srand(time(NULL))` và random đặt 30 quả mìn ngẫu nhiên trên bàn cờ. Đảm bảo ô đó chưa có mìn thì mới đặt.
    3.  Reset các biến đếm: `moveCount = 0`, `roundCount = 1`, lượt đi đầu luôn là Player 1 (`isPlayer1Turn = true`), `matchStatus = 0`.
    4.  Khởi tạo cho cả 2 người chơi: `hp = 2`, `scansLeft = 2`, `stepCount = 0`, `winCount = 0`, `loseCount = 0`.
    5.  Thiết lập con trỏ ở giữa bàn cờ (`cursorRow = 7`, `cursorCol = 7`).

### 2. Hàm `ResetRound(GameState& game)`
**Mục đích:** Dọn dẹp bàn cờ sau một hiệp đấu để bắt đầu hiệp mới, KHÔNG reset điểm số hay số round.
*   **Tham số:** `GameState& game`.
*   **Thuật toán:**
    1.  Xóa mảng `board` tương tự như `InitGame`.
    2.  Nếu Booming mode, tạo lại 30 quả mìn mới ngẫu nhiên.
    3.  Đổi lượt đi đầu tiên cho người chơi còn lại (`isPlayer1Turn = !game.isPlayer1Turn`).
    4.  Reset lại máu (`hp = 2`) và số lần scan (`scansLeft = 2`) cho hiệp mới.
    5.  Tăng `roundCount++`.
    6.  Đặt `matchStatus = 0`, reset biến đếm `moveCount = 0`.

### 3. Hàm `MakeMove(GameState& game, int row, int col)`
**Mục đích:** Xử lý một nước đi khi có người chơi (hoặc Bot) click vào ô `(row, col)`.
*   **Kiểm tra tính hợp lệ:** Trả về `false` nếu ô `(row, col)` nằm ngoài bàn cờ, hoặc ô đó đã có cờ (`board[row][col].c != 0`), hoặc game đã kết thúc (`matchStatus != 0`).
*   **Xử lý chế độ Booming:**
    *   Nếu đang chơi Mode 1 và ô đó có mìn (`isMine == true`): 
        *   Ô này bị lộ mìn (`isScanned = true`).
        *   Người vừa đánh bị trừ 1 HP. 
        *   Nếu HP <= 0, game kết thúc và cập nhật `matchStatus` thành đối phương thắng.
*   **Đặt cờ:** Đặt `c = 1` (nếu P1) hoặc `c = 2` (nếu P2) vào ô.
*   **Cập nhật:** Tăng `moveCount`, `stepCount`, và cập nhật tọa độ `lastMoveRow`, `lastMoveCol`.
*   **Kiểm tra thắng/hòa:**
    *   Chỉ khi `matchStatus == 0`, gọi hàm `CheckWin`. Nếu thắng, cập nhật `matchStatus` và cộng `winCount` / `loseCount`.
*   **Chuyển lượt:** Nếu ván đấu chưa kết thúc, đổi giá trị của `isPlayer1Turn`. Trả về `true`.

### 4. Hàm `ScanMine(GameState& game, int centerRow, int centerCol)`
**Mục đích:** Xử lý kỹ năng Scan (Quét mìn 3x3) trong Booming Mode.
*   **Điều kiện:** Trận đấu đang diễn ra (`matchStatus == 0`), Mode 1, và người chơi hiện tại còn lượt scan (`scansLeft > 0`).
*   **Thuật toán:** Duyệt qua vùng 3x3 xung quanh ô `(centerRow, centerCol)`. Gán `isScanned = true` cho các ô nằm trong bàn cờ. Sau đó trừ `scansLeft` đi 1. Trả về `true` nếu thành công.

### 5. Hàm `CheckWin(GameState& game, int lastRow, int lastCol)`
**Mục đích:** Kiểm tra xem nước đi vừa đánh tại `(lastRow, lastCol)` có tạo thành chuỗi 5 quân chiến thắng hay không.
*   **Thuật toán:**
    *   Sử dụng mảng `directions[4][2]` chứa 4 hướng: Ngang `{0, 1}`, Dọc `{1, 0}`, Chéo chính `{1, 1}`, Chéo phụ `{1, -1}`.
    *   Dùng vòng lặp `while` trực tiếp để đếm số quân cùng loại theo 2 phía xuôi và ngược (`+dx,+dy` và `-dx,-dy`).
    *   Biến `count` đếm số quân, biến `blocks` đếm số đầu bị chặn bởi biên bàn cờ hoặc quân địch.
    *   **Luật chặn 2 đầu:** Trả về thắng nếu `count >= 5 && blocks < 2`.
    *   Nếu thắng, lưu tọa độ 5 ô chiến thắng vào mảng `winLine[5][2]` để phục vụ hiệu ứng đồ họa, và trả về chỉ số người thắng (`1` hoặc `2`).
    *   Kiểm tra hòa: Nếu `moveCount == BOARD_SIZE * BOARD_SIZE`, trả về `3`. Không có kết quả trả về `0`.

### 6. Hàm `GetLineStatus(GameState& game, int row, int col, int dx, int dy, int player, int& count, int& blocks)`
**Mục đích:** Hàm phân tích trạng thái một đường cờ, hỗ trợ riêng cho AI (Hàm `EvaluatePosition`). Dùng tham chiếu `&count` và `&blocks` để trả về nhiều kết quả.
*   **Thuật toán:** Duyệt vòng lặp `while` theo hướng chỉ định (`+dx, +dy`) và ngược lại (`-dx, -dy`), KHÔNG bao gồm ô hiện tại `(row, col)`. Tăng `count` cho mỗi quân cùng phe (`player`). Tăng `blocks` nếu đụng tường hoặc quân đối phương.

### 7. Trí Tuệ Nhân Tạo (AI - Bot)
Hệ thống AI sử dụng thuật toán **Heuristic Evaluation** (Chấm điểm vị trí). Do giới hạn hiệu năng, nó duyệt tất cả các ô trống để tính điểm và chọn ô tốt nhất thay vì duyệt cây đệ quy sâu.

#### Bảng Điểm Đánh Giá (Heuristic Score Matrix)
Sử dụng mảng 2 chiều kích thước `[3][6]` đại diện cho `[số đầu bị chặn][số quân liên tiếp]`.
```cpp
int AttackScore[3][6] = {
    { 0, 9, 54, 162, 1458, 13112 }, // 0 đầu bị chặn (Mở 2 đầu)
    { 0, 2, 18, 54,  729,  13112 }, // 1 đầu bị chặn
    { 0, 0, 0,  0,   0,    0     }  // 2 đầu bị chặn (Vô dụng)
};

int DefenseScore[3][6] = {
    { 0, 3, 27, 99,  729,  6561 },
    { 0, 1, 9,  33,  364,  6561 },
    { 0, 0, 0,  0,   0,    0    }
};
```
(Lưu ý: Điểm Tấn công của cùng một tình thế luôn cao hơn Điểm Phòng thủ một chút để Bot ưu tiên tấn công chủ động hơn).

#### Hàm `EvaluatePosition(GameState& game, int row, int col)`
*   **Mục đích:** Tính tổng điểm tấn công và phòng thủ nếu đặt quân cờ vào ô `(row, col)`.
*   **Quy trình:**
    *   Duyệt qua 4 hướng `directions`.
    *   **Tấn công (ATTACK):** Gọi `GetLineStatus` với `player = 2` (Quân của Bot). Lấy số quân `attackCount` và đầu chặn `attackBlocks`. Giới hạn `attackCount <= 5` để tránh tràn mảng. Cộng điểm từ `AttackScore`.
    *   **Phòng thủ (DEFENSE):** Gọi `GetLineStatus` với `player = 1` (Quân người chơi). Mô phỏng nếu để người chơi đánh vào đó thì họ có chuỗi bao nhiêu. Cộng điểm từ `DefenseScore`.
    *   Trả về tổng của điểm Tấn công + Phòng thủ.

#### Hàm `BotMove(GameState& game)`
*   **Mục đích:** Tìm và thực hiện nước đi tối ưu nhất cho máy.
*   **Thuật toán:**
    *   Khởi tạo `bestScore = -1`. Tạo mảng `bestMoves` để lưu danh sách các tọa độ có cùng số điểm cao nhất.
    *   Duyệt 225 ô. Ô nào trống (`c == 0`) thì gọi `EvaluatePosition`.
    *   Nếu điểm > `bestScore`, làm rỗng mảng `bestMoves` và thêm tọa độ này vào đầu mảng. Cập nhật `bestScore`.
    *   Nếu điểm == `bestScore`, thêm tiếp tọa độ vào mảng `bestMoves` (giúp đa dạng hóa các nước đánh có độ tốt ngang nhau).
    *   Cuối cùng, dùng `rand() % bestMoveCount` để chọn ngẫu nhiên 1 trong các nước đi tốt nhất.
    *   Gọi `MakeMove` để đánh cờ.

### 8. File I/O (Lưu & Tải Game)
Gồm các hàm `SaveGameSlot`, `LoadGameSlot`, `PeekGameSlot`, `DeleteGameSlot`.
*   Dùng hàm `fopen` với các chế độ đọc/ghi nhị phân: `"wb"` (write binary) hoặc `"rb"` (read binary).
*   Ghi toàn bộ trạng thái ván cờ vào file bằng lệnh `fwrite(&game, sizeof(GameState), 1, file)` và đọc lại bằng `fread`.
*   Tên file được định dạng theo slot: `save_0.dat` đến `save_3.dat`.
*   `PeekGameSlot` là hàm chuyên dụng để mở file lấy thông tin hiển thị lên menu UI (như `saveName`, `roundCount`) mà không lưu đè vào ván game hiện tại.
