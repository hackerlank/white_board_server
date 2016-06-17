#ifndef FRAMEWORK_SERVER_MYSQL_RST_H_
#define FRAMEWORK_SERVER_MYSQL_RST_H_

#include <inttypes.h>
#include <assert.h>

#include <string>

#ifdef __APPLE__
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#define INDEX_ERROR "Index Error!"

class Field
{
public:
	Field():mValue_(nullptr) {}
	~Field() {}
	const std::string GetString() const {
		if(!mValue_)
			return "";
		std::string reValue;
		reValue = mValue_;
		if("(null)"== reValue)
			return "";
		return reValue;
	}
	float GetFloat()const	{ return mValue_ ? static_cast<float>(atof(mValue_))	 : 0; }
	bool GetBool()const		{ return mValue_ ? atoi(mValue_) > 0 : false; }
	uint8_t GetUInt8()const	{ return mValue_ ? static_cast<uint8_t>(atoll(mValue_)) : 0; }
	int8_t GetInt8()const		{ return mValue_ ? static_cast<int8_t>(atoll(mValue_)) : 0; }
	uint16_t GetUInt16()const { return mValue_ ? static_cast<uint16_t>(atoll(mValue_)) : 0; }
	int16_t GetInt16()const	{ return mValue_ ? static_cast<int16_t>(atoll(mValue_)) : 0; }
	uint32_t GetUInt32()const { return mValue_ ? static_cast<uint32_t>(atoll(mValue_)) : 0; }
	int32_t GetInt32() const	{ return mValue_ ? static_cast<int32_t>(atoll(mValue_)) : 0; }
	int64_t GetInt64() const	{ return mValue_ ? static_cast<int64_t>(atoll(mValue_)) : 0; }
	uint64_t GetUInt64()const
	{
		if(mValue_)
		{
			uint64_t value;
            #ifdef __linux__
            sscanf(mValue_, "%20lu", &value);
            #else
			sscanf(mValue_, "%20" PRIu64, &value);
            #endif
			return value;
		} else {
			return 0;
		}
	}

private:
	friend class MysqlRst;

	void SetValue(char* value) { mValue_ = value; }

	char *mValue_;
};

class MysqlRst
{
	friend class MysqlRstPtr;
public:
	MysqlRst(MYSQL_RES* res, uint32_t FieldCount, uint32_t RowCount);

	bool NextRow();

	Field* Fetch() { return pCurrentRow_; }
	uint32_t GetFieldCount() const { return nFieldCount_; }
	uint32_t GetRowCount() const {
		return nRowCount_;
	}
	std::string GetFieldName(uint32_t iCol);
	const Field& operator[](uint32_t iCol) const;
protected:

	~MysqlRst();
	void Delete() { delete this; }
protected:
	uint32_t nFieldCount_;
	uint32_t nRowCount_;
    Field * pCurrentRow_;
	MYSQL_RES* pResult_;
	MYSQL_FIELD* pFields_;
};

class MysqlRstPtr
{
public:
	explicit MysqlRstPtr(MysqlRst* rst = nullptr) : rst_(rst)
	{
	};
	MysqlRstPtr(const MysqlRstPtr& rst) : rst_(nullptr)
	{
		*this = rst;
	};

	~MysqlRstPtr()
	{
		Delete();
	};
	MysqlRstPtr & operator = (const MysqlRstPtr& arst)
	{
		Delete();
		rst_ = arst.rst_;
		arst.rst_ = nullptr;
		return *this;
	};
	MysqlRst * operator -> ()
	{
		assert( rst_ != nullptr );
		return rst_;
	};
	const MysqlRst * operator -> () const
	{
		assert( rst_ != nullptr );
		return rst_;
	};
	MysqlRst & operator*() const
	{
		assert( rst_ != nullptr );
		return *rst_;
	}
	MysqlRst * Get()
	{
		return rst_;
	};
	operator bool () const
	{
		return rst_ != nullptr;
	};
	bool operator! () const // never throws
	{
		return rst_ == nullptr;
	};
public:
	void Delete()
	{
		if (rst_)
		{
			rst_->Delete();
			rst_ = nullptr;
		}
	};
private:
	mutable MysqlRst * rst_;
};

#endif  // FRAMEWORK_SERVER_MYSQL_RST_H_
