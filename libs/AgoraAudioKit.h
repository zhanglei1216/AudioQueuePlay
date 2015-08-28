//
//  AgoraAudioKit.h
//  AgoraAudioKit
//
//  Created by GongYuhua on 14-7-2.
//  Copyright (c) 2014 AgoraLab. All rights reserved.
//
//  v0.4

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

typedef NS_ENUM(NSUInteger, AgoraAudioErrorCode) {
    AgoraAudio_NoError = 0,
    AgoraAudio_GetAudioAddrFailed = 1,
    AgoraAudio_GetAudioAddrZeroAddr,
    AgoraAudio_LoginMediaFailed,
    AgoraAudio_LoadAudioEngineError,
    AgoraAudio_StartCallError,
    AgoraAudio_CreateChannelTimeout,
    AgoraAudio_CreateChannelFailed,
    AgoraAudio_GetAudioAddrTimeout,
    AgoraAudio_ConnectMediaTimeout,
    AgoraAudio_ConnectMediaFailed,
    AgoraAudio_LoginMediaTimeout,
    AgoraAudio_RegetAudioAddr,
};

typedef NS_ENUM(NSUInteger, AgoraAudioMediaQuality) {
    AgoraAudio_MediaQuality_Unknown = 0,
    AgoraAudio_MediaQuality_Excellent = 1,
    AgoraAudio_MediaQuality_Good = 2,
    AgoraAudio_MediaQuality_Poor = 3,
    AgoraAudio_MediaQuality_Bad = 4,
    AgoraAudio_MediaQuality_VBad = 5,
    AgoraAudio_MediaQuality_Down = 6,
};

@interface VideoCanvas : NSObject
@property (strong, nonatomic) UIView* view;
@property (assign, nonatomic) float top;
@property (assign, nonatomic) float left;
@property (assign, nonatomic) float bottom;
@property (assign, nonatomic) float right;

@end

@interface AgoraAudioSessionStat : NSObject
@property (assign, nonatomic) NSUInteger duration;
@property (assign, nonatomic) NSUInteger txBytes;
@property (assign, nonatomic) NSUInteger rxBytes;
@end

@interface AgoraAudioSpeakerInfo : NSObject
@property (assign, nonatomic) NSUInteger uid;
@property (assign, nonatomic) NSUInteger volume;
@end

@interface AgoraAudioKit : NSObject

/**
 *  Returns Agora Voice SDK version.
 *
 *  @return SDK version
 */
+ (NSString *)getSdkVersion;
+ (NSString *)getChatEngineVersion;

/**
 *  The constructor for AgoraAudioKit Class. The method accept three callback blocks as arguments, thy are to be executed on main queue.
 *
 *  @param qualityBlock  executed every 3 seconds for each user.
 *  @param logEventBlock executed on receive log messages.
 *  @param errorBlock    executed on receive error messages.
 *
 *  @return an object of AgoraAudioKit class
 */
- (id)initWithVideoEnabled:(BOOL)enabled
              quality:(void(^)(NSUInteger uid, AgoraAudioMediaQuality quality, NSUInteger delay, NSUInteger jitter, NSUInteger lost, NSUInteger lost2))qualityBlock
                error:(void(^)(AgoraAudioErrorCode errorCode))errorBlock;

- (void)recapStatBlock:(void(^)(const char *recap_stat, NSInteger length))recapStatBlock;
- (void)speakersReportBlock:(void(^)(NSArray *speakers, NSInteger mixVolume))speakersReportBlock;

- (void)firstVideoFrameBlock:(void(^)(NSInteger))firstVideoFrameBlock;
- (void)userJoinedBlock:(void(^)(NSUInteger uid, NSInteger elapsed))userJoinedBlock;
- (void)userOfflineBlock:(void(^)(NSUInteger uid))userOfflineBlock;
- (void)userMuteAudioBlock:(void(^)(NSUInteger uid, bool muted))userMuteAudioBlock;
- (void)userMuteVideoBlock:(void(^)(NSUInteger uid, bool muted))userMuteVideoBlock;
- (void)localVideoStatBlock:(void(^)(NSInteger sentBytes, NSInteger sentFrames, NSInteger sentQP, NSInteger sentRtt, NSInteger sentLoss))localVideoStatBlock;
- (void)remoteVideoStatBlock:(void(^)(NSUInteger uid, NSInteger frameCount, NSInteger delay, NSInteger receivedBytes))remoteVideoStatBlock;

- (void)loadAudioEngineSuccessBlock:(void(^)())loadAudioEngineSuccessBlock;
- (void)rejoinChannelSuccessBlock:(void(^)(NSString* channel, NSUInteger uid, NSInteger elapsed))rejoinChannelSuccessBlock;
- (void)getAudioServerAddressSuccess:(void(^)())getAudioServerAddressSuccess;
- (void)logEventBlock:(void(^)(NSString *eventString))logEventBlock;
- (void)updateSessionStatBlock:(void(^)(AgoraAudioSessionStat* stat))updateSessionBlock;
- (void)leaveChannelBlock:(void(^)(AgoraAudioSessionStat* stat))leaveChannelBlock;
- (void)networkQualityBlock:(void(^)(AgoraAudioMediaQuality quality))networkQualityBlock;
- (BOOL)isBuiltinSpeakerOn;

/**
 *  Create an open UDP socket to the AgoraAudioKit cloud service to join a channel.
 *
 *  @param vendorKey    Account level credentials issued by AgoraLab company to each vendor.
 *  @param channelName  Joining in the same channel indicates those clients have entered in one room.
 *  @param info         Optional argument can be whatever the programmer likes personally.
 *  @param uid          Optional argument is the unique ID for each member in one channel. Uid management will be taken over if it is 0 by default.
 *  @param successBlock executed on main queue after join the channel successfully.
 */
- (int)joinChannelByKey:(NSString *)vendorKey
             channelName:(NSString *)channelName
                    info:(NSString *)info
                     uid:(NSUInteger)uid
                 success:(void(^)(NSString* channel, NSUInteger uid, NSInteger elapsed))successBlock;

/**
 *  Leave channel positively.
 */
- (int)leaveChannel;

/**
 *  Start echo test.
 */
- (int)startEchoTest:(NSString *)vendorKey
              success:(void(^)(NSString* channel, NSUInteger uid, NSInteger elapsed))successBlock;

/**
 *  Stop echo test.
 */
- (int)stopEchoTest;

/**
 *  Enable network test.
 */
- (int)enableNetworkTest:(NSString *)vendorKey;

/**
 *  Disable network test.
 */
- (int)disableNetworkTest;

/**
 *  Mute / Unmute the client.
 *
 *  @param shouldMute set YES to mute, NO to unmute.
 */
- (int)setMute:(BOOL)shouldMute;

/**
 *  Mute / Unmute peers.
 *
 *  @param shouldMute set YES to mute, NO to unmute.
 */
- (int)setMutePeers:(BOOL)shouldMute;

/**
 *  Enable / Disable speaker
 *
 *  @param enableSpeaker set YES to enable, NO to disable.
 */
- (int)setEnableSpeaker:(BOOL)enableSpeaker;

/**
 *  Set speaker volume.
 *
 *  @param volume set between 0 and 255.
 */
- (int)setSpeakerVolume:(NSUInteger)volume;

/**
 *  Specify sdk parameters
 *
 *  @param options, sdk options in json format.
 */
- (int)setParameters:(NSString *)options;

/**
 *  Enable speakers' volume report
 *
 *  @param interval, <=0 - disabled, >0 interval in ms.
 */
- (int)enableSpeakersReport:(NSInteger)interval
                smoothFactor:(NSInteger)smoothFactor;

/**
 *  Enable recap stat
 *
 *  @param interval, <=0 - disabled, >0 interval in ms.
 */
- (int)enableRecapStat:(NSInteger)interval;

/**
 *  Start playing recap conversation
 *
 */
- (int)startRecapPlay;

/**
 *  Start recording conversation to file specified by the file path
 *
 *  @param filePath, file path to save recorded conversation.
 */
- (int)startRecording:(NSString*)filePath;

/**
 *  Stop conversation recording
 *
 */
- (int)stopRecording;

/**
 *  Specify sdk profile
 *
 *  @param profile, sdk profile in json format.
 *  @param merge, specify merge or replace old profile
 */
- (int)setProfile:(NSString *)profile
             merge:(BOOL)merge;

- (int)setupVideo:(VideoCanvas*)local andRemote:(VideoCanvas*)remote;

- (NSString*) makeQualityReportUrl:(NSString*)vendorKey
                            channel:(NSString*) channel
                        listenerUid:(NSUInteger) listenerUid
                       speakerrUid:(NSUInteger) speakerUid;

- (NSString*) getCallId;

- (int) rate:(NSString*)callId
      rating:(NSInteger) rating;

- (int) complain:(NSString*) callId;

/**
 * specify camera index
 * 0 － back camera
 * 1 － front camera
 * @param cameraIndex camera index
 */
- (int)setVideoCamera: (int)cameraIndex;
- (int)setVideoResolution: (int)width andHeight: (int)height;

- (int)switchCamera;

- (int)enableRemoteVideo: (bool)enabled;
- (int)enableLocalVideo: (bool)enabled;
@end
