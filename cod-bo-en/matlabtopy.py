#THƯ VIỆN
import sys
import numpy as np
from PyQt5.QtWidgets import (QApplication, QWidget, QSlider, QLabel, QVBoxLayout, QHBoxLayout, 
                            QPushButton, QLineEdit, QGroupBox, QSplitter, QFrame, QGridLayout,
                            QMessageBox, QSpinBox, QDoubleSpinBox, QComboBox, QRadioButton, QButtonGroup)
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPalette, QColor, QFont, QIcon
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.patches as patches

#Thanh trượt hiện đại
class ModernSlider(QWidget):
    def __init__(self, title, min_val, max_val, initial_val=0, unit="", parent=None):
        super().__init__(parent)
        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(0, 0, 0, 0)
        
        header_layout = QHBoxLayout()
        self.title_label = QLabel(title)
        self.title_label.setFont(QFont("Arial", 10, QFont.Bold))
        self.value_label = QLabel(f"{initial_val} {unit}")
        header_layout.addWidget(self.title_label)
        header_layout.addStretch()
        header_layout.addWidget(self.value_label)
        self.layout.addLayout(header_layout)
        
        self.slider = QSlider(Qt.Horizontal)
        self.slider.setMinimum(min_val)
        self.slider.setMaximum(max_val)
        self.slider.setValue(initial_val)
        self.slider.setStyleSheet("""
            QSlider::groove:horizontal {
                border: 1px solid #999999;
                height: 8px;
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #B1B1B1, stop:1 #c4c4c4);
                margin: 2px 0;
                border-radius: 4px;
            }
            QSlider::handle:horizontal {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4A86E8, stop:1 #5C9AFF);
                border: 1px solid #5c9aff;
                width: 18px;
                margin: -2px 0;
                border-radius: 9px;
            }
        """)
        self.layout.addWidget(self.slider)
        
        self.unit = unit
        self.slider.valueChanged.connect(self._update_value_label)
    
    def _update_value_label(self, value):
        self.value_label.setText(f"{value} {self.unit}")
    
    def value(self):
        return self.slider.value()
    
    def setValue(self, value):
        self.slider.setValue(value)

#Vùng vẽ robot
class SCARARobotCanvas(FigureCanvas):
    def __init__(self, parent=None, width=8, height=8, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi, tight_layout=True)
        self.axes = self.fig.add_subplot(111)
        super().__init__(self.fig)
        self.setParent(parent)
        
        self.fig.patch.set_facecolor('#F0F0F0')
        self.axes.set_facecolor('#FFFFFF')
        self.axes.grid(True, linestyle='--', alpha=0.7)
        self.workspace_max = None
        self.workspace_min = None
        
    def draw_robot(self, joint_coords, reach_min, reach_max, in_range=True):
        self.axes.clear()
        x0, y0 = 0, 0
        x1, y1 = joint_coords[0]
        x2, y2 = joint_coords[1]
        
        self.axes.plot([x0, x1], [y0, y1], 'r-', linewidth=4)
        self.axes.plot([x1, x2], [y1, y2], 'g-', linewidth=4)
        self.axes.plot(x0, y0, 'bo', markersize=12)
        self.axes.plot(x1, y1, 'go', markersize=8)
        self.axes.plot(x2, y2, 'ko' if in_range else 'ro', markersize=10)
        
        if self.workspace_max is None:
            self.workspace_max = patches.Circle((0, 0), reach_max, fill=False, 
                                            edgecolor='blue', linestyle='--', alpha=0.3)
            self.axes.add_patch(self.workspace_max)
        if self.workspace_min is None:
            self.workspace_min = patches.Circle((0, 0), reach_min, fill=False, 
                                            edgecolor='red', linestyle='--', alpha=0.3)
            self.axes.add_patch(self.workspace_min)
        
        self.axes.set_xlim(-40, 40)
        self.axes.set_ylim(-40, 40)
        self.axes.set_aspect('equal')
        self.axes.set_xlabel("X (cm)")
        self.axes.set_ylabel("Y (cm)")
        self.axes.axhline(y=0, color='k', linestyle='-', alpha=0.3)
        self.axes.axvline(x=0, color='k', linestyle='-', alpha=0.3)
        self.draw()

#Giao diện chính
class SCARAGUI(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("SCARA Robot 2DOF - Simulator")
        self.setGeometry(100, 100, 1200, 700)
        self.setStyleSheet("""QWidget {font-family: Arial; font-size: 11pt; background-color: #F0F0F0;}""")
        self.setup_robot_parameters()
        self.init_ui()
        self.update_plot()

    def setup_robot_parameters(self):
        self.L1 = 17
        self.L2 = 13
        self.PPR1 = 64000
        self.PPR2 = 2400
        self.offset1 = 0
        self.offset2 = -122
        self.max2 = 156
        self.range2_deg = self.max2 - self.offset2
        self.max_p2 = self.range2_deg * (self.PPR2 / 360)
        self.reach_max = self.L1 + self.L2
        self.reach_min = abs(self.L1 - self.L2)

    def init_ui(self):
        main_layout = QHBoxLayout(self)
        left_panel = QWidget()
        left_layout = QVBoxLayout(left_panel)
        
        # Sliders
        self.slider1 = ModernSlider("Joint 1 (Base)", 0, int(self.PPR1 * 180 / 360), 0, "pulses")
        self.slider2 = ModernSlider("Joint 2 (Elbow)", 0, int(self.max_p2), 0, "pulses")
        self.slider1.slider.valueChanged.connect(self.update_plot)
        self.slider2.slider.valueChanged.connect(self.update_plot)
        left_layout.addWidget(self.slider1)
        left_layout.addWidget(self.slider2)
        
        # Angle display
        self.angle_display = QLabel("θ1 = 0.0°, θ2 = 0.0°")
        self.angle_display.setAlignment(Qt.AlignCenter)
        left_layout.addWidget(self.angle_display)
        
        # Position display
        self.position_display = QLabel("X = 0.00 cm, Y = 0.00 cm")
        self.position_display.setAlignment(Qt.AlignCenter)
        left_layout.addWidget(self.position_display)
        
        # Position input
        pos_group = QGroupBox("Position Control")
        pos_layout = QGridLayout()
        pos_layout.addWidget(QLabel("X:"), 0, 0)
        self.x_input = QDoubleSpinBox(); self.x_input.setRange(-40, 40); self.x_input.setDecimals(2)
        pos_layout.addWidget(self.x_input, 0, 1)
        pos_layout.addWidget(QLabel("Y:"), 0, 2)
        self.y_input = QDoubleSpinBox(); self.y_input.setRange(-40, 40); self.y_input.setDecimals(2)
        pos_layout.addWidget(self.y_input, 0, 3)
        self.compute_button = QPushButton("Move to Position")
        self.compute_button.clicked.connect(self.inverse_kinematics)
        pos_layout.addWidget(self.compute_button, 1, 0, 1, 4)
        pos_group.setLayout(pos_layout)
        left_layout.addWidget(pos_group)

        # Angle input
        angle_group = QGroupBox("Angle Input")
        angle_layout = QGridLayout()
        angle_layout.addWidget(QLabel("θ1 (°):"), 0, 0)
        self.theta1_input = QDoubleSpinBox(); self.theta1_input.setRange(-180, 180)
        angle_layout.addWidget(self.theta1_input, 0, 1)
        angle_layout.addWidget(QLabel("θ2 (°):"), 0, 2)
        self.theta2_input = QDoubleSpinBox(); self.theta2_input.setRange(-180, 180)
        angle_layout.addWidget(self.theta2_input, 0, 3)
        self.set_angle_button = QPushButton("Set Angles")
        self.set_angle_button.clicked.connect(self.set_joint_angles)
        angle_layout.addWidget(self.set_angle_button, 1, 0, 1, 4)
        angle_group.setLayout(angle_layout)
        left_layout.addWidget(angle_group)
        
        self.reset_button = QPushButton("Reset Position")
        self.reset_button.clicked.connect(self.reset_position)
        left_layout.addWidget(self.reset_button)
        left_layout.addStretch()

        self.robot_canvas = SCARARobotCanvas(self, width=6, height=6)
        main_layout.addWidget(left_panel, 3)
        main_layout.addWidget(self.robot_canvas, 7)

    def update_plot(self):
        p1 = self.slider1.value()
        p2 = self.slider2.value()
        theta1 = np.deg2rad(self.offset1 + p1 * (360 / self.PPR1))
        theta2 = np.deg2rad(self.offset2 + p2 * (360 / self.PPR2))
        x1 = self.L1 * np.cos(theta1)
        y1 = self.L1 * np.sin(theta1)
        x2 = x1 + self.L2 * np.cos(theta1 + theta2)
        y2 = y1 + self.L2 * np.sin(theta1 + theta2)
        d = np.hypot(x2, y2)
        in_range = self.reach_min <= d <= self.reach_max
        self.robot_canvas.draw_robot([(x1, y1), (x2, y2)], self.reach_min, self.reach_max, in_range)
        self.angle_display.setText(f"θ1 = {np.rad2deg(theta1):.1f}°, θ2 = {np.rad2deg(theta2):.1f}°")
        self.position_display.setText(f"X = {x2:.2f} cm, Y = {y2:.2f} cm")
        self.x_input.setValue(round(x2, 2))
        self.y_input.setValue(round(y2, 2))

    def inverse_kinematics(self):
        x = self.x_input.value()
        y = self.y_input.value()
        d = np.hypot(x, y)
        if d < self.reach_min or d > self.reach_max:
            QMessageBox.warning(self, "Out of Reach", "Target out of reach!")
            return
        cos_t2 = (x**2 + y**2 - self.L1**2 - self.L2**2) / (2 * self.L1 * self.L2)
        cos_t2 = np.clip(cos_t2, -1.0, 1.0)
        sin_t2 = np.sqrt(max(0, 1 - cos_t2**2))
        t2 = np.arctan2(sin_t2, cos_t2)
        k1 = self.L1 + self.L2 * cos_t2
        k2 = self.L2 * sin_t2
        t1 = np.arctan2(y, x) - np.arctan2(k2, k1)
        p1 = (np.rad2deg(t1) - self.offset1) * self.PPR1 / 360
        p2 = (np.rad2deg(t2) - self.offset2) * self.PPR2 / 360
        self.slider1.setValue(int(np.clip(p1, 0, self.PPR1 * 180 / 360)))
        self.slider2.setValue(int(np.clip(p2, 0, self.max_p2)))
        self.update_plot()

    def set_joint_angles(self):
        theta1_deg = self.theta1_input.value()
        theta2_deg = self.theta2_input.value()
        p1 = (theta1_deg - self.offset1) * self.PPR1 / 360
        p2 = (theta2_deg - self.offset2) * self.PPR2 / 360
        self.slider1.setValue(int(np.clip(p1, 0, self.PPR1 * 180 / 360)))
        self.slider2.setValue(int(np.clip(p2, 0, self.max_p2)))
        self.update_plot()

    def reset_position(self):
        self.slider1.setValue(0)
        self.slider2.setValue(0)
        self.update_plot()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    gui = SCARAGUI()
    gui.show()
    sys.exit(app.exec_())
