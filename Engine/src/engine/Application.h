#pragma once

class feApplication
{
public:

	///
	/// Should create an instance of your application
	/// @returns pointer to your application instance
	static feApplication* CreateInstance();
	///
	/// Should clean up and delete your application instance
	/// @param aplication: pointer the instance to delete
	static void DeleteInstance(feApplication* application);
public:
	virtual ~feApplication() noexcept = default;

	void Start();
	void Stop();
	bool IsRunning() const;
	/// Gets the frame time
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