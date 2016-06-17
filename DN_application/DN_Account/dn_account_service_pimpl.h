/*
 * dn_account_pimpl.h
 *
 *  Created on: Mar 12, 2014
 *      Author: jszhang
 */

#ifndef DN_APPLICATION_DN_ACCOUNT_DN_ACCOUNT_SERVICE_PIMPL_H_
#define DN_APPLICATION_DN_ACCOUNT_DN_ACCOUNT_SERVICE_PIMPL_H_

#include "rapidjson/document.h"

#include "account_service_pimpl.h"

class DNAccountServicePimpl : public AccountServicePimpl {
public:
	explicit DNAccountServicePimpl(rapidjson::Value& conf);

protected:
	virtual bool Init();
    void ResetAllPlayerExchangeInfo();
public:
    virtual void Update();
	virtual PlayerSession* CreatePlayerSession();
	virtual void DestroyPlayerSession(PlayerSession* playerSession);
private:
    time_t   update_time_;
};



#endif  // DN_APPLICATION_DN_ACCOUNT_DN_ACCOUNT_SERVICE_PIMPL_H_
