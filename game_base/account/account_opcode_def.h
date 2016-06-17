/*
 * opcode_def.h
 *
 *  Created on: Dec 9, 2013
 *      Author: jszhang
 */

#ifndef GAME_BASE_ACCOUNT_ACCOUNT_OPCODE_DEF_H_
#define GAME_BASE_ACCOUNT_ACCOUNT_OPCODE_DEF_H_

#include <rapidjson/document.h>
#include "global_error_code.h"
#include "player_def.h"
#include <list>

#define ERROR_NOT_IN_ROOM ("not in any room")
#define ERROR_PACKET_WRONG ("the packet you send is wrong")
#define ERROR_SHAPE_ID_NOT_EXIST ("this shape id is not exist")
#define ERROR_ADD_SHAPE_TOO_MANY ("Opertion error,you add too many shape once")
#define ERROR_CREATE_ROOM_EXIST  ("create room when room already exist")
#define ERROR_ENTER_ROOM_NOT_EXIST ("enter room when room not exist")
#define ERROR_COPY_ROOM_SRC_NOT_EXIST ("copy room src room not exist")
#define ERROR_COPY_ROOM_DES_EXIST ("copy room des room already exist")
#define ERROR_SYNC_PLAYER_LIST_NO_ROOOM ("ask player list ,room not exist")
#define ERROR_DELETE_ROOM_NOT_EXIST ("delete room not exist")
#define ERROR_ROOM_DELET ("the room you in has been delete,you quit")

enum ShapeErrorCode{
	Shape_Common_Error,	//通常错误
	Shape_No_In_Room,//不在任何桌子内操作 图元
	Shape_Packet_Wrong,//发送的协议内容有问题
	Shape_Id_Not_Exist,//试图删除的图元不存在
	Shape_Wrong_Operation,//错误操作
	Shape_Leave_When_not_In_Room,//离桌但不在房间里
	Shape_Create_Room_When_Key_Exist,//创建房间但房间key已存在
	Shape_Copy_Room_When_Src_Key_No_Exist,//拷贝房间 原始房间不存在
	Shape_Copy_Room_When_Des_Key_Exist,//拷贝房间 目的房间已存在
	Shape_Enter_Room_Not_Exist,//进入的房间不存在
	Shape_Sync_Player_List_No_Room, //请求的房间的人列表,但是房间不存在
	Shape_Delete_Room_Not_Exist,//删除的房间不存在
	Shape_Room_Delete_You_Out,//房间被删除,你被剔除房间
};

enum AccountOpcode {
	ProtoBuff_Login=100,//登录
	ProtoBuff_Room_Operate,//房间操作
	ProtoBuff_Sync_Room_List,//同步房间列表
	ProtoBuff_Sync_Player_List,//同步用户列表
	ProtoBuff_Add_Sharp,//增加图元
	ProtoBuff_Remove_Sharp,//删除图元
	ProtoBuff_Error_Info,//错误信息
};

#endif  // GAME_BASE_ACCOUNT_ACCOUNT_OPCODE_DEF_H
