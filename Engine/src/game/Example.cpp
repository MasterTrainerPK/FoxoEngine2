// Example of how to create an application
#if 0

#include "../engine/Application.h"

class Example : public feApplication
{
public:
	virtual ~Example() noexcept = default;

	virtual void Init() override
	{
		
	}

	virtual void Destroy() override
	{
		
	}

	virtual void Update() override
	{
		
	}

	virtual double GetTime() override
	{
		return 0;
	}
};

feApplication* feApplication::CreateInstance()
{
	return new Example;
}

void feApplication::DeleteInstance(feApplication* application)
{
	delete application;
}

#endif