#ifndef _MESSSAGE_PROCESSOR_REGISTER_H__
#define _MESSSAGE_PROCESSOR_REGISTER_H__

#include "../Include/server_base.h"
#include "processor_base.h"
#include "google/protobuf/message.h"
#include "../Include/server_base.h"

namespace IslandSurvival {

class MessageProcessorRegister
{
public:
    // 获得单例指针
    static MessageProcessorRegister* Instance();
    // 注册消息，即绑定协议编号与消息结构
    int32_t RegistMessage(int32_t id, ::google::protobuf::Message* pb);
    // 注册消息处理器，即绑定协议编号与处理这种消息的入口函数
    int32_t RegistProcessor(int32_t id, MessageProcessorBase* processor);
    // 根据消息编号创建消息
    ::google::protobuf::Message* CreateMessage(int32_t id);
    // 处理消息，该函数会首先从消息中读取协议编号，再找到对应的处理函数交给它处理
    int32_t ProcessRequest(int32_t user_id, CMessage* message);

private:
    // 构造函数与析构函数
    MessageProcessorRegister() {};
    ~MessageProcessorRegister() {};
    // 单例指针
    static MessageProcessorRegister* m_pInstance;
    // 从协议编号到消息基类指针的映射
    std::map<int32_t, ::google::protobuf::Message*> m_proto_type;
    // 从协议编号到指向处理消息函数的函数指针的映射
    std::map<int32_t, MessageProcessorBase*> m_processor;

};
#define REGISTER MessageProcessorRegister::Instance()

// 巧用“静态对象的构造函数优先于主函数执行”的性质，用宏定义实现消息的注册
#define REGISTE_MESSAGE_ID(ID, PB_TYPE, PROCESSOR) \
        struct _static_##ID \
        { \
            PB_TYPE  __pb_seed_##ID; \
            _static_##ID() \
            { \
                MessageProcessorRegister::Instance()->RegistMessage((ID), &__pb_seed_##ID); \
                MessageProcessorRegister::Instance()->RegistProcessor((ID), (PROCESSOR)); \
            } \
        }; \
        static _static_##ID g_xxx89djhfq7##ID##__LINE__;




/***********************************************************************************/

#define MSG_CSLogin                     0x0000
#define MSG_SCLogin                     0x0001
#define MSG_CSRegist                    0x0002
#define MSG_SCRegist                    0x0003
#define MSG_PBRoomInfo                  0x0004
#define MSG_PBDetailRoomInfo            0x0005
#define MSG_CSChangeTeam                0x0006
#define MSG_CSPullAllRoomInfo           0x0007
#define MSG_SCPullAllRoomInfo           0x0008
#define MSG_CSCreateRoom                0x0009
#define MSG_SCCreateRoom                0x000a
#define MSG_CSJoinRoom                  0x000b
#define MSG_SCJoinRoom                  0x000c
#define MSG_CSPlayerExitRoom            0x000d
#define MSG_SCPlayerExitRoom            0x000e
#define MSG_SCPullOneRoomDetail         0x000f
#define MSG_CSReadyToStart              0x0010
#define MSG_SCGameLoading               0x0011
#define MSG_CSLoadSucceed               0x0012
#define MSG_SCGameStart                 0x0013
#define MSG_SCPlayerExitGame            0x0014
#define MSG_CSChoseRole                 0x0015
#define MSG_SCChoseRole                 0x0016
#define MSG_SCFrameMess                 0x0017
#define MSG_CSMove                      0x0018
#define MSG_SCMove                      0x0019
#define MSG_CSKill                      0x001a
#define MSG_SCKill                      0x001b
#define MSG_CSGamePause                 0x001c
#define MSG_SCGamePause                 0x001d
#define MSG_CSSkill                     0x001e
#define MSG_SCSkill                     0x001f
#define MSG_CSHeartbeat                 0x0020
#define MSG_CSGameEnd                   0x0021
#define MSG_SCGameEnd                   0x0022



}
#endif
