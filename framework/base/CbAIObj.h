#ifndef	FRAMEWORK_BASE_CBAIOBJ_H_
#define FRAMEWORK_BASE_CBAIOBJ_H_


#include  "CbCommonInclude.h"
#include  "CbStateMachine.h"

using namespace std;

class  CbAIObj 
{
public:
	CbAIObj() ;  
	virtual						~CbAIObj(); 
	void						updateAIState();
	void						handleMessage(void* pPacket); 

	//�ⲿ�ı�AI״̬
	int32_t						changeAIState(int32_t nStateType);

	int32_t						getCurrentAIStateType()
	{
		return getFSM()->currentState()->getAIStateType(); 
	}

	CbAIState<CbAIObj>*			getCurrentAIState()
	{
		return getFSM()->currentState(); 
	}

	CbAIState<CbAIObj>*			findAIState(int32_t nType);

	CbStateMachine<CbAIObj>*	getFSM()
	{
		return m_pStateMachine;
	}

protected:
	void						initAIState(); 
	void						setCurrentAIState();
	void						setGlobalAIState();
	void						addAIState(int32_t nType);

	//an instance of the state machine class
	CbStateMachine<CbAIObj>*					m_pStateMachine;
	map<int32_t, CbAIState<CbAIObj>* >			m_stateMap;
};


#endif  // FRAMEWORK_BASE_CBAIOBJ_H_
