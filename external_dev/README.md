# External Device
Connect Stretch with external devices
## 1. Connect Realsense with ZMQ
realsenseZMQ [repo](https://github.com/AlfredMoore/realsenseZMQ)
```bash
# source your proj
ros2 launch external_dev external_realsense.launch.py serial:=<RealsenseSerial>
```