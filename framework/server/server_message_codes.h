//
//  server_message_codes.h
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_SERVER_SERVER_MESSAGE_CODES_H_
#define FRAMEWORK_SERVER_SERVER_MESSAGE_CODES_H_

enum ServerMessageCodes {
    SERVER_MESSAGE_NONE = 0,
    SERVER_MESSAGE_STOP,
    SERVER_MESSAGE_USER = 0x00000FFF,
};

#endif  // FRAMEWORK_SERVER_SERVER_MESSAGE_CODES_H_
