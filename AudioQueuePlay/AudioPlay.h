//
//  AudioPlay.h
//  AudioQueuePlay
//
//  Created by foreveross－bj on 15/8/27.
//  Copyright (c) 2015年 foreveross－bj. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#define kNumberRecordBuffers 3
#define kBufferDurationSeconds .5

@interface AudioPlay : NSObject{
    AudioQueueRef pQueue;
    AudioQueueBufferRef pBuffers[kNumberRecordBuffers];
    AudioQueueRef rQueue;
    AudioQueueBufferRef rBuffers[kNumberRecordBuffers];
    AudioFileID					rRecordFile;
    SInt64	rRecordPacket;
    AudioStreamBasicDescription	mRecordFormat;
    CFStringRef rFileName;
}

- (void)startWithUserId:(NSString *)userId toUserId:(NSString *)toUserId;
- (void)stop;
@end
