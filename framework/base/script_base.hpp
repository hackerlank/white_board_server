/*
 * script_base.hpp
 *
 *  Created on: Dec 18, 2013
 *      Author: jszhang
 */

#ifndef FRAMEWORK_BASE_SCRIPT_BASE_HPP_
#define FRAMEWORK_BASE_SCRIPT_BASE_HPP_

#include <string>
#include <set>

#include "lua/lua_tinker.hpp"
#include "logging.h"
#include "transcode_packet.h"

#define Script_RegClass(klass)	            gScript.regClass<klass>(#klass)
#define Script_RegClassFunc(klass, member)  gScript.regClassFunc<klass>(#member, &klass::member)
#define Script_RegClassVar(klass, member)   gScript.regClassVar<klass>(#member, &klass::member)

typedef lua_tinker::table luaTable;

class ScriptBase
{
public:
	ScriptBase()
        :_L(nullptr)
    {}
    virtual ~ScriptBase();

public:
    virtual bool init(const char *str) = 0;
    virtual bool reload() {return true;}

    //@brief: 默认的初始化，一般情况下调用这个就可以
    bool defaultInti(const char *str);

    bool dofile(const char *filename)
    {
      return lua_tinker::dofile( _L, filename);
    }

    //HACK 使用引用传递，
    template<typename T>
    void setglobal(const char* name, T object)
    {
      lua_tinker::setglobal(_L, name, object);
    }

    template<typename T>
    T getglobal(const char* name)
    {
      return lua_tinker::getglobal<T>(_L, name);
    }

    //导入全局函数
    template<typename F>
    void regFunc(const char *funcName, F func);

    //导入class
    template<typename Class>
    void regClass(const char *name) {lua_tinker::class_add<Class>(_L, name);}

    template<typename Class, typename F>
    void regClassFunc(const char* name, F func) {lua_tinker::class_def<Class>(_L, name, func);}

    template<typename Class, typename BASE, typename VAR>
    void regClassVar(const char* name, VAR BASE::*val) {lua_tinker::class_mem<Class>(_L, name, val);}

public:

    // Call Lua Func {
    template <typename R, typename T1>
    inline R callLua(const char *func, const T1& t1)
    {
      return lua_tinker::call<R, T1>(_L, func, t1);
    }

    template <typename R>
    inline R callLua(const char *func)
    {
      return lua_tinker::call<R>(_L, func);
    }



    template <typename R, typename T1, typename T2>
    inline R callLua(const char *func, const T1& t1,  const T2& t2)
    {
      return lua_tinker::call<R, T1, T2>(_L, func, t1, t2);
    }

    template <typename R, typename T1, typename T2, typename T3>
    inline R callLua(const char *func, const T1& t1,  const T2& t2, const T3& t3)
    {
      return lua_tinker::call<R, T1, T2, T3>(_L, func, t1, t2, t3);
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4>
    inline R callLua(const char *func, const T1& t1,  const T2& t2, const T3& t3, const T4& t4)
    {
      return lua_tinker::call<R, T1, T2, T3, T4>(_L, func, t1, t2, t3, t4);
    }
    // Call lua Func End}

protected:
    lua_State * _L;
    std::string _path;

};

enum LuaHostEngineErrorCode {
	LUA_HOST_ENGINE_SUCCESS = 0,
	LUA_HOST_ENGINE_NEW_STATE_FAIL,
	LUA_HOST_ENGINE_INIT_FAIL,
	LUA_HOST_ENGINE_LOAD_SCRIPT_FAIL,
};

class LuaHostEngine;

typedef bool (*LuaHostEngineInitCallback)(LuaHostEngine * engine, lua_State * l);

class LuaEngineService {
public:
	LuaEngineService() {};
	virtual ~LuaEngineService() {};

public:
	virtual int init(LuaHostEngine * engine, lua_State * l) = 0;
};

class LuaHostEngine {
public:
	LuaHostEngine(LuaHostEngineInitCallback callback = nullptr);
	virtual ~LuaHostEngine();

public:
	lua_State * getLuaState();
	virtual int reload();
	virtual int load(const char *file);

	void addService(LuaEngineService * service);
	void removeService(LuaEngineService * service);

protected:

	virtual lua_State * newState();
	virtual void deleteState(lua_State ** l);
	virtual bool init(lua_State * l);
	virtual bool dofile(lua_State* l, const char *file);

	inline lua_State * GetActualLuaState(lua_State * l) {
		if (l != nullptr) {
			return l;
		}
		return _L;
	}

protected:
	lua_State * _L;
	char * _path;

	LuaHostEngineInitCallback _callback;

	std::set<LuaEngineService*> _services;

public:

	//HACK 使用引用传递，
	template<typename T>
	void setglobal(const char* name, T object, lua_State * l = nullptr) {
		lua_tinker::setglobal(GetActualLuaState(l), name, object);
	}

	template<typename T>
	T getglobal(const char* name, lua_State * l = nullptr)
	{
		return lua_tinker::getglobal<T>(GetActualLuaState(l), name);
	}

	//导入全局函数
	template<typename F>
	inline void regFunc(const char *funcName, F func);

	template<typename F>
	inline void regFuncToState(const char *funcName, F func, lua_State * l);


	//导入class
	template<typename Class>
	void regClass(const char *name, lua_State * l = nullptr);

	template<typename Class, typename F>
	void regClassFunc(const char* name, F func, lua_State * l = nullptr) {
		lua_tinker::class_def<Class>(GetActualLuaState(l), name, func);
	}

	template<typename Class, typename BASE, typename VAR>
	void regClassVar(const char* name, VAR BASE::*val, lua_State * l = nullptr) {
		lua_tinker::class_mem<Class>(GetActualLuaState(l), name, val);
	}

	template<typename T>
	bool getRetValue(T* p)
	{
		assert(p);
		*p = (T)(*lua_touserdata(_L, -1));
		lua_pop(_L, 1);
		return true;
	}

public:
	// Call Lua Func {
	template <typename R>
	inline R callLua(const char *func)
	{
		return lua_tinker::call<R>(_L, func);
	}

	template <typename R, typename T1>
	inline R callLua(const char *func, const T1& t1)
	{
		return lua_tinker::call<R, T1>(_L, func, t1);
	}

	template <typename R, typename T1, typename T2>
	inline R callLua(const char *func, const T1& t1,  const T2& t2)
	{
		return lua_tinker::call<R, T1, T2>(_L, func, t1, t2);
	}

	template <typename R, typename T1, typename T2, typename T3>
	inline R callLua(const char *func, const T1& t1,  const T2& t2, const T3& t3)
	{
		return lua_tinker::call<R, T1, T2, T3>(_L, func, t1, t2, t3);
	}

	template <typename R, typename T1, typename T2, typename T3, typename T4>
	inline R callLua(const char *func, const T1& t1,  const T2& t2, const T3& t3, const T4& t4)
	{
		return lua_tinker::call<R, T1, T2, T3, T4>(_L, func, t1, t2, t3, t4);
	}

	template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
	inline R callLua(const char *func, const T1& t1,  const T2& t2, const T3& t3, const T4& t4, const T5& t5)
	{
		return lua_tinker::call<R, T1, T2, T3, T4, T5>(_L, func, t1, t2, t3, t4, t5);
	}

	inline void callLuaCDEL(const char* func)
	{
		lua_pushcclosure(_L, lua_tinker::on_error, 0);
		int errfunc = lua_gettop(_L);

		lua_getglobal(_L,func);
		if(lua_isfunction(_L,-1))
		{
			if(lua_pcall(_L, 0, 1, errfunc) != 0)
			{
				lua_pop(_L, 1);
			}
		}
		else
		{
			LOG(INFO) << "lua_tinker::call() attempt to call global '" << func <<"' (not a function)" ;
		}

		lua_remove(_L, -2);
	}

	template<typename T1>
	inline void callLuaCDEL(const char* func, const T1 &arg)
	{
		lua_pushcclosure(_L, lua_tinker::on_error, 0);
		int errfunc = lua_gettop(_L);

		lua_getglobal(_L,func);
		if(lua_isfunction(_L,-1))
		{
			lua_tinker::push(_L, arg);
			if(lua_pcall(_L, 1, 1, errfunc) != 0)
			{
				lua_pop(_L, 1);
			}
		}
		else
		{
			LOG(INFO) << "lua_tinker::call() attempt to call global '" << func << "' (not a function)";
		}

		lua_remove(_L, -2);
	}

	template<typename T1, typename T2>
	inline void callLuaCDEL(const char* func, const T1 &arg1, const T2 &arg2)
	{
		lua_pushcclosure(_L, lua_tinker::on_error, 0);
		int errfunc = lua_gettop(_L);

		lua_getglobal(_L, func);
		if(lua_isfunction(_L,-1))
		{
			lua_tinker::push(_L, arg1);
			lua_tinker::push(_L, arg2);
			if(lua_pcall(_L, 2, 1, errfunc) != 0)
			{
				lua_pop(_L, 1);
			}
		}
		else
		{
			LOG(INFO) << "lua_tinker::call() attempt to call global '" << func << "' (not a function)";
		}

		lua_remove(_L, -2);
	}

	template<typename T1, typename T2, typename T3>
	inline void callLuaCDEL(const char* func, const T1 &arg1, const T2 &arg2, const T3 &arg3)
	{
		lua_pushcclosure(_L, lua_tinker::on_error, 0);
		int errfunc = lua_gettop(_L);

		lua_getglobal(_L, func);
		if(lua_isfunction(_L,-1))
		{
			lua_tinker::push(_L, arg1);
			lua_tinker::push(_L, arg2);
			lua_tinker::push(_L, arg3);
			if(lua_pcall(_L, 2, 1, errfunc) != 0)
			{
				lua_pop(_L, 1);
			}
		}
		else
		{
			LOG(INFO) << "lua_tinker::call() attempt to call global '" << func << "' (not a function)";
		}

		lua_remove(_L, -2);
	}

	template<typename T1, typename T2, typename T3, typename T4>
	inline void callLuaCDEL(const char* func, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4)
	{
		lua_pushcclosure(_L, lua_tinker::on_error, 0);
		int errfunc = lua_gettop(_L);

		lua_getglobal(_L, func);
		if(lua_isfunction(_L,-1))
		{
			lua_tinker::push(_L, arg1);
			lua_tinker::push(_L, arg2);
			lua_tinker::push(_L, arg3);
			lua_tinker::push(_L, arg4);
			if(lua_pcall(_L, 2, 1, errfunc) != 0)
			{
				lua_pop(_L, 1);
			}
		}
		else
		{
			LOG(INFO) << "lua_tinker::call() attempt to call global '" << func << "' (not a function)";
		}

		lua_remove(_L, -2);
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	inline void callLuaCDEL(const char* func, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5)
	{
		lua_pushcclosure(_L, lua_tinker::on_error, 0);
		int errfunc = lua_gettop(_L);

		lua_getglobal(_L, func);
		if(lua_isfunction(_L,-1))
		{
			lua_tinker::push(_L, arg1);
			lua_tinker::push(_L, arg2);
			lua_tinker::push(_L, arg3);
			lua_tinker::push(_L, arg4);
			lua_tinker::push(_L, arg5);
			if(lua_pcall(_L, 2, 1, errfunc) != 0)
			{
				lua_pop(_L, 1);
			}
		}
		else
		{
			LOG(INFO) << "lua_tinker::call() attempt to call global '" << func << "' (not a function)";
		}

		lua_remove(_L, -2);
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	inline void callLuaCDEL(const char* name, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6)
	{
		lua_pushcclosure(_L, lua_tinker::on_error, 0);
		int errfunc = lua_gettop(_L);

		lua_getglobal(_L,name);
		if(lua_isfunction(_L,-1))
		{
			lua_tinker::push(_L, arg1);
			lua_tinker::push(_L, arg2);
			lua_tinker::push(_L, arg3);
			lua_tinker::push(_L, arg4);
			lua_tinker::push(_L, arg5);
			lua_tinker::push(_L, arg6);
			if(lua_pcall(_L, 6, 1, errfunc) != 0)
			{
				lua_pop(_L, 1);
			}
		}
		else
		{
			LOG(INFO) << "lua_tinker::call() attempt to call global '" << name << "' (not a function)";
		}

		lua_remove(_L, -2);
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	inline void callLuaCDEL(const char* name, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7)
	{
		lua_pushcclosure(_L, lua_tinker::on_error, 0);
		int errfunc = lua_gettop(_L);

		lua_getglobal(_L,name);
		if(lua_isfunction(_L,-1))
		{
			lua_tinker::push(_L, arg1);
			lua_tinker::push(_L, arg2);
			lua_tinker::push(_L, arg3);
			lua_tinker::push(_L, arg4);
			lua_tinker::push(_L, arg5);
			lua_tinker::push(_L, arg6);
			lua_tinker::push(_L, arg7);
			if(lua_pcall(_L, 7, 1, errfunc) != 0)
			{
				lua_pop(_L, 1);
			}
		}
		else
		{
			LOG(INFO) << "lua_tinker::call() attempt to call global '" << name << "' (not a function)" ;
		}

		lua_remove(_L, -2);
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	inline void callLuaCDEL(const char* name, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8)
	{
		lua_pushcclosure(_L, lua_tinker::on_error, 0);
		int errfunc = lua_gettop(_L);

		lua_getglobal(_L,name);
		if(lua_isfunction(_L,-1))
		{
			lua_tinker::push(_L, arg1);
			lua_tinker::push(_L, arg2);
			lua_tinker::push(_L, arg3);
			lua_tinker::push(_L, arg4);
			lua_tinker::push(_L, arg5);
			lua_tinker::push(_L, arg6);
			lua_tinker::push(_L, arg7);
			lua_tinker::push(_L, arg8);
			if(lua_pcall(_L, 8, 1, errfunc) != 0)
			{
				lua_pop(_L, 1);
			}
		}
		else
		{
			LOG(INFO) << "lua_tinker::call() attempt to call global '" << name << "' (not a function)";
		}

		lua_remove(_L, -2);
	}




	

};

//导入非lua_CFunction 类型的函数
template<typename F>
inline void ScriptBase::regFunc(const char *funcName, F func)
{
  lua_tinker::def<F>(_L, funcName, func);
}

//导入全局函数 warp for lua_register
template<>
inline void ScriptBase::regFunc<lua_CFunction>(const char *funcName, lua_CFunction func)
{
   lua_register(_L, funcName, func);
}

//导入全局函数
template<typename F>
inline void LuaHostEngine::regFunc(const char *funcName, F func) {
	lua_tinker::def<F>(_L, funcName, func);
}

template<typename F>
inline void LuaHostEngine::regFuncToState(const char *funcName, F func, lua_State * l) {
	lua_tinker::def<F>(GetActualLuaState(l), funcName, func);
}


template<>
inline void LuaHostEngine::regFunc<lua_CFunction>(const char *funcName, lua_CFunction func) {
	lua_register(_L, funcName, func);
}

template<>
inline void LuaHostEngine::regFuncToState<lua_CFunction>(const char *funcName, lua_CFunction func, lua_State * l) {
	lua_register(GetActualLuaState(l), funcName, func);
}

//template<typename T>
//inline bool LuaHostEngine::getRetValue(T* p)
//{
//	assert(p);
//	*p = (T)(*lua_touserdata(_L, -1));
//	lua_pop(_L, 1);
//	return true;
//}

template<>
inline bool LuaHostEngine::getRetValue<std::string>(std::string* pStr)
{
	assert(pStr);
	size_t uSize = 0;
	bool bRet = false;
	if (lua_isstring(_L, -1))
	{
		const char * respBuf = lua_tolstring(_L, -1, &uSize);
		*pStr = std::string(const_cast<char*>(respBuf), uSize);
		bRet = true;
	}
	lua_pop(_L, 1);
	return bRet;
}

template<>
inline bool LuaHostEngine::getRetValue<double>(double* p)
{
	assert(p);
	bool bRet = false;
	int isNum = 0;
	lua_Number number = lua_tonumberx(_L, -1, &isNum);
	if (isNum)
	{
		*p = number;
		bRet = true;
	}
	lua_pop(_L, 1);
	return bRet;
}

template<>
inline bool LuaHostEngine::getRetValue<int>(int* p)
{
	assert(p);
	bool bRet = false;
	int isInteger = 0;
	lua_Integer number = lua_tointegerx(_L, -1, &isInteger);
	if (isInteger)
	{
		*p = static_cast<int>(number);
		bRet = true;
	}
	lua_pop(_L, 1);
	return bRet;
}

template<>
inline bool LuaHostEngine::getRetValue<int64_t>(int64_t* p)
{
	assert(p);
	bool bRet = false;
	int isInteger = 0;
	lua_Integer number = lua_tointegerx(_L, -1, &isInteger);
	if (isInteger)
	{
		*p = (int64_t)number;
		bRet = true;
	}
	lua_pop(_L, 1);
	return bRet;
}

template<>
inline bool LuaHostEngine::getRetValue<uint32_t>(uint32_t* p)
{
	assert(p);
	bool bRet = false;
	int isInteger = 0;
	lua_Unsigned number = lua_tounsignedx(_L, -1, &isInteger);
	if (isInteger)
	{
		*p = (uint32_t)number;
		bRet = true;
	}
	lua_pop(_L, 1);
	return bRet;
}

template<>
inline bool LuaHostEngine::getRetValue<uint64_t>(uint64_t* p)
{
	assert(p);
	bool bRet = false;
	int isInteger = 0;
	lua_Unsigned number = lua_tounsignedx(_L, -1, &isInteger);
	if (isInteger)
	{
		*p = (uint64_t)number;
		bRet = true;
	}
	lua_pop(_L, 1);
	return bRet;
}

template<>
inline bool LuaHostEngine::getRetValue<lua_CFunction>(lua_CFunction* p)
{
	assert(p);
	bool bRet = false;
	int isFunc = lua_iscfunction(_L, -1);
	if (isFunc)
	{
		*p = *lua_tocfunction(_L, -1);
		bRet = true;
	}
	lua_pop(_L, 1);
	return bRet;
}

template<>
inline bool LuaHostEngine::getRetValue<PacketTranscode>(PacketTranscode * p)
{
	assert(p);
	size_t uSize = 0;
	bool bRet = false;
	if (lua_isstring(_L, -1))
	{
		const char *respBuf = lua_tolstring(_L, -1, &uSize);

		//p->Append(respBuf, uSize);
		p->Write(p->buf_, reinterpret_cast<const uint8_t*>(respBuf), uSize);

		bRet = true;
	}
	lua_pop(_L, 1);
	return bRet;
}


#endif  // FRAMEWORK_BASE_SCRIPT_BASE_HPP_
