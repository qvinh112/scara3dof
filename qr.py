import qrcode

# Nội dung của mã QR
data = "QR3"

# Tạo mã QR
qr = qrcode.QRCode(
    version=1,
    error_correction=qrcode.constants.ERROR_CORRECT_L,
    box_size=10,
    border=4,
)

qr.add_data(data)
qr.make(fit=True)

# Tạo ảnh QR
img = qr.make_image(fill_color="black", back_color="white")

# Lưu ảnh
img.save("QR1.png")

print("✅ Mã QR QR1 đã được tạo và lưu thành 'QR1.png'")
