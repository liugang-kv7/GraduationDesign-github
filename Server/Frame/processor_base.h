#pragma once
#ifndef _PROCESSOR_BASE_H__
#define _PROCESSOR_BASE_H__

#include "../Include/server_base.h"
#include "../Include/CMessage.h"

namespace IslandSurvival {

class MessageProcessorBase
{
public:
    virtual int32_t ProcessRequest(int32_t user_id, const CMessage& message) = 0;
};


}

#endif