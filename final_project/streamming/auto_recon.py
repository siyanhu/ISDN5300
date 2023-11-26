import pycolmap as colmap

import file_io as fio
import time_io as tio

def automatic_reconstruction(tag):
        __parent_dir = fio.createPath(fio.sep, ["/Users/siyanhu/GitHub/ISDN5300/final_project/data", tag])

        image_dir_rgb = fio.createPath(fio.sep, [__parent_dir], 'images')
        print(image_dir_rgb)
        image_dir_depth = fio.createPath(fio.sep, [__parent_dir], 'depths')
        sparse_dir = fio.createPath(fio.sep, [__parent_dir], 'sparse')
        db_path = fio.createPath(fio.sep, [__parent_dir], 'database.db')
        if fio.file_exist(db_path):
                fio.delete_file(db_path)

        fio.ensure_dir(image_dir_rgb)
        fio.ensure_dir(image_dir_depth)
        fio.ensure_dir(sparse_dir)
        init_ts = tio.current_timestamp()

        # ops = colmap.SiftExtractionOptions()
        # ops.max_num_features = 512
        # colmap.extract_features(db_path, image_dir_rgb, sift_options=ops)
        colmap.extract_features(db_path, image_dir_rgb)
        
        colmap.match_exhaustive(db_path)
        maps = colmap.incremental_mapping(db_path, image_dir_rgb, sparse_dir)
        maps[0].sparse_dir

    
automatic_reconstruction('home_streamming')