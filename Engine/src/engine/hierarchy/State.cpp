#include "State.h"

void feState::Init()
{
}

void feState::Update()
{
}

void feState::Destroy()
{
}

feStateManager* feState::GetStateManager() const
{
	return m_StateManager;
}

feStateManager::~feStateManager() noexcept
{
	Reset();
}

void feStateManager::Reset()
{
	if (m_State)
	{
		m_State->Destroy();
		m_State->m_StateManager = nullptr;
		m_State = nullptr;
	}
}

void feStateManager::Update() const
{
	if (m_State) m_State->Update();
}

void* feStateManager::GetUserPtr() const
{
	return m_UserPtr;
}

void feStateManager::SetUserPtr(void* userPtr)
{
	m_UserPtr = userPtr;
}