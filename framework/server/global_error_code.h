/*
 * error_code.h
 *
 *  Created on: Dec 3, 2013
 *      Author: vagrant
 */

#ifndef FRAMEWORK_SERVER_GLOBAL_ERROR_CODE_H_
#define FRAMEWORK_SERVER_GLOBAL_ERROR_CODE_H_

enum GErrorCode
{
	CommonSuccess = 0,
	CommonError = 0x00001000,
	ServerReadyMaintenance,

	SessionHandleBegin = 0x00000100,
	SessionHandleKill,
	SessionHandleWarning,
	SessionHandleError,
	SessionHandleUnhandle,
	SessionHandleSuccess,
	SessionHandlePending,

	SessionKickNormal = 0x00000200,
	SessionKickByGM,
	SessionKickMapServerExit,
	SessionKickAccountRelogin,
	SessionKickInitDataError,
	SessionKickDataError,
	SessionKickTimeOut,
	SessionKickMaintenance,
	SessionKickHacker,
	SessionKickServerConfig,

    SPREE_SUM_ERROR_REACH_LIMIT = 0x00001200,

	SHOP_ORDER_HAS_BEEN_PROCESSED = 16408,
	SHOP_ORDER_DATA_NOT_JSON,
	SHOP_ORDER_ITEMID_NOT_EXISTS,
	SHOP_ORDER_DB_CANNOT_UPDATE,
	SHOP_ORDER_ID_NOT_EXISTS,
	SHOP_ORDER_ERROR_NOT_ENOUGH,
	SHOP_ORDER_PLAYER_NOT_ONLINE,
    
	INVITED_ERROR_COMPLETED = 16425,
	INVITED_ERROR_INVALID,

    
    

	CommonUserError = 0x0000FFFF
};

enum InAppPurchaseBuyError
{
	INAPPPURCHASE_BUY_OK,
	INAPPPURCHASE_BUY_CANCEL,
	INAPPPURCHASE_BUY_FAIL
};

const char* GetErrorMessage(GErrorCode code);

#endif  // FRAMEWORK_SERVER_GLOBAL_ERROR_CODE_H_
