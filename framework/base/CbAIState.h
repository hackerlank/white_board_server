#ifndef FRAMEWORK_BASE_CBAISTATE_H_
#define FRAMEWORK_BASE_CBAISTATE_H_

#include "CbCommonInclude.h"

template <typename T>
class CbAIState
{
public:
  explicit CbAIState(int32_t nType)
  {
	  m_nType	=	nType; 
  }

  int32_t	  getAIStateType()
  {
	  return m_nType; 
  }

  virtual ~CbAIState(){}

  //this will execute when the state is entered
  virtual void enter(T*)=0;

  //this is the states normal update function
  virtual void execute(T*)=0;

  //this will execute when the state is exited. 
  virtual void exit(T*)=0;

  //this executes if the agent receives a message from the 
  //message dispatcher
  virtual bool onMessage(T*, void* pPacket)=0;

  //state type
  int32_t m_nType;
};

#endif  // FRAMEWORK_BASE_CBAISTATE_H_
