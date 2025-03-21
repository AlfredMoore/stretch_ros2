import zmq
import cv2
import numpy as np
import pickle
import click
import csv

import sys
import termios
import time
import tty
import threading

from stretch_mujoco_test import StretchMujocoSimulator


def getch():
    """
    Get a single character from the terminal
    """
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch


def print_keyboard_options():
    click.secho("\n       Keyboard Controls:", fg="yellow")
    click.secho("=====================================", fg="yellow")
    print("W / A /S / D : Move BASE")
    print("U / J / H / K : Move LIFT & ARM")
    print("N / M : Open & Close GRIPPER")
    print("Q : Stop")
    click.secho("=====================================", fg="yellow")


def keyboard_control(robot_sim):
    while robot_sim.is_running():
        print_keyboard_options()
        key = getch().lower()
        if key == "w" and not robot_sim._base_in_pos_motion:
            robot_sim.move_by("base_translate", 0.07)
        elif key == "s" and not robot_sim._base_in_pos_motion:
            robot_sim.move_by("base_translate", -0.07)
        elif key == "a" and not robot_sim._base_in_pos_motion:
            robot_sim.move_by("base_rotate", 0.15)
        elif key == "d" and not robot_sim._base_in_pos_motion:
            robot_sim.move_by("base_rotate", -0.15)
        elif key == "u":
            robot_sim.move_by("lift", 0.1)
        elif key == "j":
            robot_sim.move_by("lift", -0.1)
        elif key == "h":
            robot_sim.move_by("arm", -0.05)
        elif key == "k":
            robot_sim.move_by("arm", 0.05)
        elif key == "n":
            robot_sim.move_by("gripper", 0.007)
        elif key == "m":
            robot_sim.move_by("gripper", -0.007)
        elif key == "q":
            robot_sim.stop()
        time.sleep(0.1)



@click.command()
@click.option(
    "--scene-xml-path", default="stretch_ros2/stretch_mujoco_ros2/scene/scene.xml", help="Path to the scene xml file"
)
@click.option("--headless", is_flag=True, help="Run the simulation headless")
def main(scene_xml_path: str, headless: bool) -> None:
    robot_sim = StretchMujocoSimulator(scene_xml_path)
    robot_sim.start(headless=headless)

    # Set up ZeroMQ context and socket
    context = zmq.Context()
    socket = context.socket(zmq.PUB)
    socket.bind("tcp://localhost:5555")
    print("Server listening on port 5555...")
    
    keyboard_thread = threading.Thread(target=keyboard_control, args=(robot_sim,))
    keyboard_thread.start()

    try:
        while robot_sim.is_running():
            camera_data = robot_sim.pull_camera_data()
            #print("Capturing images...")

            rgb = camera_data["cam_d435i_rgb"]
            depth = camera_data["cam_d435i_depth"]
            cv2.imshow("test", depth)
            cv2.waitKey(1)
            #print(np.max(depth))

            # Encode RGB as JPEG
            _, rgb_encoded = cv2.imencode(".png", rgb)
            rgb_bytes = rgb_encoded.tobytes()

            # Encode Depth as PNG (lossless)
            depth = depth * 6553.5
            depth = depth.astype(np.uint16)
            _, depth_encoded = cv2.imencode(".png", depth)
            depth_bytes = depth_encoded.tobytes()

            # Send RGB + Depth as multipart
            socket.send_multipart([rgb_bytes, depth_bytes])
            #print("RGB-D Data Sent!")
            # Save depth data to CSV for debugging
            with open('depth_img_data_server.csv', 'w', newline='') as csvfile:
                writer = csv.writer(csvfile)
                for row in depth:
                    writer.writerow(row)

            #data = pickle.dumps(camera_data["cam_d405_rgb"])
            #socket.send(data)

    except KeyboardInterrupt:
        robot_sim.stop()
    finally:
        keyboard_thread.join()
        socket.close()
        context.term()

if __name__ == "__main__":
    import warnings
    warnings.warn("use 'python -m stretch_mujoco', not 'python -m stretch_mujoco.stretch_mujoco'", DeprecationWarning)

    # Check if we are on macOS
    main()