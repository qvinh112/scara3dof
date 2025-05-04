import tkinter as tk
from tkinter import messagebox
from math import cos, sin, radians, degrees, sqrt, atan2
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# Thông số robot
L1 = 17
L2 = 13
reach_max = L1 + L2
reach_min = abs(L1 - L2)

class ScaraGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("SCARA Robot 2DOF")
        self.root.geometry("850x550")

        # Figure matplotlib
        self.fig, self.ax = plt.subplots()
        self.ax.set_xlim([-35, 35])
        self.ax.set_ylim([-35, 35])
        self.ax.set_aspect('equal')
        self.ax.grid(True)
        self.ax.set_xlabel('X (cm)')
        self.ax.set_ylabel('Y (cm)')
        self.ax.set_title('SCARA 2DOF')

        canvas = FigureCanvasTkAgg(self.fig, master=root)
        canvas.get_tk_widget().place(x=350, y=30, width=480, height=450)
        self.canvas = canvas

        # Slider Theta1
        tk.Label(root, text="Góc Theta 1 (°)").place(x=20, y=470)
        self.s1 = tk.Scale(root, from_=0, to=180, orient=tk.HORIZONTAL, command=self.update_plot)
        self.s1.set(90)
        self.s1.place(x=20, y=490, width=250)

        # Slider Theta2
        tk.Label(root, text="Góc Theta 2 (°)").place(x=20, y=430)
        self.s2 = tk.Scale(root, from_=-156, to=156, orient=tk.HORIZONTAL, command=self.update_plot)
        self.s2.set(0)
        self.s2.place(x=20, y=450, width=250)

        # Tọa độ đầu cuối
        self.coord_label = tk.Label(root, text="", justify=tk.LEFT)
        self.coord_label.place(x=20, y=350)

        # Nhập X, Y
        tk.Label(root, text="X:").place(x=20, y=310)
        self.x_entry = tk.Entry(root)
        self.x_entry.insert(0, "0")
        self.x_entry.place(x=50, y=310, width=60)

        tk.Label(root, text="Y:").place(x=120, y=310)
        self.y_entry = tk.Entry(root)
        self.y_entry.insert(0, "0")
        self.y_entry.place(x=150, y=310, width=60)

        # Nút tính IK
        tk.Button(root, text="Tính góc", command=self.inverse_kinematics).place(x=100, y=270)

        # Vẽ ban đầu
        self.update_plot()

    def update_plot(self, event=None):
        theta1 = radians(self.s1.get())
        theta2 = radians(self.s2.get())

        x1 = L1 * cos(theta1)
        y1 = L1 * sin(theta1)
        x2 = x1 + L2 * cos(theta1 + theta2)
        y2 = y1 + L2 * sin(theta1 + theta2)

        d = sqrt(x2**2 + y2**2)
        in_range = reach_min <= d <= reach_max

        self.ax.cla()
        self.ax.set_xlim([-35, 35])
        self.ax.set_ylim([-35, 35])
        self.ax.set_aspect('equal')
        self.ax.grid(True)
        self.ax.set_xlabel('X (cm)')
        self.ax.set_ylabel('Y (cm)')

        # Vẽ cánh tay robot
        self.ax.plot([0, x1], [0, y1], 'r', linewidth=3)
        self.ax.plot([x1, x2], [y1, y2], 'g', linewidth=3)
        self.ax.plot(0, 0, 'bo')
        self.ax.plot(x2, y2, 'ko' if in_range else 'ro')

        msg = f"Tọa độ đầu cuối:\nX = {x2:.2f} cm\nY = {y2:.2f} cm"
        msg += "\n✓ Trong vùng làm việc" if in_range else "\n✗ Ngoài vùng làm việc"
        self.coord_label.config(text=msg)

        self.ax.set_title(f"θ₁ = {degrees(theta1):.1f}°, θ₂ = {degrees(theta2):.1f}°")
        self.canvas.draw()

    def inverse_kinematics(self):
        try:
            x = float(self.x_entry.get())
            y = float(self.y_entry.get())
        except ValueError:
            messagebox.showerror("Lỗi", "Vui lòng nhập số hợp lệ cho X và Y.")
            return

        d = sqrt(x**2 + y**2)
        if d < reach_min or d > reach_max:
            messagebox.showwarning("Lỗi", "Điểm nằm ngoài vùng hoạt động!")
            return

        cos_theta2 = (x**2 + y**2 - L1**2 - L2**2) / (2 * L1 * L2)
        sin_theta2 = sqrt(1 - cos_theta2**2)
        theta2 = atan2(sin_theta2, cos_theta2)

        k1 = L1 + L2 * cos(theta2)
        k2 = L2 * sin(theta2)
        theta1 = atan2(y, x) - atan2(k2, k1)

        self.s1.set(degrees(theta1))
        self.s2.set(degrees(theta2))
        self.update_plot()

if __name__ == "__main__":
    root = tk.Tk()
    app = ScaraGUI(root)
    root.mainloop()
