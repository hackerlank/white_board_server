/*
 * dn_account_service_pimp_app.cpp
 *
 *  Created on: Mar 12, 2014
 *      Author: jszhang
 */

#include "dn_account_service_pimpl.h"

#ifdef __cplusplus
extern "C" {
#endif


AccountServicePimpl* CreateAccountServicePimpl(rapidjson::Value& conf) {
	return new DNAccountServicePimpl(conf);
}

void DeleteAccountServicePimpl(AccountServicePimpl* account_service_pimp) {
	delete account_service_pimp;
}

#ifdef __cplusplus
}
#endif


