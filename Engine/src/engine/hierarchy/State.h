#pragma once

#include <memory>

class feStateManager;

class feState
{
public:
	feState() = default;
	virtual ~feState() noexcept = default;

	virtual void Init();
	virtual void Update();
	virtual void Destroy();

	feStateManager* GetStateManager() const;
private:
	feStateManager* m_StateManager = nullptr;

	friend class feStateManager;
};

class feStateManager final
{
public:
	~feStateManager() noexcept;

	void Reset();

	template<typename T, typename... Args>
	void SetState(Args&&...args)
	{
		if (m_State)
		{
			m_State->Destroy();
			m_State->m_StateManager = nullptr;
		}

		m_State = std::make_unique<T>(std::forward<Args>(args)...);

		if (m_State)
		{
			m_State->m_StateManager = this;
			m_State->Init();
		}
	}

	void Update() const;

	void* GetUserPtr() const;

	template<typename T>
	T* GetUserPtr() const
	{
		return static_cast<T*>(m_UserPtr);
	}

	void SetUserPtr(void* userPtr);
	
private:
	std::unique_ptr<feState> m_State = nullptr;
	void* m_UserPtr = nullptr;
};