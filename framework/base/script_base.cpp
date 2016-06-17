/*
 * script_base.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: jszhang
 */

#include "script_base.hpp"

extern "C" {
	LUALIB_API int luaL_openlib_struct (lua_State *L);
};


ScriptBase::~ScriptBase()
{
  if (_L != nullptr) lua_close( _L );
    _L = nullptr;
}

bool ScriptBase::defaultInti( const char *path )
{
    _L = luaL_newstate();
    if (_L == nullptr)
      return false;
    luaL_openlibs( _L );

    lua_tinker::init(_L); //support 64
    _path = path;
    return dofile(path);
}

LuaHostEngine::LuaHostEngine(LuaHostEngineInitCallback callback /* = nullptr */) :
	_L(nullptr)
	, _path(nullptr)
	, _callback(callback) {
}

LuaHostEngine::~LuaHostEngine() {
	if (_L != nullptr) {
		deleteState(&_L);
	}

	if (_path != nullptr) {
		free(_path);
		_path = nullptr;
	}
}

lua_State * LuaHostEngine::newState() {
	lua_State * l = luaL_newstate();
	if (l == nullptr) {
		return nullptr;
	}

	luaL_openlibs(l);
	luaL_openlib_struct(l);

	lua_tinker::init(l); //support 64

	return l;
}

void LuaHostEngine::deleteState(lua_State ** l) {
	if (l != nullptr && *l != nullptr) {
		lua_close(*l);
		*l = nullptr;
	}
}

lua_State * LuaHostEngine::getLuaState() {
	return _L;
}

bool LuaHostEngine::init(lua_State * l) {
	if (_callback) {
		return _callback(this, l);
	}

	for (std::set<LuaEngineService*>::iterator it = _services.begin(); it != _services.end(); ++it) {
		if ((*it)->init(this, l) != LUA_HOST_ENGINE_SUCCESS) {
			return false;
		}
	}

	return true;
}

bool LuaHostEngine::dofile(lua_State* l, const char *file) {
	return lua_tinker::dofile(l, file);
}

int LuaHostEngine::reload() {
	lua_State * l = newState();
	if (l == nullptr) {
		return LUA_HOST_ENGINE_NEW_STATE_FAIL;
	}

	if (init(l) == false) {
		deleteState(&l);
		return LUA_HOST_ENGINE_INIT_FAIL;
	}

	if (_path != nullptr) {
		if (dofile(l, _path) == false) {
			deleteState(&l);
			return LUA_HOST_ENGINE_LOAD_SCRIPT_FAIL;
		}
	}

	if (_L != nullptr) {
		deleteState(&_L);
	}
	LOG(INFO) << "[LuaHostEngine::reload]     l=" << l;
	_L = l;

	return LUA_HOST_ENGINE_SUCCESS;
}

int LuaHostEngine::load(const char *file) {
	if (_path != nullptr) {
		free(_path);
		_path = nullptr;
	}

	size_t len = strlen(file) + 1;
	_path = reinterpret_cast<char*>(calloc(len, sizeof(*_path)));
	memcpy(_path, file, len);

	return reload();
}

void LuaHostEngine::addService(LuaEngineService * service) {
	_services.insert(service);
}

void LuaHostEngine::removeService(LuaEngineService * service) {
	_services.erase(service);
}

//template<typename F>
//void LuaHostEngine::regFunc(const char *funcName, F func, lua_State * l) {
//	lua_tinker::def<F>(GetActualLuaState(l), funcName, func);
//}
//
//template<>
//void LuaHostEngine::regFunc<lua_CFunction>(const char *funcName, lua_CFunction func) {
//	lua_register(_L, funcName, func);
//}
//
//template<>
//void LuaHostEngine::regFunc<lua_CFunction>(const char *funcName, lua_CFunction func, lua_State * l) {
//		lua_register(GetActualLuaState(l), funcName, func);
//}




