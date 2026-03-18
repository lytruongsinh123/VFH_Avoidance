import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32MultiArray
import matplotlib.pyplot as plt
import numpy as np


class PlotNode(Node):

    def __init__(self):

        super().__init__('plot_hist')

        self.subscription = self.create_subscription(
            Float32MultiArray,
            '/polot_histogram',
            self.callback,
            10)

        # ===== CẤU HÌNH VFH =====
        self.ALPHA = 10.0
        self.ANGLE_MIN = -135.0

        # ===== PLOT =====
        plt.ion()
        self.fig, self.ax = plt.subplots(figsize=(10, 5))

    def callback(self, msg):

        self.ax.clear()

        data = np.array(msg.data)

        # ===== TÁCH DATA =====
        T_low = data[-2]
        T_high = data[-1]
        values = data[:-2]

        SECTOR = len(values)

        # ===== GÓC TÂM SECTOR =====
        angles = np.array([
            self.ANGLE_MIN + (k + 0.5) * self.ALPHA
            for k in range(SECTOR)
        ])

        # ===== VẼ HISTOGRAM =====
        self.ax.bar(
            angles,
            values,
            width=self.ALPHA*0.8,
            align='center'
        )

        # ===== THRESHOLD =====
        self.ax.axhline(T_low, linestyle='--', label='T_low')
        self.ax.axhline(T_high, linestyle='--', label='T_high')

        # ===== LABEL =====
        self.ax.set_xlabel("Angle (deg)")
        self.ax.set_ylabel("H(k)")
        self.ax.set_title("Polar Histogram (center-based)")

        # ===== TRỤC =====
        self.ax.set_xlim(-135, 135)

        # ✅ HIỂN THỊ ĐÚNG TÂM (QUAN TRỌNG)
        self.ax.set_xticks(angles)
        self.ax.set_xticklabels([f"{int(a)}" for a in angles], rotation=90)

        self.ax.grid(True)
        self.ax.legend()

        plt.pause(0.01)


def main():

    rclpy.init()

    node = PlotNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()