#pragma once
#include "Utility_Framework/IWorld.h"

class TestWorld : public IWorld
{
public:
	TestWorld() = default;
	~TestWorld() = default;
	void Initialize() override;
	void Update(float tick) override;
	void Render() override;
	void Destroy() override;

private:
	
};