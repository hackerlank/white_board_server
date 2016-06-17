/*
 * dn_account_service_pimpl.cpp
 *
 *  Created on: Mar 12, 2014
 *      Author: vagrant
 */

#include "logging.h"
#include "dn_account_service_pimpl.h"
#include "DN_player_def.h"
#include "dn_player_session.h"
#include "cb_time.h"

const uint32_t reset_player_exchange_info_interval = 0;

DNAccountServicePimpl::DNAccountServicePimpl(rapidjson::Value& conf)
: AccountServicePimpl(conf)
{

}

bool DNAccountServicePimpl::Init() {

	return AccountServicePimpl::Init();
}

void DNAccountServicePimpl::Update() {
    AccountServicePimpl::Update();
}

void DNAccountServicePimpl::ResetAllPlayerExchangeInfo()
{

}

PlayerSession* DNAccountServicePimpl::CreatePlayerSession() {
	return new DNPlayerSession();
}

void DNAccountServicePimpl::DestroyPlayerSession(PlayerSession* playerSession) {
	delete playerSession;
}













