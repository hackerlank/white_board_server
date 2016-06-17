#ifndef FRAMEWORK_BASE_CBSTATEMACHINE_H_
#define FRAMEWORK_BASE_CBSTATEMACHINE_H_

#include "CbAIState.h"


template <typename  T>
class CbStateMachine
{
private:

  //a pointer to the agent that owns this instance
  T*						m_pOwner;

  CbAIState<T>*				m_pCurrentState;
  
  //a record of the last state the agent was in
  CbAIState<T>*				m_pPreviousState;

  //this is called every time the FSM is updated
  CbAIState<T>*				m_pGlobalState;
  

public:

  explicit CbStateMachine(T* owner):m_pOwner(owner),
                                   m_pCurrentState(NULL),
                                   m_pPreviousState(NULL),
                                   m_pGlobalState(NULL)
  {}

  virtual ~CbStateMachine(){}

  //use these methods to initialize the FSM
  void setCurrentState(CbAIState<T>* s){m_pCurrentState = s;}
  void setGlobalState(CbAIState<T>* s) {m_pGlobalState = s;}
  void setPreviousState(CbAIState<T>* s){m_pPreviousState = s;}
  
  //call this to update the FSM
  void  update()const
  {
    //if a global state exists, call its execute method, else do nothing
    if(m_pGlobalState)   m_pGlobalState->execute(m_pOwner);

    //same for the current state
    if (m_pCurrentState) m_pCurrentState->execute(m_pOwner);
  }

  bool  handleMessage(void* pPacket)const
  {
    //first see if the current state is valid and that it can handle
    //the message
    if (m_pCurrentState && m_pCurrentState->onMessage(m_pOwner, pPacket))
    {
		 return true;
    }
  
    //if not, and if a global state has been implemented, send 
    //the message to the global state
    if (m_pGlobalState && m_pGlobalState->onMessage(m_pOwner, pPacket))
    {
		 return true;
    }
    return false;
  }

  //change to a new state
  void  changeState(CbAIState<T>* pNewState)
  {
 
    //keep a record of the previous state
    m_pPreviousState = m_pCurrentState;

    //call the exit method of the existing state
    m_pCurrentState->exit(m_pOwner);

    //change state to the new state
    m_pCurrentState = pNewState;

    //call the entry method of the new state
    m_pCurrentState->enter(m_pOwner);
  }

  //change state back to the previous state
  void  revertToPreviousState()
  {
		changeState(m_pPreviousState);
  }

  //returns true if the current state's type is equal to the type of the
  //class passed as a parameter. 
  bool  isInState(const CbAIState<T>& st)const
  {
    if (typeid(*m_pCurrentState) == typeid(st)) return true;
    return false;
  }

  CbAIState<T>*  currentState()  const{return m_pCurrentState;}
  CbAIState<T>*  globalState()   const{return m_pGlobalState;}
  CbAIState<T>*  previousState() const{return m_pPreviousState;}

  //only ever used during debugging to grab the name of the current state
  std::string getNameOfCurrentState()const
  {
//    std::string s(typeid(*m_pCurrentState).name());
//
//    //remove the 'class ' part from the front of the string
//    if (s.size() > 5)
//    {
//      s.erase(0, 6);
//    }
    return std::string();
  }
};




#endif  // FRAMEWORK_BASE_CBSTATEMACHINE_H_


