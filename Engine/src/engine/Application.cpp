#include "Application.h"

void feApplication::Start()
{
	if (m_Running) return;
	Run();
}

void feApplication::Stop()
{
	m_Running = false;
}

bool feApplication::IsRunning() const
{
	return m_Running;
}

double feApplication::GetDeltaTime() const
{
	return m_DeltaTime;
}

void feApplication::Init()
{
}

void feApplication::Update()
{
}

void feApplication::Destroy()
{
}

void feApplication::Run()
{
	m_Running = true;

	Init();

	double lastTime = GetTime();
	double currentTime;

	while (m_Running)
	{
		currentTime = GetTime();
		m_DeltaTime = currentTime - lastTime;
		lastTime = currentTime;

		Update();
	}

	Destroy();
}