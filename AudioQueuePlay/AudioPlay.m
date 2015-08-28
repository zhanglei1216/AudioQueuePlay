//
//  AudioPlay.m
//  AudioQueuePlay
//
//  Created by foreveross－bj on 15/8/27.
//  Copyright (c) 2015年 foreveross－bj. All rights reserved.
//

#import "AudioPlay.h"
#import <AVFoundation/AVFoundation.h>

#import "agorasdk.h"

@interface AudioPlay (){
    BOOL isRunning;
    dispatch_queue_t rdQueue;
    dispatch_queue_t pdQueue;
    
}

@property (nonatomic, strong) NSString *toUserId;
@property (atomic, strong) NSMutableArray *readMessageQueue;
@end
@implementation AudioPlay

- (instancetype)init
{
    self = [super init];
    if (self) {
        UInt32 category = kAudioSessionCategory_PlayAndRecord;
        
        AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
    }
    return self;
}
static void  HandleInputBuffer (	void *								inUserData,
                     AudioQueueRef						inAQ,
                     AudioQueueBufferRef					inBuffer,
                     const AudioTimeStamp *				inStartTime,
                     UInt32								inNumPackets,
                     const AudioStreamPacketDescription*	inPacketDesc)
{
    AudioPlay *audioPlay = (__bridge AudioPlay *)inUserData;
    NSData *data = [NSData dataWithBytes:inBuffer->mAudioData length:inBuffer->mAudioDataByteSize];
    NSString *message = [data base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength];
    
    [[AgoraAPI getInstanceWithoutMedia] messageInstantSend:audioPlay.toUserId uid:0 msg:message msgID:[[NSUUID UUID] UUIDString]];
    if (audioPlay->isRunning) {
        AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
    }
}

static void AQBufferCallback(void *					inUserData,
                                AudioQueueRef			inAQ,
                                AudioQueueBufferRef		inCompleteAQBuffer){
    AudioPlay *audioPlay = (__bridge AudioPlay *)inUserData;
    if (audioPlay.readMessageQueue.count > 0) {
        NSString *msg = [audioPlay.readMessageQueue firstObject];
        [audioPlay.readMessageQueue removeObjectAtIndex:0];
        NSData *data = [[NSData alloc] initWithBase64EncodedString:msg options:NSDataBase64DecodingIgnoreUnknownCharacters];
        [data getBytes:inCompleteAQBuffer->mAudioData length:[data length]];
        inCompleteAQBuffer->mAudioDataByteSize = (UInt32)[data length];
        inCompleteAQBuffer->mPacketDescriptionCount = inCompleteAQBuffer->mAudioDataByteSize / audioPlay->mRecordFormat.mBytesPerPacket;
        AudioQueueEnqueueBuffer(inAQ, inCompleteAQBuffer, 0, NULL);
    }else{
        if (audioPlay->isRunning)
        {
            AQBufferCallback(inUserData, inAQ, inCompleteAQBuffer);
        }
        else
        {
            AudioQueueStop(inAQ, false);
        }
    }
}
- (void)setupAudioFormat:(UInt32)inFormatID
{
    memset(&self->mRecordFormat, 0, sizeof(self->mRecordFormat));
    
    UInt32 size = sizeof(self->mRecordFormat.mSampleRate);
    
    NSAssert(AudioSessionGetProperty(	kAudioSessionProperty_CurrentHardwareSampleRate,
                                     &size,
                                     &self->mRecordFormat.mSampleRate), @"couldn't get hardware sample rate");
    
    size = sizeof(self->mRecordFormat.mChannelsPerFrame);
    NSAssert(AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareInputNumberChannels,
             &size,
             &self->mRecordFormat.mChannelsPerFrame), @"couldn't get input channel count");
    
    self->mRecordFormat.mFormatID = inFormatID;
    if (inFormatID == kAudioFormatLinearPCM)
    {
        // if we want pcm, default to signed 16-bit little-endian
        self->mRecordFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
        self->mRecordFormat.mBitsPerChannel = 16;
        self->mRecordFormat.mBytesPerPacket = self->mRecordFormat.mBytesPerFrame = (self->mRecordFormat.mBitsPerChannel / 8) * self->mRecordFormat.mChannelsPerFrame;
        self->mRecordFormat.mFramesPerPacket = 1;
    }
}


- (int)computeRecordBufferSize:(const AudioStreamBasicDescription *)format :(float) seconds
{
    int packets, frames, bytes = 0;
    @try {
        frames = (int)ceil(seconds * format->mSampleRate);
        
        if (format->mBytesPerFrame > 0)
            bytes = frames * format->mBytesPerFrame;
        else {
            UInt32 maxPacketSize;
            if (format->mBytesPerPacket > 0)
                maxPacketSize = format->mBytesPerPacket;	// constant packet size
            else {
                UInt32 propertySize = sizeof(maxPacketSize);
                NSAssert(AudioQueueGetProperty(rQueue, kAudioQueueProperty_MaximumOutputPacketSize, &maxPacketSize,
                                               &propertySize), @"couldn't get queue's maximum output packet size");
            }
            if (format->mFramesPerPacket > 0)
                packets = frames / format->mFramesPerPacket;
            else
                packets = frames;	// worst-case scenario: 1 frame in a packet
            if (packets == 0)		// sanity check
                packets = 1;
            bytes = packets * maxPacketSize;
        }
    } @catch (NSException *e) {

        NSLog(@"%@", e);
        return 0;
    }	
    return bytes;
}
- (void)setUpAudioFormat {
    
    UInt32 size = sizeof(mRecordFormat.mSampleRate);
    AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareSampleRate, &size, &mRecordFormat.mSampleRate);
    
    size = sizeof(mRecordFormat.mChannelsPerFrame);
    AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareInputNumberChannels, &size, &mRecordFormat.mChannelsPerFrame);
    
    mRecordFormat.mFormatID = kAudioFormatLinearPCM;
    
    mRecordFormat.mSampleRate = 44100.0;
    
    mRecordFormat.mChannelsPerFrame = 2;
    
    mRecordFormat.mBitsPerChannel = 16;
    
    mRecordFormat.mBytesPerFrame = mRecordFormat.mChannelsPerFrame * sizeof (SInt16); mRecordFormat.mBytesPerPacket = mRecordFormat.mBytesPerFrame;
    
    mRecordFormat.mFramesPerPacket = 1;
    
    mRecordFormat.mFormatFlags = // kLinearPCMFormatFlagIsEndian|
    kLinearPCMFormatFlagIsSignedInteger|
    kLinearPCMFormatFlagIsPacked;
}

void DeriveBufferSize ( AudioQueueRef audioQueue, AudioStreamBasicDescription ASBDescription, Float64 seconds, UInt32 *outBufferSize )

{
    
    static const int maxBufferSize = 0x50000;
    
    int maxPacketSize = ASBDescription.mBytesPerPacket;
    
    if (maxPacketSize == 0)
        
    {
        
        UInt32 maxVBRPacketSize = sizeof(maxPacketSize);
        
        AudioQueueGetProperty (audioQueue, kAudioQueueProperty_MaximumOutputPacketSize,
                               // in Mac OS X v10.5, instead use //kAudioConverterPropertyMaximumOutputPacketSize
                               &maxPacketSize, &maxVBRPacketSize );
        
    }
    
    Float64 numBytesForTime = ASBDescription.mSampleRate * maxPacketSize * seconds;
    
    *outBufferSize = (UInt32)(numBytesForTime < maxBufferSize ? numBytesForTime : maxBufferSize);
    
}

- (void)startWithUserId:(NSString *)userId toUserId:(NSString *)toUserId{
    isRunning = YES;
    self.toUserId = toUserId;
    self.readMessageQueue = [[NSMutableArray alloc] init];
    
    [[AgoraAPI getInstanceWithoutMedia] login:@"6D7A26A1D3554A54A9F43BE6797FE3E2" account:@"userId" token:@"2c72883e86624caaa0dbb0b6fee079fe" uid:0 deviceID:@""];
    [self startRecord];
    [self startPlay];
    
    [AgoraAPI getInstanceWithoutMedia].onMessageInstantReceive = ^(NSString* account,uint32_t uid,NSString* msg){
        [_readMessageQueue addObject:msg];
    };
        
}

- (void)stop{
    isRunning = NO;
    
    [[AgoraAPI getInstanceWithoutMedia] logout];
    
    if (rQueue) {
        AudioQueueStop(rQueue, true);
        AudioQueueDispose(rQueue, true);
    }
    
    if (pQueue) {
        AudioQueueStop(pQueue, true);
        AudioQueueDispose(pQueue, true);
    }
}

- (void)startRecord{
    
    UInt32 bufferByteSize;
    
    // Set Up Audio Format
    
    [self setUpAudioFormat];
    
    // Creating a Recording Audio Queue
    
    OSStatus err = AudioQueueNewInput(&mRecordFormat, (AudioQueueInputCallback)HandleInputBuffer, (__bridge void*)self, NULL, kCFRunLoopCommonModes, 0, &rQueue);
    
    if (err == -1) { }
    
    // Set an Audio Queue Buffer Size
    
    DeriveBufferSize(rQueue, mRecordFormat,1, &bufferByteSize);
    
    // Prepare a Set of Audio Queue Buffers
    for (int i = 0; i < kNumberRecordBuffers; ++i) {
        AudioQueueAllocateBuffer(rQueue, bufferByteSize, &rBuffers[i]);
        AudioQueueEnqueueBuffer(rQueue,rBuffers[i],0,NULL); }
    AudioQueueStart(rQueue, NULL);
};

- (void)startPlay{
    int i, bufferByteSize;
    
   AudioQueueNewOutput(&mRecordFormat, AQBufferCallback, (__bridge void *)self,
                                 CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &pQueue);
   
    
//    NSAssert(AudioQueueAddPropertyListener(pQueue, kAudioQueueProperty_IsRunning, isRunningProc, self), @"adding property listener");
    
     bufferByteSize = [self computeRecordBufferSize:&mRecordFormat :kBufferDurationSeconds];
    for (i = 0; i < kNumberRecordBuffers; ++i) {
        AudioQueueAllocateBuffer(pQueue, bufferByteSize, &pBuffers[i]);
        AudioQueueEnqueueBuffer(pQueue, pBuffers[i], 0, NULL);
    }
    
    // set the volume of the queue
    AudioQueueSetParameter(pQueue, kAudioQueueParam_Volume, 1.0);
   
    
   AudioQueueStart(pQueue, NULL);
}

@end
