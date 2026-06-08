# Phân tích chi tiết: main.cpp

Tài liệu này giải thích chi tiết file `main.cpp` - điểm bắt đầu (entry point) của toàn bộ chương trình Caro Battle.

## 1. File `main.cpp`

**Mục đích:**
File `main.cpp` là hàm `main()` của C++. Nó có nhiệm vụ khởi tạo cửa sổ game, khởi tạo cấu trúc dữ liệu (`GameState`, `UIState`), quản lý vòng lặp game chính (Game Loop) và dọn dẹp tài nguyên khi thoát game.
Đặc biệt, file này cài đặt kỹ thuật **Resolution-Independent Rendering** (Vẽ không phụ thuộc độ phân giải) bằng cách sử dụng `RenderTexture2D` của raylib, giúp game luôn giữ đúng tỷ lệ khung hình (16:9) dưới dạng Letterboxing (viền đen) bất kể người dùng resize cửa sổ ra sao.

### Phân tích từng đoạn code

#### 1. Khởi tạo cơ bản
```cpp
#include "Library.h"

int main() {
    // Cho phép người dùng thay đổi kích thước cửa sổ (Resizable) và bật VSync để mượt mà (tránh xé hình)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    
    // Tạo cửa sổ với độ phân giải mặc định ban đầu là 1600x900, tiêu đề "Do An Caro"
    InitWindow(1600, 900, "Do An Caro");
    
    // Giới hạn tốc độ khung hình ở 60 FPS (Frames Per Second)
    SetTargetFPS(60);
    
    // Vô hiệu hóa phím ESC mặc định của raylib để tự quản lý việc thoát game (ui.shouldExit)
    SetExitKey(0);
```

#### 2. Khởi tạo Dữ liệu (State)
```cpp
    GameState game;
    InitGame(game, 0);   // Hàm trong LogicControl.cpp: Đặt lại game về trạng thái ban đầu, mode 0 (Classic)
    game.inputType = 0;  // Đặt điều khiển mặc định là chuột

    UIState ui;
    InitGUI(ui);         // Hàm trong GUI.cpp: Load toàn bộ hình ảnh, âm thanh, font chữ vào RAM/VRAM
```

#### 3. Kỹ thuật Render Độc lập Độ phân giải (Letterboxing)
```cpp
    // Tạo một khung vẽ ảo (RenderTexture2D) có độ phân giải cố định (thiết kế gốc) là 1920x1080 (FullHD)
    RenderTexture2D target = LoadRenderTexture(1920, 1080);
    
    // Dùng thuật toán lọc Bilinear để khi phóng to/thu nhỏ hình ảnh từ khung ảo lên cửa sổ thật không bị rỗ (pixelated)
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); 
```
**Giải thích:** Mọi thành phần UI và logic trong dự án (tọa độ nút bấm, vị trí ảnh) đều được hardcode dựa trên không gian màn hình là `1920x1080`. Vẽ lên khung ảo này, sau đó sẽ ép (scale) cho khớp với cửa sổ thực tế của người chơi.

#### 4. Vòng lặp Game (The Game Loop)
```cpp
    // Vòng lặp chạy liên tục cho đến khi biến ui.shouldExit = true (người chơi bấm Exit trong menu)
    // hoặc người dùng ấn nút X đóng cửa sổ (WindowShouldClose)
    while (!ui.shouldExit && !WindowShouldClose()) {
```

#### 5. Tính toán Tỷ lệ (Scaling) cho Letterboxing
```cpp
        // Tính tỷ lệ giữa kích thước thật của cửa sổ và kích thước gốc (1920x1080)
        float scaleW = (float)GetScreenWidth() / 1920.0f;
        float scaleH = (float)GetScreenHeight() / 1080.0f;
        
        // Lấy tỷ lệ nhỏ hơn để đảm bảo toàn bộ khung 1920x1080 vừa vặn bên trong cửa sổ thật mà không bị cắt xén
        float scale = (scaleW < scaleH) ? scaleW : scaleH;
        
        // Căn chỉnh tọa độ chuột: Vì khung hình 1920x1080 bị thu nhỏ/phóng to và đặt ở giữa màn hình (có viền đen)
        // Ta phải dịch (offset) và chia tỷ lệ (scale) tọa độ con chuột của raylib về đúng với không gian ảo 1920x1080
        SetMouseOffset(-(GetScreenWidth() - (1920.0f * scale)) * 0.5f, -(GetScreenHeight() - (1080.0f * scale)) * 0.5f);
        SetMouseScale(1.0f / scale, 1.0f / scale);
```

#### 6. Xử lý Logic (Update)
```cpp
        // Cập nhật mọi trạng thái logic của màn hình hiện tại (nhấn nút, di chuyển, đánh cờ, tính toán hạt)
        UpdateGUI(game, ui);
```

#### 7. Vẽ lên Khung Ảo (Draw)
```cpp
        // Chuyển hướng việc vẽ: Thay vì vẽ thẳng ra màn hình, mọi lệnh Draw... tiếp theo sẽ vẽ vào 'target' (1920x1080)
        BeginTextureMode(target);
            ClearBackground(RAYWHITE); // Xóa nền cũ của khung ảo
            DrawGUI(game, ui);         // Vẽ toàn bộ game (nhân vật, bảng, menu)
        EndTextureMode();              // Kết thúc vẽ vào khung ảo
```

#### 8. Xuất khung ảo ra Màn Hình thật (Screen)
```cpp
        // Bắt đầu vẽ ra màn hình thật của người dùng
        BeginDrawing();
            ClearBackground(BLACK); // Xóa màn hình thật thành màu đen (Đây chính là phần viền đen letterbox)
            
            // Lấy toàn bộ texture của khung ảo. Lưu ý chiều cao bị âm (-target.texture.height)
            // LÝ DO: Trong OpenGL (engine vẽ sau lưng raylib), trục Y của Texture bị lật ngược. Ta lật lại bằng chiều cao âm.
            Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height };
            
            // Tính toán hình chữ nhật đích để đặt khung ảo vào chính giữa cửa sổ thật
            Rectangle destRec = { 
                (GetScreenWidth() - (1920.0f * scale)) * 0.5f,  // Căn giữa theo trục X
                (GetScreenHeight() - (1080.0f * scale)) * 0.5f, // Căn giữa theo trục Y
                1920.0f * scale,                                // Chiều rộng đã scale
                1080.0f * scale                                 // Chiều cao đã scale
            };
            
            // Vẽ khung ảo lên cửa sổ thật
            DrawTexturePro(target.texture, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }
```

#### 9. Dọn dẹp và Thoát
```cpp
    // Dọn dẹp bộ nhớ RAM/VRAM
    UnloadRenderTexture(target);
    UnloadGUI(ui);      // Hàm tự định nghĩa: Xả toàn bộ Textures, âm thanh đã nạp trong InitGUI
    CloseWindow();      // Đóng cửa sổ đồ họa, giải phóng context OpenGL
    return 0;
}
```
