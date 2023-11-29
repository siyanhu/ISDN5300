import cv2 as cv
import numpy as np
import pycolmap as colmap

import time_io as tio
import file_io as fio

class CameraObject(object):
    status = 0

    def __init__(self, device_id, device_name):
        self.idx = str(device_id)
        self.name = str(device_name)

    def update_status(self, status):
        '''
        status = 0: not connected
        status = 1: connected, standby
        status = 2: streaming
        '''
        self.status = status

    def set_basic_params(self, width, height):
        self.width = width
        self.height = height

    def set_intrinsics(self, fx, fy, tx, ty):
        self.intrinsics_fx = fx
        self.intrinsics_fy = fy
        self.intrinsics_tx = tx
        self.intrinsics_ty = ty
        self.intrinsics = np.array([[fx, 0, tx],
                                    [0, fy, ty],
                                    [0, 0, 1]])
    
    def output_params(self, path):
        # fx, fy, cx, cy
        # or fx, fy, cx, cy, k1, k2, p1, p2, k3
        if fio.file_exist(path):
            with open(path, mode='r') as f:
                lines = f.readlines()
                for l in lines:
                    elem = l.split(' ')
                    if len(elem) < 2:
                        break
                    elif (elem[0] == self.idx):
                        return
                
        output = ' '.join([self.idx, self.name, 
                           str(self.width), str(self.height),
                           str(self.intrinsics_fx), str(self.intrinsics_fy), str(self.intrinsics_tx), str(self.intrinsics_ty)])
        output += '\n'
        file = open(path, "a")  # append mode
        file.write(output)
        file.close()

        
class ImageObject(object):
    timestamp = 0

    def __init__(self, type, path, name):
        '''
        type = depth / RGB
        '''
        self.type = type
        self.image_path = path
        self.image_name = name
    
    def set_extrinsics(self, camera_pose):
        # Quaternion + world position (accessible via camera_pose.[qx|qy|qz|qw|tx|ty|tz])
        self.camera_pose = camera_pose

    def output_extrinsics(self, index, opath, camera):
        # IMAGE_ID, QW, QX, QY, QZ, TX, TY, TZ, CAMERA_ID, NAME
        if fio.file_exist(opath):
            with open(opath, mode='r') as f:
                lines = f.readlines()
                for l in lines:
                    elem = l.split(' ')
                    if len(elem) < 2:
                        break
                    elif (elem[-1] == self.image_name):
                        return

        output = ' '.join([str(index), 
                           str(self.camera_pose.qw), str(self.camera_pose.qx), str(self.camera_pose.qy), str(self.camera_pose.qz), 
                           str(self.camera_pose.tx), str(self.camera_pose.ty), str(self.camera_pose.tz), 
                           str(camera.idx), self.image_name])
        output += '\n\n'
        file = open(opath, "a")  # append mode
        file.write(output)
        file.close()


class ColmapProject():
    sparse_file_paths = []
    db_file = []
    images = []
    depth_images = []

    def __init__(self, camera_id, camera_name, tag):
        self.camera = CameraObject(camera_id, camera_name)
        __parent_dir = fio.createPath(fio.sep, ["/Users/siyanhu/GitHub/ISDN5300/final_project/data", tag])
        if fio.file_exist(__parent_dir):
            fio.delete_folder(__parent_dir)
        fio.ensure_dir(__parent_dir)

        self.image_dir_rgb = fio.createPath(fio.sep, [__parent_dir], 'images')
        self.image_dir_depth = fio.createPath(fio.sep, [__parent_dir], 'depths')
        self.sparse_dir = fio.createPath(fio.sep, [__parent_dir], 'sparse')
        self.db_path = fio.createPath(fio.sep, [__parent_dir], 'database.db')
        fio.ensure_dir(self.image_dir_rgb)
        fio.ensure_dir(self.image_dir_depth)
        fio.ensure_dir(self.sparse_dir)
        self.init_ts = tio.current_timestamp()

    def feed_image_rgb(self, image_array, image_pose, timestamp):
        image_name = str(len(self.images)) + '_' + 'rgb' + '.png'
        higher_dir = self.image_dir_rgb
        
        img_path = fio.createPath(fio.sep, [higher_dir], image_name)
        img = ImageObject(type, img_path, image_name)
        cv.imwrite(img_path, image_array)
        img.set_extrinsics(image_pose)
        img.timestamp = timestamp
        self.images.append(img)
    
    def feed_image_depth(self, image_array, image_pose, timestamp):
        image_name = str(len(self.images)) + '_' + 'depth' + '.png'
        higher_dir = self.image_dir_depth
        
        img_path = fio.createPath(fio.sep, [higher_dir], image_name)
        img = ImageObject(type, img_path, image_name)
        cv.imwrite(img_path, image_array)
        img.set_extrinsics(image_pose)
        img.timestamp = timestamp
        self.depth_images.append(img)
    
    def create_cameras_txt(self):
        camera_opath = fio.createPath(fio.sep, [self.sparse_dir], 'cameras.txt')
        self.camera.output_params(camera_opath)

    def create_images_txt(self):
        images_opath = fio.createPath(fio.sep, [self.sparse_dir], 'images.txt')
        for i in range(0, len(self.images)):
            imgObj = self.images[i]
            imgObj.output_extrinsics(i, images_opath, self.camera)