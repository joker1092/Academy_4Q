#pragma once
#include "Core.Minimal.h"

interface IComponent
{
	virtual void Update(float tick) = 0;
};
