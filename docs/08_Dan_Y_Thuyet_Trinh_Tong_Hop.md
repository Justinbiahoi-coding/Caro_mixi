# KỊCH BẢN THUYẾT TRÌNH THEO ĐÚNG 22 SLIDE CANVA (1 NGƯỜI NÓI)

*Kịch bản này bám sát ĐÚNG 100% thứ tự 22 trang slide hiện tại của bạn và được thiết kế liền mạch dành cho 1 người thuyết trình từ đầu đến cuối.*

---

**Slide 1: Tiêu đề**
- "Dạ em chào Thầy và các bạn. Hôm nay em xin đại diện nhóm 15 trình bày về Đồ án Cơ sở lập trình. Đề tài của nhóm là tựa game Caro Battle do 2 thành viên Bùi Văn Thiên và Đỗ Phước Thiện thực hiện."

**Slide 2: Report Highlights**
- "Bài báo cáo của nhóm gồm 7 phần chính, đi từ Giới thiệu, Kiến trúc, Thiết kế dữ liệu, Chức năng hệ thống, Thuật toán Logic, Tổng kết và cuối cùng là phần Demo Giao diện."

**Slide 3: Giới thiệu**
- "Đầu tiên về phần Giới thiệu: Game Caro Battle được xây dựng bằng C++ và thư viện đồ họa Raylib. Không chỉ dừng lại ở cờ Caro truyền thống, đồ án đã mở rộng thêm chức năng chọn Tướng, đánh với AI Bot và hệ thống Lưu/Tải game, nhằm mang lại một giao diện trực quan và trải nghiệm tốt nhất."

**Slide 4: Kiến trúc hệ thống (Phần 1)**
- "Để code không bị rối và dễ bảo trì, nhóm em lựa chọn mô hình phân lớp. Tụi em tách biệt hoàn toàn phần vẽ giao diện đồ họa, phần xử lý nghiệp vụ (luật chơi) và phần quản lý dữ liệu ra riêng biệt."

**Slide 5: Kiến trúc hệ thống (Phần 2 - 3 Lớp)**
- "Cụ thể kiến trúc gồm 3 khối: Lớp Giao diện chuyên xử lý hình ảnh, font chữ. Lớp Điều khiển (Logic Control) là bộ não xử lý thắng thua. Và Lớp Quản trị Xử lý (GameState) là nơi chứa toàn bộ dữ liệu. Mọi thao tác từ giao diện đều phải đi qua Điều khiển trước rồi mới cập nhật vào Dữ liệu."

**Slide 6: Sơ đồ cấu trúc tổng thể**
- "Sơ đồ luồng đi của hệ thống hoạt động theo 1 chiều: Giao diện UI nhận thao tác của người chơi -> Chuyển xuống Logic Control xử lý -> Lưu kết quả vào Game State."

**Slide 7: Thiết kế dữ liệu**
- "Đi sâu vào Dữ liệu: Nhóm thiết kế mô hình tập trung với struct `GameState`. Bất kỳ module nào từ giao diện đến AI Bot hay lưu file đều truy cập chung vào `GameState`. Điều này giúp dữ liệu luôn đồng nhất."

**Slide 8: Chi tiết Dữ liệu (Bàn cờ, Ván đấu, Bot, Lưu trữ)**
- "Ví dụ, Bàn cờ là một mảng 2 chiều 15x15. Mỗi phần tử lưu một trạng thái ô cờ riêng. Trạng thái ván cờ như lượt đi hiện tại hay kết quả trận đấu cũng nằm gọn trong `GameState`, giúp Bot AI hay hàm Save Game lấy dữ liệu cực kỳ dễ dàng."

**Slide 9: Tổng quan chức năng**
- "Về mặt chức năng, ngoài Caro truyền thống, điểm nhấn của game là tích hợp AI Bot, cơ chế Lưu trữ dữ liệu nhị phân và giao diện đồ họa bắt mắt."

**Slide 10: Quản lý trò chơi**
- "Phần Quản lý vòng đời game sử dụng hàm `InitGame` để tạo ván mới, `ResetRound` để xóa bàn cờ đánh lại. Trạng thái trận đấu được kiểm soát chặt chẽ: 0 là đang chơi, 1 hoặc 2 là có người thắng, 3 là hòa."

**Slide 11: Chức năng Caro**
- "Bước sang luồng chức năng thực tế: Việc đánh cờ được thực hiện qua hàm `MakeMove`. Cứ sau mỗi nước đi hợp lệ, hệ thống sẽ đổi lượt người chơi và tăng biến đếm số nước đi."

**Slide 12: Chế độ đánh Bot**
- "Đặc biệt nhất là chế độ AI Bot. Khi đến lượt máy, hàm `BotMove` kích hoạt. Bot sẽ gọi hàm `EvaluatePosition` và `GetLineStatus` để tính điểm. Dựa vào 2 mảng Điểm Tấn Công và Điểm Phòng Thủ, Bot sẽ đánh giá tiềm năng tạo thành chuỗi 5 quân để tìm ra vị trí đánh tối ưu nhất."

**Slide 13: Lưu và tải dữ liệu**
- "Tính năng Save/Load sử dụng hàm `fopen` của C. Nhóm dùng chế độ đọc ghi Nhị phân (Binary) toàn bộ khối dữ liệu `GameState` xuống ổ cứng bằng hàm `fread` và `fwrite`, giúp lưu game nhanh và an toàn."

**Slide 14: Thuật toán xử lý và Logic Game (Mở bài phần 5)**
- "Tiếp theo là phần cốt lõi nhất của đồ án: Các thuật toán xử lý Logic."

**Slide 15: Hàm MakeMove**
- *(Gợi ý: Nhớ thay ảnh code bằng Mã Giả ở file 07)*. "Như trên slide là mã giả của hàm MakeMove. Hàm sẽ kiểm tra xem tọa độ chuột có hợp lệ không. Nếu ô trống, quân cờ được đặt xuống, và ngay lập tức gọi hàm CheckWin để cập nhật trạng thái kết quả trận đấu."

**Slide 16: Hàm Check Win**
- *(Gợi ý: Nhớ thay ảnh code bằng Mã Giả ở file 07)*. "Để Kiểm tra thắng thua hiệu quả, thay vì viết 4 vòng lặp độc lập rất dài, nhóm dùng một Mảng Vector chứa 4 hướng. Duyệt qua mảng này để đếm số quân liên tiếp và số đầu chặn. Luật thắng chuẩn được áp dụng là: Đạt đủ 5 quân và bị chặn ít hơn 2 đầu."

**Slide 17: Hàm Bot Move**
- "Luồng hoạt động của Bot rất rõ ràng: Khi đến lượt -> Gọi `BotMove` phân tích bàn cờ -> Quét tìm ô trống có điểm cao nhất -> Đặt quân và kiểm tra thắng thua."

**Slide 18: Hàm Evaluate Position**
- *(Gợi ý: Nhớ thay ảnh code bằng Mã Giả ở file 07)*. "Đây là hàm tính điểm cho Bot (Heuristic Evaluation). Với mỗi ô trống, Bot dùng ma trận điểm để chấm cho cả thế công lẫn thế thủ. Nhóm cố tình cấu hình điểm Tấn Công cao hơn Phòng Thủ một chút để Bot có lối chơi chủ động áp đảo người chơi thay vì chỉ biết phòng ngự."

**Slide 19: Hàm GetLineStatus**
- "Hàm `GetLineStatus` là hàm phụ trợ, có nhiệm vụ duyệt theo một hướng nhất định để đếm số quân cờ liên tiếp, và đếm xem 2 đầu có bị chặn bởi tường hay quân địch không, làm cơ sở dữ liệu cho Bot chấm điểm."

**Slide 20: Tổng kết**
- "Tổng kết lại, đồ án đã hoàn thành tốt các mục tiêu đề ra về thuật toán cờ Caro, xử lý đồ họa mượt mà và phát triển được AI Bot. Trong tương lai, nhóm mong muốn nâng cấp AI thông minh hơn bằng thuật toán đệ quy và có thể thêm chế độ chơi online."

**Slide 21: Giao diện và Demo**
- "Trăm nghe không bằng một thấy, sau đây em xin phép được Demo trực tiếp game để Thầy và các bạn cùng trải nghiệm giao diện cũng như thử sức với con Bot của nhóm ạ." 
*(Mở game lên và thao tác chơi)*

**Slide 22: Thank You**
- "Dạ phần trình bày và Demo của nhóm 15 đến đây là kết thúc, em xin cảm ơn Thầy và các bạn đã chú ý lắng nghe ạ."
