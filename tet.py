import sys   
import cv2
import serial
import serial.tools.list_ports
from pyzbar.pyzbar import decode
from PyQt5.QtWidgets import (
    QApplication, QLabel, QPushButton, QWidget, QVBoxLayout,
    QHBoxLayout, QComboBox, QLineEdit, QGroupBox, QFormLayout
)
from PyQt5.QtCore import QTimer, QDateTime
from PyQt5.QtGui import QImage, QPixmap

class QRScanner(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("SCARA QR Visual Control")
        self.camera = cv2.VideoCapture(0)
        self.serial = None
        self.qr_last_time = {}  # Lưu thời gian quét cuối của từng mã QR
        self.is_moving = False  # Cờ kiểm tra trạng thái di chuyển

        # Giao diện camera và kết quả
        self.video_label = QLabel()
        self.qr_result = QLabel("QR Result: None")
        self.qr_result.setStyleSheet("font-size: 16px; color: green")

        # Nhập mã QR tương ứng với lệnh
        self.qr1_input = QLineEdit("QR1")
        self.qr2_input = QLineEdit("QR2")
        self.qr3_input = QLineEdit("QR3")

        # Nhập mã QR thủ công
        self.manual_qr_input = QLineEdit()
        self.manual_qr_input.setPlaceholderText("Nhập mã QR thủ công để gửi lệnh")
        self.send_manual_btn = QPushButton("Gửi lệnh")
        self.send_manual_btn.clicked.connect(self.send_manual_qr)

        qr_set_group = QGroupBox("Thiết lập mã QR cho các lệnh")
        form_layout = QFormLayout()
        form_layout.addRow("Lệnh CMD1:", self.qr1_input)
        form_layout.addRow("Lệnh CMD2:", self.qr2_input)
        form_layout.addRow("Lệnh CMD3:", self.qr3_input)
        qr_set_group.setLayout(form_layout)

        manual_group = QGroupBox("Gửi lệnh từ mã QR thủ công")
        manual_layout = QHBoxLayout()
        manual_layout.addWidget(self.manual_qr_input)
        manual_layout.addWidget(self.send_manual_btn)
        manual_group.setLayout(manual_layout)

        # Serial port
        self.port_select = QComboBox()
        self.refresh_ports()

        self.connect_btn = QPushButton("Kết nối Serial")
        self.connect_btn.clicked.connect(self.connect_serial)

        serial_layout = QHBoxLayout()
        serial_layout.addWidget(self.port_select)
        serial_layout.addWidget(self.connect_btn)

        layout = QVBoxLayout()
        layout.addWidget(self.video_label)
        layout.addWidget(self.qr_result)
        layout.addWidget(qr_set_group)
        layout.addWidget(manual_group)
        layout.addLayout(serial_layout)
        self.setLayout(layout)

        # Timer camera
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_frame)
        self.timer.start(30)

    def refresh_ports(self):
        ports = serial.tools.list_ports.comports()
        self.port_select.clear()
        for port in ports:
            self.port_select.addItem(port.device)

    def connect_serial(self):
        port = self.port_select.currentText()
        try:
            self.serial = serial.Serial(port, 9600, timeout=1)
            self.connect_btn.setText("Đã kết nối")
            self.connect_btn.setEnabled(False)
            self.qr_result.setText("✅ Serial kết nối thành công.")
        except Exception as e:
            self.qr_result.setText(f"❌ Lỗi Serial: {e}")
            self.qr_result.setStyleSheet("color: red")

    def update_frame(self):
        ret, frame = self.camera.read()
        if not ret:
            return

        # Quét QR bằng pyzbar
        decoded_objects = decode(frame)
        for obj in decoded_objects:
            data = obj.data.decode('utf-8')
            now = QDateTime.currentDateTime()

            # Kiểm tra thời gian quét cuối cho mã QR này
            last_time = self.qr_last_time.get(data, now.addSecs(-10))
            if last_time.msecsTo(now) > 6000:  # Cách nhau ít nhất 6 giây
                self.qr_last_time[data] = now
                self.qr_result.setText(f"Đã quét QR: {data}")
                self.process_qr(data)

        # Hiển thị camera
        rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        h, w, ch = rgb.shape
        img = QImage(rgb.data, w, h, ch * w, QImage.Format_RGB888)
        self.video_label.setPixmap(QPixmap.fromImage(img))

    def process_qr(self, data):
        cmd_sent = ""
        try:
            if self.serial and self.serial.is_open:
                if data == self.qr1_input.text().strip():
                    cmd_sent = "CMD1"
                elif data == self.qr2_input.text().strip():
                    cmd_sent = "CMD2"
                elif data == self.qr3_input.text().strip():
                    cmd_sent = "CMD3"

                if cmd_sent:
                    self.is_moving = True  # Đánh dấu là đang di chuyển
                    self.serial.write((cmd_sent + "\n").encode())
                    self.qr_result.setText(f"✅ Gửi: {cmd_sent} (từ QR: {data})")
                    self.qr_result.setStyleSheet("font-size: 16px; color: green")
                    print(f"Đã gửi: {cmd_sent}")
                else:
                    self.qr_result.setText(f"⚠️ QR [{data}] không khớp với lệnh nào.")
                    self.qr_result.setStyleSheet("font-size: 16px; color: orange")
        except Exception as e:
            self.qr_result.setText(f"❌ Lỗi gửi lệnh: {e}")
            self.qr_result.setStyleSheet("color: red")
        finally:
            if self.is_moving:
                # Sau khi di chuyển xong, reset lại kết quả QR
                QTimer.singleShot(2000, self.reset_qr_result)  # Đặt lại sau 2 giây

    def send_manual_qr(self):
        manual_data = self.manual_qr_input.text().strip()
        if manual_data:
            self.qr_result.setText(f"🔍 Kiểm tra mã QR nhập tay: {manual_data}")
            self.process_qr(manual_data)

    def reset_qr_result(self):
        self.qr_result.setText("QR Result: None")
        self.is_moving = False

    def closeEvent(self, event):
        self.camera.release()
        if self.serial:
            self.serial.close()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = QRScanner()
    win.resize(800, 600)
    win.show()
    sys.exit(app.exec_())
