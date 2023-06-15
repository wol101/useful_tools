/*
 *  TCPIPMessage.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 13/01/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#ifndef TCPIPMessage_h
#define TCPIPMessage_h

#include <stdint.h>

namespace AsynchronousGA
{

struct TCPIPMessage
{
    char text[32];
    uint32_t length;
    uint32_t runID;
    double score;
    uint32_t md5[4];

    enum { StandardMessageSize = 64 };
};

}

#endif
