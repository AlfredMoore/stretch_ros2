import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    # ====================================================================
    # === IMPORTANT: SET THE PATH TO YOUR EXTERNAL EXECUTABLE HERE ===
    # ====================================================================
    # Replace this with the actual, absolute path to your application
    executable_path = '/path/to/your/external_executable'
    # For example: '/usr/local/bin/my_network_app'
    # ====================================================================

    # Declare the launch arguments
    declare_serial_arg = DeclareLaunchArgument(
        'serial',
        description='Serial number for the RealSense camera 12 digits.'
    )

    # declare_address_arg = DeclareLaunchArgument(
    #     'address',
    #     default_value='tcp://*:5555',
    #     description='Address for the ZMQ publisher binding. E.g., tcp://*:5555'
    # )

    serial = LaunchConfiguration('serial')
    # address = LaunchConfiguration('address')
    
    run_external_app = ExecuteProcess(
        cmd=[
            executable_path,
            '--serial', serial,
            # '--address', address,
        ],
        output='screen'
    )

    return LaunchDescription([
        declare_serial_arg,
        # declare_address_arg,
        run_external_app,
    ])
