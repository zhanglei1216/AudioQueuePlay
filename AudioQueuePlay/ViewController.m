//
//  ViewController.m
//  AudioQueuePlay
//
//  Created by foreveross－bj on 15/8/27.
//  Copyright (c) 2015年 foreveross－bj. All rights reserved.
//

#import "ViewController.h"
#import "AudioPlay.h"

@interface ViewController ()
@property (strong, nonatomic) IBOutlet UITextField *userName;
@property (strong, nonatomic) IBOutlet UITextField *contactName;
@property (strong, nonatomic) AudioPlay *audioPlay;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    self.audioPlay = [[AudioPlay alloc] init];
}
- (IBAction)startOrStop:(UIButton *)sender {
    
    if ([sender.currentTitle isEqualToString:@"start"]) {
        [_audioPlay startWithUserId:_userName.text toUserId:_contactName.text];
        [sender setTitle:@"stop" forState:UIControlStateNormal];
    }else{
        [_audioPlay stop];
      [sender setTitle:@"start" forState:UIControlStateNormal];
    }
}
- (IBAction)tap:(id)sender {
    [_userName resignFirstResponder];
    [_contactName resignFirstResponder];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
