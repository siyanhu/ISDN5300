//
//  CollectorViewController.m
//  DataCollector
//
//  Created by HU Siyan on 21/11/2023.
//

#import "CollectorViewController.h"
#import <AVFoundation/AVFCore.h>
#import <AVFoundation/AVCaptureSession.h>

@interface CollectorViewController () {
    AVCaptureDevice *_captureDevice;
    float min_depth, max_depth;
    float scale_movement;
    float min_depth_range, max_depth_range;
    
}

@property (nonatomic, strong) AVCaptureSession *av_session;

@end

@implementation CollectorViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
}

#pragma mark - Configuration
- (void)initTheCamera {
    _captureDevice = [AVCaptureDevice defaultDeviceWithDeviceType:AVCaptureDeviceTypeBuiltInLiDARDepthCamera mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionUnspecified];
    
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
