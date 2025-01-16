//게임 월드의 인터페이스
#pragma once
#include "Core.Minimal.h"
interface IWorld 
{
	virtual void Initialize() = 0;
	virtual void Update(float tick) = 0;
	virtual void Render() = 0;
	virtual void Destroy() = 0;
};

