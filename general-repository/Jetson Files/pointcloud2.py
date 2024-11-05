from board import SCL, SDA
import busio
from adafruit_pca9685 import PCA9685
from time import sleep
from adafruit_motor import servo
import sys
import tfli2c as tfl    #  Import `tfli2c` module v0.0.1
import numpy as np      #  Import NumPy library for arrays
import open3d as o3d   #  Import Open3D library for point cloud visualization
from math import sin, cos, radians

#   - - -  Set I2C Port and Address numbers  - - - - - - - -
I2CAddr1 = 0x10   # Device address in Hex, Decimal 16 for first Luna
I2CAddr2 = 0x11   # Device address in Hex, Decimal 17 for second Luna
I2CPort = 0      # I2C(4), /dev/i2c-4, GPIO 8/9, pins 24/21

#  - - -  Initalize module and devices - - -
i2c = busio.I2C(SCL, SDA)
pca = PCA9685(i2c)
pca.frequency = 43
servo1 = servo.Servo(pca.channels[0])
luna1 = tfl.TFLuna(I2CAddr2, I2CPort)
#luna2 = tfl.TFLuna(I2CAddr1, I2CPort)


# - - - Point cloud - - -
pcd = o3d.geometry.PointCloud()
point_cloud = []
i_degrees = 0
i_radians = 0.0

while True:
    try:
        servo1.angle = i_degrees
        i_radians = radians(i_degrees) * .8
        sleep(0.01)   # Delay 50ms for 20Hz loop-rate
        luna1.getData()     # Get data from first Luna
        #luna2.getData()     # Get data from second Luna
        # Combine data from both Lunas into a single point cloud
        point_cloud.append([float(luna1.dist) * cos(i_radians), 0.0, -1.0 * float(luna1.dist) * sin(i_radians)])
        # Visualize the point cloud
        pcd.points = o3d.utility.Vector3dVector(point_cloud)
        i_degrees = i_degrees + 1
    #
    #  Use control-C to break loop
    except KeyboardInterrupt:
        print( 'Keyboard Interrupt')
        break
    #
    #  Catch all other exceptions
    except:
        eType = sys.exc_info()[0]  # Return exception type
        print( eType)
        break
servo1.angle = 0
sleep(1)
i_degrees = 0
while True:
    try:
        servo1.angle = i_degrees
        i_radians = radians(i_degrees) * .75
        sleep(0.01)   # Delay 50ms for 20Hz loop-rate
        luna1.getData()     # Get data from first Luna
        #luna2.getData()     # Get data from second Luna
        # Combine data from both Lunas into a single point cloud
        point_cloud.append([float(luna1.dist) * cos(i_radians), 0.0, -1.0 * float(luna1.dist) * sin(i_radians)])
        # Visualize the point cloud
        pcd.points = o3d.utility.Vector3dVector(point_cloud)
        i_degrees = i_degrees + 1
    #
    #  Use control-C to break loop
    except KeyboardInterrupt:
        print( 'Keyboard Interrupt')
        break
    #
    #  Catch all other exceptions
    except:
        eType = sys.exc_info()[0]  # Return exception type
        print( eType)
        break

servo1.angle = 0
pcd.points = o3d.utility.Vector3dVector(point_cloud)
o3d.visualization.draw_geometries([pcd])

