# Smart Home - Hệ thống nhà thông minh sử dụng ESP32

Đồ án PBL3, Khoa Điện tử - Viễn thông, Trường Đại học Bách khoa, Đại học Đà Nẵng.
Lớp học phần 22N40A, Nhóm 8.

## Giới thiệu

Smart Home là hệ thống tự động giám sát và điều khiển thiết bị điện trong gia đình, xây dựng trên vi điều khiển ESP32 kết hợp các cảm biến tích hợp. Hệ thống thu thập dữ liệu môi trường, phát hiện xâm nhập, đo điện năng tiêu thụ và điều khiển thiết bị chấp hành, đồng thời cho phép giám sát và điều khiển từ xa qua giao thức MQTT và giao diện web. Sản phẩm được trình diễn trên mô hình ngôi nhà thu nhỏ.

Mục tiêu của đề tài:

- Nâng cao sự tiện nghi trong sinh hoạt.
- Tiết kiệm năng lượng thông qua theo dõi điện năng tiêu thụ.
- Đảm bảo an toàn với chức năng cảnh báo chống trộm.

## Tính năng

| Tính năng | Mô tả |
|---|---|
| Giám sát môi trường | Đo nhiệt độ và độ ẩm bằng cảm biến DHT22, hiển thị thời gian thực trên màn hình TFT và giao diện web |
| Đo điện năng | Module PZEM-004T đo điện áp, dòng điện, công suất, tần số, hệ số công suất và điện năng tiêu thụ |
| Chống trộm | Cảm biến PIR phát hiện chuyển động. Khi có người, LED đỏ sáng và còi báo động. Khi không có người, LED xanh sáng |
| Phơi đồ tự động | Cảm biến mưa phát hiện mưa và điều khiển servo thu giàn phơi đồ. Khi hết mưa, giàn phơi được mở ra |
| Hiển thị cục bộ | Màn hình TFT hiển thị nhiệt độ, độ ẩm, công suất và trạng thái thời tiết |
| Điều khiển từ xa | Giám sát và điều khiển qua giao diện web, truyền dữ liệu bằng giao thức MQTT qua Wi-Fi |
| Điều khiển hồng ngoại | Điều khiển cục bộ bằng remote hồng ngoại thông qua IR Receiver |

## Kiến trúc hệ thống

```text
 ┌──────────────────────┐         ┌───────────────────┐         ┌─────────────────────────┐
 │ Khối thu thập dữ liệu│         │   Khối điều khiển │         │ Khối hiển thị/công suất │
 │  - DHT22             │ ──────> │    trung tâm      │ ──────> │ - LED trạng thái        │
 │  - PIR               │         │     ESP32         │         │ - Màn hình TFT          │
 │  - Cảm biến mưa      │         │                   │         │ - Còi (buzzer)          │
 │  - PZEM-004T         │         └─────────┬─────────┘         │ - Servo                 │
 │  - IR Receiver       │                   │                   └─────────────────────────┘
 └──────────────────────┘            Wi-Fi / MQTT
                                            │
                                    ┌───────▼─────────┐
                                    │ Cloud (Node-RED,│
                                    │  MQTT Broker)   │
                                    │ ⇄ Web Dashboard│
                                    └─────────────────┘

### Nguyên lý hoạt động

1. Hệ thống khởi tạo ngoại vi, kết nối Wi-Fi và MQTT.
2. ESP32 đọc dữ liệu từ các cảm biến, hiển thị lên màn hình TFT và gửi lên MQTT với chu kỳ 5 giây.
3. Khi cảm biến mưa phát hiện mưa, servo thu giàn phơi đồ. Khi không mưa, servo mở giàn phơi.
4. Khi cảm biến PIR phát hiện chuyển động, hệ thống bật LED đỏ, kích hoạt còi và gửi thông báo qua MQTT.
5. Người dùng có thể đóng/mở servo từ giao diện web (qua MQTT) hoặc bằng remote hồng ngoại.

```
## Phần cứng

| Linh kiện | Số lượng | Vai trò |
| :--- | :---: | :--- |
| **ESP32 DevKit V1 (38 chân)** | 1 | Vi điều khiển trung tâm, kết nối Wi-Fi |
| **Cảm biến nhiệt độ, độ ẩm DHT22** | 1 | Đo nhiệt độ và độ ẩm |
| **Cảm biến mưa** | 1 | Phát hiện mưa |
| **Cảm biến chuyển động PIR** | 1 | Phát hiện người |
| **Module đo điện năng PZEM-004T** | 1 | Đo thông số điện AC, giao tiếp UART |
| **Servo** | 1 | Đóng/mở giàn phơi đồ |
| **Còi** | 1 | Cảnh báo xâm nhập |
| **Màn hình TFT** | 1 | Hiển thị thông số hệ thống |
| **LED (đỏ, xanh)** | 2 | Báo trạng thái |
| **IR Receiver và remote** | 1 | Điều khiển hồng ngoại |

Mạch in (PCB) được thiết kế trên Altium Designer. Sơ đồ nguyên lý và bản vẽ mạch in được trình bày trong báo cáo đề tài.

## Phần mềm và công cụ

| Hạng mục | Công nghệ |
| :--- | :--- |
| **Ngôn ngữ firmware** | C/C++ |
| **Môi trường phát triển** | PlatformIO IDE |
| **Giao thức truyền thông** | MQTT |
| **Backend / Cloud** | MQTT Broker (Mosquitto hoặc HiveMQ), Node-RED, Firebase |
| **Giao diện người dùng** | Web (HTML) |
| **Thiết kế và mô phỏng mạch** | Altium Designer, Proteus |

Thư viện sử dụng: `DHT`, `PZEM004T`, `Adafruit_GFX`, `SPI`, `WiFi`, `PubSubClient`.
## Hướng dẫn cài đặt và sử dụng

### Yêu cầu

- Visual Studio Code kèm tiện ích PlatformIO.
- Bo mạch ESP32 DevKit V1 và các cảm biến như mục Phần cứng.
- MQTT Broker hoạt động và có thể truy cập từ mạng Wi-Fi của ESP32.

### Các bước thực hiện

1. Clone mã nguồn:

```bash
git clone <repository-url>
cd <repository-folder>
```

2. Mở thư mục dự án bằng PlatformIO và chờ công cụ tự cài đặt thư viện.
3. Cấu hình thông tin Wi-Fi (SSID, mật khẩu) và MQTT (địa chỉ broker, cổng, topic) trong mã nguồn.
4. Đấu nối phần cứng theo sơ đồ nguyên lý hoặc sử dụng mạch in đã thiết kế.
5. Nạp chương trình cho ESP32 qua cổng USB và theo dõi log bằng Serial Monitor.
6. Mở giao diện web và kết nối tới MQTT Broker hoặc Firebase đã cấu hình để giám sát và điều khiển.

Lưu ý an toàn: PZEM-004T làm việc với điện lưới AC. Cần đấu nối đúng kỹ thuật và đảm bảo cách điện trước khi cấp nguồn.

## Giao diện web
<img width="1238" height="903" alt="image" src="https://github.com/user-attachments/assets/1b544f4d-9deb-414b-8644-e6cb481bee98" />
<img width="1221" height="887" alt="image" src="https://github.com/user-attachments/assets/aaad4757-ca90-46bc-a457-151e5d905b44" />


Giao diện SmartHome gồm các phân hệ:

- Tổng quan: trạng thái thời tiết, công suất, các công tắc điều khiển (điện gia đình, PIR, chế độ mưa tự động, phơi đồ).
- Thông số và tiêu thụ điện: biểu đồ điện năng theo tháng, điện áp, dòng điện, tần số và hệ số công suất.
- Nhiệt độ và độ ẩm: biểu đồ theo thời gian thực từ cảm biến DHT22.
- Nhật ký hệ thống: ghi nhận bật/tắt thiết bị, phát hiện PIR, cảnh báo mưa và điện áp bất thường.

## Kết quả thực nghiệm
<img width="1280" height="960" alt="image" src="https://github.com/user-attachments/assets/e616985e-d67e-440c-9d08-3c2dd7a49daf" />

<img width="1280" height="960" alt="image" src="https://github.com/user-attachments/assets/4e177a5a-3562-4209-aedb-e94b2b1e90c0" />

- Mô hình hoạt động đúng với các chức năng đã đề ra.
- Servo mở giàn phơi khi trời khô và tự động thu lại khi cảm biến phát hiện mưa.
- Khi PIR phát hiện chuyển động, LED đỏ sáng và còi báo động. Khi không phát hiện, LED xanh sáng.
- PZEM-004T đo được công suất khoảng 6,2 W khi bật đèn học. Màn hình TFT hiển thị đúng nhiệt độ, độ ẩm, công suất và trạng thái thời tiết.

## Hạn chế

- Kết cấu cơ khí của mô hình chưa thật chắc chắn, việc đi dây chưa gọn.
- Một số tính năng mở rộng chưa được triển khai trong khuôn khổ đề tài.

## Hướng phát triển


- Tích hợp camera AI để phát hiện sớm nguy cơ cháy, khói và người lạ xâm nhập.
- Xây dựng hệ thống phun sương làm mát tự động theo nhiệt độ.
- Bổ sung điều khiển bằng giọng nói, cảnh báo qua ứng dụng và lưu nhật ký sự kiện.
- Mở rộng thiết bị: khóa cửa thông minh, cảm biến rò rỉ nước, rèm tự động.
- Hoàn thiện mô hình theo dạng module, tối ưu bố cục và đi dây.

## Thành viên thực hiện

| Họ và tên | Lớp | Nhiệm vụ |
|---|---|---|
| Huỳnh Văn Khoa | 22DT2 | Viết firmwave, xây dựng web điều khiển, vẽ lưu đồ hệ thống, hỗ trợ làm mô hình |
| Nguyễn Đức Khoa | 22DT2 | Gia công mạch, hỗ trợ lập trình firmwave, thực hiện slide và báo cáo, đo kiểm tra mạch |
| Nguyễn Đức Thịnh | 22DT2 | Thiết kế mạch in, gia công phần cứng, làm mô hình, thực hiện slide và báo cáo |
| Phạm Hùng Cường | 21DT1 | Vẽ sơ đồ khối và mạch phần cứng, xây dựng web điều khiển,hỗ trợ phần mềm, thực hiện slide và báo cáo |

Giảng viên hướng dẫn: TS. Huỳnh Việt Thắng, ThS. Vũ Vân Thanh.

## Tài liệu tham khảo

1. Firebase là gì? Tìm hiểu tổng quan và hướng dẫn sử dụng Firebase: https://fptshop.com.vn/tin-tuc/thu-thuat/firebase-la-gi-159218
2. Triển khai Website lên Firebase Hosting: https://www.youtube.com/watch?v=Ez7e99RS_jw
3. Kết nối Node-RED với Realtime Database của Firebase (ESP32): https://www.youtube.com/watch?v=7OK1JC4JPCI
4. Node-RED là gì? Những kiến thức cơ bản về Node-RED: https://dientuviet.com/node-red-la-gi/

Đà Nẵng, tháng 11/2025.
