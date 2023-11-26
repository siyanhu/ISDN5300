import argparse
import numpy as np
import cv2
import json
from threading import Event
from record3d import Record3DStream
import create_model as CLPModel

import time_io as tio

class DemoApp:
    def __init__(self):
        self.event = Event()
        self.session = None
        self.DEVICE_TYPE__TRUEDEPTH = 0
        self.DEVICE_TYPE__LIDAR = 1

    def on_new_frame(self):
        self.event.set()  # Notify the main thread to stop waiting and process new frame.

    def on_stream_stopped(self):
        print('Stream stopped')

    def connect_to_device(self, dev_idx, dev_name):
        print('Searching for devices')
        devs = Record3DStream.get_connected_devices()
        print('{} device(s) found'.format(len(devs)))
        for dev in devs:
            print('\tID: {}\n\tUDID: {}\n'.format(dev.product_id, dev.udid))

        if len(devs) <= dev_idx:
            raise RuntimeError('Cannot connect to device #{}, try different index.'
                               .format(dev_idx))

        dev = devs[dev_idx]
        self.session = Record3DStream()
        self.session.on_new_frame = self.on_new_frame
        self.session.on_stream_stopped = self.on_stream_stopped
        self.session.connect(dev)  # Initiate connection and start capturing

        self.device_name = dev_name
        self.device_id = dev_idx

    def get_intrinsic_mat_from_coeffs(self, coeffs):
        return np.array([[coeffs.fx,         0, coeffs.tx],
                         [        0, coeffs.fy, coeffs.ty],
                         [        0,         0,         1]])

    def start_processing_stream(self, display=True, save=False):
        while True:
            self.event.wait()  # Wait for new frame to arrive

            # Copy the newly arrived RGBD frame
            depth = self.session.get_depth_frame()
            rgb = self.session.get_rgb_frame()
            current_ts = tio.current_timestamp()

            dim = (rgb.shape[1], rgb.shape[0])
            depth = cv2.resize(depth, dim, interpolation = cv2.INTER_AREA)

            coeffi = self.session.get_intrinsic_mat()
            intrinsic_mat = self.get_intrinsic_mat_from_coeffs(coeffi)
            camera_pose = self.session.get_camera_pose()  # Quaternion + world position (accessible via camera_pose.[qx|qy|qz|qw|tx|ty|tz])
            
            clpproj.camera.set_basic_params(rgb.shape[1], rgb.shape[0])
            clpproj.camera.set_intrinsics(coeffi.fx, coeffi.fy, coeffi.tx, coeffi.ty)
            clpproj.create_cameras_txt()

            clpproj.feed_image_rgb(image_array=rgb, image_pose=camera_pose, timestamp=current_ts)
            clpproj.feed_image_depth(image_array=depth, image_pose=camera_pose, timestamp=current_ts)
            clpproj.create_images_txt()

            # You can now e.g. create point cloud by projecting the depth map using the intrinsic matrix.
            # rgb = cv2.cvtColor(rgb, cv2.COLOR_RGB2BGR)

            # Show the RGBD Stream
            if display:
                cv2.imshow('RGB', rgb)
                cv2.imshow('Depth', depth)
                cv2.waitKey(1)

            self.event.clear()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='Streaming Receiver',
        description='The script receives data streaming from Record3D devices.'
        )
    
    parser.add_argument('--device_id', type=int, help='specify an id of the camera in integer type.') 
    parser.add_argument('--device_name', type=str, help='specify a name of the camera in non-integer type.') 
    parser.add_argument('--collection_period', type=int, help='specify how many seconds required to do a full collection.') 
    args = parser.parse_args()

    app = DemoApp()
    app.connect_to_device(dev_idx=args.device_id, dev_name=args.device_name)
    clpproj = CLPModel.ColmapProject(args.device_id, args.device_name, 'home_streamming', args.collection_period)

    app.start_processing_stream()