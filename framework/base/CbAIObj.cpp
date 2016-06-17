#include "CbAIObj.h"

void CbAIObj::updateAIState()
{
	m_pStateMachine->update();
}

void CbAIObj::handleMessage(void* pPacket)
{
	m_pStateMachine->handleMessage(pPacket); 
}

CbAIObj::CbAIObj()
{
	m_pStateMachine = NULL; 
}
CbAIObj::~CbAIObj()
{
	if  (m_pStateMachine != NULL)
	{
		delete m_pStateMachine;
		m_pStateMachine = NULL; 
	}

	for (map<int32_t, CbAIState<CbAIObj>* >::iterator iter = m_stateMap.begin(); iter != m_stateMap.end(); ++iter)
	{
		delete  iter->second; 
	}
	m_stateMap.clear(); 
}

CbAIState<CbAIObj>* CbAIObj::findAIState(int32_t nType)
{
	map<int32_t, CbAIState<CbAIObj>*  >::iterator iter = m_stateMap.find(nType);
	if  (iter  != m_stateMap.end())
	{
		return  iter->second; 
	}
	else
	{
		return NULL; 
	}
}

//外部改变AI状态
int32_t		CbAIObj::changeAIState(int32_t nStateType)
{
	CbAIState<CbAIObj>* pState		=	this->findAIState(nStateType);
	if  (pState != NULL)
	{
		this->getFSM()->changeState(pState);
		return  0; 
	}
	else
	{
		return -1; 
	}
}


void		CbAIObj::initAIState()
{

}
void		CbAIObj::addAIState(int32_t nType)
{

}

void     	CbAIObj::setCurrentAIState()
{	

}
void		CbAIObj::setGlobalAIState()
{

}
