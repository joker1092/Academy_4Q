#pragma once
#include "../Academy_4Q/Utility_Framework/DeviceResources.h"
#include "D2DObjScene.h"
#include <memory>
#include <vector>
namespace DirectX11 {


	class D2DRenderer
	{
	public:
		D2DRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);

	private:
		std::vector<D2DObjScene>			m_D2DObjVec;
		std::shared_ptr<DeviceResources>	m_DeviceResources;

	};
}



