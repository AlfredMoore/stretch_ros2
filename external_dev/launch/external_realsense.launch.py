from launch import LaunchDescription
from launch.actions import ExecuteProcess

def generate_launch_description():
    # ====================================================================
    # === IMPORTANT: SET THE PATH TO YOUR EXTERNAL EXECUTABLE HERE ===
    # ====================================================================
    # Replace this with the actual, absolute path to your application
    executable_path = '/path/to/your/external_executable'
    # For example: '/usr/local/bin/my_network_app'
    # ====================================================================

    return LaunchDescription([
        ExecuteProcess(
            cmd=[executable_path],
            name='external_realsense_ZMQ_pub',
            output='screen'
        )
    ])
