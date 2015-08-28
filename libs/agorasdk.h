
#ifndef io_agora_sdk2_h
#define io_agora_sdk2_h

#import "AgoraAudioKit.h"

typedef NS_ENUM(NSUInteger, AgoraEcode){
    AgoraEcode_SUCCESS = 0,
    AgoraEcode_LOGOUT_E_OTHER = 100,
    AgoraEcode_LOGOUT_E_USER = 101,
    AgoraEcode_LOGOUT_E_NET = 102,
    AgoraEcode_LOGOUT_E_KICKED = 103,
    AgoraEcode_LOGOUT_E_PACKET = 104,
    AgoraEcode_LOGOUT_E_TOKENEXPIRED = 105,
    AgoraEcode_LOGIN_E_OTHER = 200,
    AgoraEcode_LOGIN_E_NET = 201,
    AgoraEcode_LOGIN_E_FAILED = 202,
    AgoraEcode_LOGIN_E_CANCEL = 203,
    AgoraEcode_LOGIN_E_TOKENEXPIRED = 204,
    AgoraEcode_JOINCHANNEL_E_OTHER = 300,
    AgoraEcode_SENDMESSAGE_E_OTHER = 400,
    AgoraEcode_SENDMESSAGE_E_TIMEOUT = 401,
    AgoraEcode_QUERYUSERNUM_E_OTHER = 500,
    AgoraEcode_QUERYUSERNUM_E_TIMEOUT = 501,
    AgoraEcode_QUERYUSERNUM_E_BYUSER = 502,
    AgoraEcode_LEAVECHANNEL_E_OTHER = 600,
    AgoraEcode_LEAVECHANNEL_E_KICKED = 601,
    AgoraEcode_LEAVECHANNEL_E_BYUSER = 602,
    AgoraEcode_INVITE_E_OTHER = 700,
    AgoraEcode_INVITE_E_REINVITE = 701,
    AgoraEcode_INVITE_E_NET = 702,
    AgoraEcode_INVITE_E_PEEROFFLINE = 703,
    AgoraEcode_INVITE_E_TIMEOUT = 704,
    AgoraEcode_INVITE_E_CANTRECV = 705
};

@interface AgoraAPI : NSObject

+ (AgoraAPI*) getInstance;
+ (AgoraAPI*) getInstanceWithoutMedia;
- (AgoraAudioKit*) getMedia;

@property (copy) void(^onReconnecting)(uint32_t nretry) ;
@property (copy) void(^onReconnected)(int fd) ;
@property (copy) void(^onLoginSuccess)(int fd) ;
@property (copy) void(^onLogout)(AgoraEcode ecode) ;
@property (copy) void(^onLoginFailed)(AgoraEcode ecode) ;
@property (copy) void(^onChannelJoined)(NSString* channelID) ;
@property (copy) void(^onChannelJoinFailed)(NSString* channelID,AgoraEcode ecode) ;
@property (copy) void(^onChannelLeaved)(NSString* channelID,AgoraEcode ecode) ;
@property (copy) void(^onChannelUserJoined)(NSString* account,uint32_t uid) ;
@property (copy) void(^onChannelUserLeaved)(NSString* account,uint32_t uid) ;
@property (copy) void(^onChannelUserList)(NSMutableArray* accounts, NSMutableArray* uids);
@property (copy) void(^onChannelQueryUserNumResult)(NSString* channelID,AgoraEcode ecode,int num) ;
@property (copy) void(^onInviteReceived)(NSString* channelID,NSString* account,uint32_t uid) ;
@property (copy) void(^onInviteReceivedByPeer)(NSString* channelID,NSString* account,uint32_t uid) ;
@property (copy) void(^onInviteAcceptedByPeer)(NSString* channelID,NSString* account,uint32_t uid) ;
@property (copy) void(^onInviteRefusedByPeer)(NSString* channelID,NSString* account,uint32_t uid) ;
@property (copy) void(^onInviteFailed)(NSString* channelID,NSString* account,uint32_t uid,AgoraEcode ecode) ;
@property (copy) void(^onInviteEndByPeer)(NSString* channelID,NSString* account,uint32_t uid) ;
@property (copy) void(^onInviteEndByMyself)(NSString* channelID,NSString* account,uint32_t uid) ;
@property (copy) void(^onMessageSendError)(NSString* messageID,AgoraEcode ecode) ;
@property (copy) void(^onMessageSendSuccess)(NSString* messageID) ;
@property (copy) void(^onMessageAppReceived)(NSString* msg) ;
@property (copy) void(^onMessageInstantReceive)(NSString* account,uint32_t uid,NSString* msg) ;
@property (copy) void(^onMessageChannelReceive)(NSString* channelID,NSString* account,uint32_t uid,NSString* msg) ;
@property (copy) void(^onLog)(NSString* txt) ;



- (void) login:(NSString*)vendorID account:(NSString*)account token:(NSString*)token uid:(uint32_t)uid deviceID:(NSString*)deviceID ;
- (void) logout;
- (void) channelJoin:(NSString*)channelID ;
- (void) channelLeave:(NSString*)channelID ;
- (void) channelQueryUserNum:(NSString*)channelID ;
- (void) channelInviteUser:(NSString*)channelID account:(NSString*)account uid:(uint32_t)uid ;
- (void) channelInvitePhone:(NSString*)channelID phoneNum:(NSString*)phoneNum uid:(uint32_t)uid ;
- (void) channelInviteAccept:(NSString*)channelID account:(NSString*)account uid:(uint32_t)uid ;
- (void) channelInviteRefuse:(NSString*)channelID account:(NSString*)account uid:(uint32_t)uid ;
- (void) channelInviteEnd:(NSString*)channelID account:(NSString*)account uid:(uint32_t)uid ;
- (void) messageAppSend:(NSString*)msg msgID:(NSString*)msgID ;
- (void) messageInstantSend:(NSString*)account uid:(uint32_t)uid msg:(NSString*)msg msgID:(NSString*)msgID ;
- (void) messageChannelSend:(NSString*)channelID msg:(NSString*)msg msgID:(NSString*)msgID ;
- (void) messagePushSend:(NSString*)account uid:(uint32_t)uid msg:(NSString*)msg msgID:(NSString*)msgID ;
- (void) messageChatSend:(NSString*)account uid:(uint32_t)uid msg:(NSString*)msg msgID:(NSString*)msgID ;
- (void) messageDTMFSend:(uint32_t)uid msg:(NSString*)msg msgID:(NSString*)msgID ;
- (void) setBackground:(uint32_t)bOut ;
- (void) setNetworkStatus:(uint32_t)bOut ;
- (void) ping;
- (void) start;
- (void) stop;
- (void) dbg:(NSString*)a b:(NSString*)b ;

@end

#endif
