# CÁC KỸ THUẬT NỔI BẬT DÙNG ĐỂ THUYẾT TRÌNH (KÈM MÃ GIẢ)

*Lưu ý quan trọng: Dựa theo yêu cầu của giảng viên, tuyệt đối KHÔNG chụp ảnh code dán vào slide. Hãy copy các đoạn Mã Giả (Pseudocode) dưới đây để đưa vào slide báo cáo.*

---

## 🌟 1. Trí tuệ nhân tạo (AI Bot) bằng thuật toán Heuristic Evaluation

**Mô tả:** Thay vì cho Bot đánh ngẫu nhiên, hệ thống AI có tư duy đánh giá từng ô cờ thông qua ma trận điểm (Ma trận Heuristic). Thuật toán áp dụng mảng 2 chiều `[Số đầu bị chặn][Số quân]` cực kỳ thông minh. Điểm tấn công luôn được cấu hình cao hơn phòng thủ để tạo ra Bot có lối chơi áp đảo.

**Mã giả (Pseudocode):**
```text
Hàm BotMove():
  Khởi tạo Điểm_Cao_Nhất = -1
  Tạo Danh_Sách_Nước_Đi_Tốt_Nhất (rỗng)

  Duyệt qua tất cả các ô trên Bàn Cờ:
     Nếu ô đang Trống:
        Điểm = Tính_Điểm_Tấn_Công() + Tính_Điểm_Phòng_Thủ()
        
        Nếu Điểm > Điểm_Cao_Nhất:
           Cập nhật Điểm_Cao_Nhất = Điểm
           Xóa Danh_Sách cũ, thêm ô này vào Danh_Sách mới
        Ngược lại Nếu Điểm == Điểm_Cao_Nhất:
           Thêm ô này vào Danh_Sách
           
  Lựa chọn Ngẫu nhiên 1 ô trong Danh_Sách_Nước_Đi_Tốt_Nhất
  Đánh quân cờ vào ô đó.
```

---

## 🌟 2. Kỹ thuật Render Độc lập Độ phân giải (Resolution-Independent) và Letterboxing

**Mô tả:** Đa số sinh viên khi làm game C++ thường cố định cứng tọa độ (ví dụ `x = 500, y = 300`). Tuy nhiên, đồ án này dùng đối tượng `RenderTexture2D` để vẽ mọi thứ vào một "tấm bạt" ảo kích thước chuẩn 1920x1080. Sau đó tự động tính toán tỷ lệ Scale để thu phóng vừa vặn với bất kỳ màn hình nào. Nếu màn hình khác tỷ lệ 16:9, thuật toán tự động tạo viền đen (Letterboxing) để hình ảnh không bị méo.

**Mã giả (Pseudocode):**
```text
Khởi tạo Bạt_Ảo (RenderTexture2D) kích thước 1920x1080

Vòng lặp Game:
  Tính toán Tỷ_lệ_Scale (Dựa trên chiều rộng/cao màn hình thực)
  Cập nhật lại tọa độ Chuột (Chia cho Tỷ_lệ_Scale) để click chính xác
  
  // Giai đoạn 1: Vẽ đồ họa
  Bắt đầu vẽ vào Bạt_Ảo:
     Vẽ Bàn Cờ, Vẽ Nhân vật, Vẽ Giao diện...
  Kết thúc vẽ Bạt_Ảo
  
  // Giai đoạn 2: Xuất ra màn hình
  Bắt đầu vẽ ra Màn Hình Thực:
     Xóa phông nền màu Đen
     In Bạt_Ảo ra màn hình với kích thước đã nhân Tỷ_lệ_Scale
  Kết thúc vẽ.
```

---

## 🌟 3. Tối ưu hóa kiểm tra Thắng/Thua bằng "Mảng Hướng" (Direction Vector)

**Mô tả:** Tránh việc phải viết 4 khối code dài ngoằng lặp đi lặp lại để duyệt ngang, dọc, chéo. Kỹ thuật này sử dụng một mảng 2 chiều chứa 4 Vector Hướng. Chỉ cần dùng 1 vòng lặp duy nhất để duyệt qua 4 Vector này, giúp tối ưu hóa hàng trăm dòng code, đạt chuẩn "Clean Code" và dễ dàng mở rộng luật chặn 2 đầu.

**Mã giả (Pseudocode):**
```text
Hàm Kiểm_Tra_Thắng(ô_vừa_đánh):
  Khai báo Mảng_Hướng = { (0,1), (1,0), (1,1), (1,-1) }
  
  Duyệt từng Hướng trong Mảng_Hướng:
     Biến Đếm_Quân = 1
     Biến Đầu_Chặn = 0
     
     Duyệt về phía Trước theo Hướng hiện tại: 
        Nếu gặp quân mình -> Tăng Đếm_Quân
        Nếu đụng tường hoặc đụng địch -> Tăng Đầu_Chặn
        
     Duyệt về phía Sau theo Hướng hiện tại:
        (Tương tự duyệt về phía trước)
        
     Nếu Đếm_Quân >= 5 VÀ Đầu_Chặn < 2:
        TRẢ VỀ Người Chơi Thắng
        
  TRẢ VỀ Chưa ai thắng
```

---

## 🌟 4. Hệ thống Hoạt Ảnh (Animation) bằng Sprite Sheet và Frame Timer

**Mô tả:** Lập trình chuyển động nhân vật 2D không chỉ đơn giản là thay đổi ảnh liên tục. Game áp dụng kỹ thuật tính toán sự chênh lệch thời gian giữa các khung hình (Delta Time) tích lũy vào bộ đếm (Timer). Khi bộ đếm đạt ngưỡng, khung hình trên Sprite Sheet mới dịch chuyển. Đảm bảo nhân vật luôn chuyển động mượt mà, đồng đều ở mọi tốc độ cấu hình máy tính (60 FPS hay 144 FPS đều không bị chạy quá nhanh).

**Mã giả (Pseudocode):**
```text
Hàm Cập_Nhật_Animation_Nhân_Vật(Nhân_Vật, Thời_Gian_Khung_Hình_Delta):
  Nhân_Vật.Bộ_Đếm_Thời_Gian += Thời_Gian_Khung_Hình_Delta
  
  Nếu Nhân_Vật.Bộ_Đếm_Thời_Gian >= Thời_Gian_Chuyển_Frame_Quy_Định:
      Nhân_Vật.Khung_Hình_Hiện_Tại++
      Nhân_Vật.Bộ_Đếm_Thời_Gian = 0
      
      Nếu Nhân_Vật.Khung_Hình_Hiện_Tại > Số_Frame_Tối_Đa:
         Reset Khung_Hình_Hiện_Tại = 0
         Cập nhật trạng thái thành Đứng_Yên (IDLE)
         
  Lấy tọa độ cắt ảnh dựa trên Khung_Hình_Hiện_Tại
  Vẽ ảnh Nhân_Vật bị cắt ra màn hình
```
