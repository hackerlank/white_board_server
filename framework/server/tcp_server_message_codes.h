//
//  tcp_server_message_codes.h
//  gateway
//
//  Created by Fubai Zhong on 10/8/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_SERVER_TCP_SERVER_MESSAGE_CODES_H_
#define FRAMEWORK_SERVER_TCP_SERVER_MESSAGE_CODES_H_

#include "server_message_codes.h"

enum TcpServerMessageCodes {
    TCP_CONNECTION_MESSAGE_READ = SERVER_MESSAGE_USER + 1,
    TCP_CONNECTION_MESSAGE_WRITE,
    TCP_CONNECTION_MESSAGE_CLOSE,
};

#endif  // FRAMEWORK_SERVER_TCP_SERVER_MESSAGE_CODES_H_
