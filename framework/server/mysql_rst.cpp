#include "mysql_rst.h"



MysqlRst::MysqlRst(MYSQL_RES* res, uint32_t FieldCount, uint32_t RowCount) : nFieldCount_(FieldCount), nRowCount_(RowCount), pResult_( res )
{
	if( res == nullptr || FieldCount == 0 )
	{
		pCurrentRow_	= nullptr;
		pFields_		= nullptr;
		nRowCount_		= 0;
	}else{
		pCurrentRow_	= new Field[FieldCount];
		pFields_		= mysql_fetch_fields(res);
	}
}

MysqlRst::~MysqlRst()
{
	if(pResult_) mysql_free_result(pResult_);
	delete [] pCurrentRow_;
}

bool MysqlRst::NextRow()
{
	if( nRowCount_ <= 0 )
		return false;

	MYSQL_ROW row = mysql_fetch_row(pResult_);
	if(row == nullptr)
		return false;

	for(uint32_t i = 0; i < nFieldCount_; ++i)
		pCurrentRow_[i].SetValue(row[i]);

	return true;
}


std::string  MysqlRst::GetFieldName(uint32_t iCol)
{
	if ( iCol > (uint32_t)1<<31 || iCol >= nFieldCount_)
	{
		return INDEX_ERROR;
	}
	return pFields_[iCol].name;
}

const Field& MysqlRst::operator[](uint32_t iCol) const
{
	assert(pCurrentRow_&& iCol<= nFieldCount_);
	return pCurrentRow_[iCol];
}
