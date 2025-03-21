import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from message_filters import ApproximateTimeSynchronizer, Subscriber, TimeSynchronizer
from cv_bridge import CvBridge
import zmq
import cv2
import numpy as np


class CamSyncZMQNode(Node):
    def __init__(self):
        super().__init__('cam_sync_zmq_node')

        # Create subscribers for RGB and Depth topics
        self.d435i_rgb_sub = Subscriber(self, Image, '/camera/d435i/color')
        self.d435i_depth_sub = Subscriber(self, Image, '/camera/d435i/depth')

        # Synchronize the topics using ApproximateTimeSynchronizer
        self.sync = TimeSynchronizer(
            [self.d435i_rgb_sub, self.d435i_depth_sub],
            queue_size=10  # Allowable time difference in seconds
)
        self.sync.registerCallback(self.sync_callback)

        # Initialize CvBridge for converting ROS Image messages to OpenCV images
        self.bridge = CvBridge()

        self.zmq_context = zmq.Context()
        self.socket = self.zmq_context.socket(zmq.PUB)
        self.socket.bind("tcp://localhost:5555")  # Bind to all network interfaces
        self.get_logger().info("Server listening on port 5555...")
        self.get_logger().info("RGB-Depth Sync Node Initialized")

    def sync_callback(self, rgb_msg, depth_msg):
        """Callback for synchronized RGB and Depth messages."""
        try:
            # RGB Image
            rgb_image = self.bridge.imgmsg_to_cv2(rgb_msg, desired_encoding='bgr8')
            _, rgb_encoded = cv2.imencode(".png", rgb_image)
            rgb_bytes = rgb_encoded.tobytes()

            # Depth Image
            depth_image = self.bridge.imgmsg_to_cv2(depth_msg, desired_encoding='32FC1')
            depth_image = depth_image * 6553.5
            depth_image = depth_image.astype(np.uint16)
            _, depth_encoded = cv2.imencode(".png", depth_image)
            depth_bytes = depth_encoded.tobytes()

            # Display the synchronized images
            cv2.imshow("RGB Image", rgb_image)
            cv2.imshow("Depth Image", depth_image)
            cv2.waitKey(1)

            self.socket.send_multipart([rgb_bytes, depth_bytes])

            self.get_logger().info("Synchronized RGB and Depth images sent over ZMQ")

        except Exception as e:
            self.get_logger().error(f"Error processing images: {e}")

    def destroy_node(self):
            """Clean up resources when the node is destroyed."""
            self.socket.close()
            self.zmq_context.term()
            super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = CamSyncZMQNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()