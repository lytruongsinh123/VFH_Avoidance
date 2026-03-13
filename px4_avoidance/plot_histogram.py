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

        # góc từ -135 → 135 mỗi 10 độ
        self.angles = np.arange(-135, 136, 15)

        plt.ion()
        plt.figure(figsize=(10,5))

    def callback(self, msg):

        plt.clf()

        data = np.array(msg.data)

        # tách threshold
        T_low = data[-2]
        T_high = data[-1]

        # histogram
        values = data[:-2]

        # đảm bảo kích thước khớp
        n = min(len(values), len(self.angles))

        angles = self.angles[:n]
        values = values[:n]

        # vẽ histogram
        plt.bar(angles, values, width=4)

        # vẽ threshold
        plt.axhline(T_low, linestyle='--', label='T_low')
        plt.axhline(T_high, linestyle='--', label='T_high')

        plt.xlabel("Angle (deg)")
        plt.ylabel("H(k)")
        plt.title("Polar Histogram")

        plt.xlim(-135, 135)

        plt.xticks(range(-135, 136, 15), rotation=90)

        plt.grid(True)
        plt.legend()

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