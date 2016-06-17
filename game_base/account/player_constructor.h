/*
 * player_construct_def.h
 *
 *  Created on: Jan 24, 2014
 *      Author: vagrant
 */

#ifndef GAME_BASE_ACCOUNT_PLAYER_CONSTRUCTOR_H_
#define GAME_BASE_ACCOUNT_PLAYER_CONSTRUCTOR_H_

#include <inttypes.h>
#include <functional>

class PlayerSession;
class AccountServicePimpl;


typedef std::function<PlayerSession*()> player_session_creator_func;
typedef std::function<void(PlayerSession* playerSession)> player_session_deletor_func;


#endif  // GAME_BASE_ACCOUNT_PLAYER_CONSTRUCTOR_H_
