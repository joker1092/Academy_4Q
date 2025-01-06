#pragma once
#include "Core.Minimal.h"
#include "IPipelineStateObject.h"
#include "StateDevice.h"

class MeshBasedPSO : public IPipelineStateObject
{
public:
    MeshBasedPSO(StateDevice* deviceResources);
    ~MeshBasedPSO() override = default;

private:
    ID3D11Device* m_d3dDevice{};
    ID3D11DeviceContext* m_d3dDeviceContext{};
};
