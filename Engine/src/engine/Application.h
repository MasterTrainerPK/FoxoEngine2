#pragma once

class feApplication
{
public:
	// To be defined in the client api
	static feApplication* CreateInstance();
	static void DeleteInstance(feApplication* application);
public:
	virtual ~feApplication() noexcept = default;

	void Start();
	void Stop();
	bool IsRunning() const;
	double GetDeltaTime() const;
protected:
	virtual void Init();
	virtual void Update();
	virtual void Destroy();
	virtual double GetTime() = 0;
private:
	void Run();
private:
	bool m_Running = false;
	double m_DeltaTime = 1.0;
};