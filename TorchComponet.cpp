#include "TorchComponet.h"
#include "Object.h"
#include "EventSystem.h"
#include "InputManager.h"

TorchComponet::TorchComponet(Scene* scene) :
	_targetScene(scene),
	_instancedFireLoopBillboard(nullptr)
{
	_id = UINT32_MAX - 2;

	//Event->Subscribe("TorchOn"+_owner->GetName(), [&](void* owner) {
	//	FireOn();
	//});
	//Event->Subscribe("TorchOff" + _owner->GetName(), [&](void* owner) {
	//	FireOff();
	//});
}


void TorchComponet::Update(float tick)
{
	if(!_isSubscribe)
	{
		Event->Subscribe("TorchOn" + _owner->GetName(), [&](void* owner) {
			FireOn();
			});
		Event->Subscribe("TorchOff" + _owner->GetName(), [&](void* owner) {
			FireOff();
			});
		_isSubscribe = true;
	}

	float3 localPos = _owner->GetLocalPosition();
	Mathf::xMatrix world = _owner->GetWorldMatrix();
	Mathf::xVector localPosV = XMLoadFloat3(&localPos);
	Mathf::xVector worldPosV = XMVector3Transform(localPosV, world);
	Mathf::Vector3 worldPos;
	XMStoreFloat3(&worldPos, worldPosV);
	if (!_instancedFireInBillboard || !_instancedFireLoopBillboard) return;
	_instancedFireInBillboard->Center = worldPos + _centerOffset;
	_instancedFireLoopBillboard->Center = worldPos + _centerOffset;
	_instancedFireInBillboard->Size = size;
	_instancedFireLoopBillboard->Size = size;

	prevTime = _StateDuration;
	
	_AccumulatedTime += tick;
	
	
	// update
	// 3가지 상태 fade in, ing, fade out
	switch (_state)
	{
	case None:
		_instancedFireInBillboard->_isVisible = false;
		_instancedFireLoopBillboard->_isVisible = false;
		break;
	case FireIn:
		FireInUpdate();
		break;
	case FireLoop:
		FireLoopUpdate();
		break;
	case FireOut:
		FireOutUpdate();
		break;
	}
}

void TorchComponet::EditorContext()
{
	ImGui::Text("TorchComponet");
	auto billboard = AssetsSystem->GetPayloadBillboard();
	if (ImGui::Button("Set Begin"))
	{
		ImGui::OpenPopup("Set Begin");
	}

	if (ImGui::BeginPopup("Set Begin"))
	{
		ImGui::Text("Please Click the desired Billboard into the content window.");
		if (billboard && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
		{
			std::string instancedName = billboard->name + std::to_string(++f_id);
			_instancedFireInBillboard = _targetScene->AddBillboard(instancedName, billboard);
			_instancedID = _instancedFireInBillboard->InstancedID;
			AssetsSystem->ClearPayloadBillboard();
		}
		ImGui::EndPopup();
	}

	if (_instancedFireInBillboard)
	{
		//_instancedFireInBillboard = _targetScene->GetBillboardPtr(_instancedID);
		ImGui::Selectable(_instancedFireInBillboard->_billboard->name.c_str(), false);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			_targetScene->RemoveBillboard(_instancedFireInBillboard->_instancedName);
			_instancedFireInBillboard = nullptr;
			return;
		}
	}

	if (ImGui::Button("Set Loop"))
	{
		ImGui::OpenPopup("Set Loop");
	}

	if (ImGui::BeginPopup("Set Loop"))
	{
		ImGui::Text("Please Click the desired Billboard into the content window.");
		if (billboard && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
		{
			std::string instancedName = billboard->name + std::to_string(++f_id);
			_instancedFireLoopBillboard = _targetScene->AddBillboard(instancedName, billboard);
			_instancedID = _instancedFireLoopBillboard->InstancedID;
			AssetsSystem->ClearPayloadBillboard();
		}
		ImGui::EndPopup();
	}

	if (_instancedFireLoopBillboard)
	{
		//_instancedFireLoopBillboard = _targetScene->GetBillboardPtr(_instancedID);
		ImGui::Selectable(_instancedFireLoopBillboard->_billboard->name.c_str(), false);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			_targetScene->RemoveBillboard(_instancedFireLoopBillboard->_instancedName);
			_instancedFireLoopBillboard = nullptr;
			return;
		}
	}

	{
		ImGui::Spacing();
		ImGui::Text("Billboards Inspector");
		ImGui::DragFloat3("Center", &_centerOffset.x, 0.1f);
		ImGui::DragFloat("Size", &size, 0.1f);
	}

	{
		ImGui::Text("Animation Inspector");
		ImGui::DragFloat("Frame Time", &_frameTime, 0.1f);
		ImGui::DragFloat("State Duration", &_StateDuration, 0.1f, 0.f, 60.f);
	}

	if (ImGui::Button("Test Start"))
	{
		FireOn();
	}

	if (ImGui::Button("Test Off"))
	{
		FireOff();
	}

}

void TorchComponet::Serialize(_inout json& out)
{
	json data;

	data["Billboard"]["Begin"]["Key"] = _instancedFireInBillboard->_billboard->name;
	data["Billboard"]["Begin"]["InstancedName"] = _instancedFireInBillboard->_instancedName;
	data["Billboard"]["Loop"]["Key"] = _instancedFireLoopBillboard->_billboard->name;
	data["Billboard"]["Loop"]["InstancedName"] = _instancedFireLoopBillboard->_instancedName;
	data["Billboard"]["Center"] =
	{
		_centerOffset.x, _centerOffset.y, _centerOffset.z
	};
	data["Billboard"]["size"] = size;
	data["Billboard"]["FrameRate"] = _frameTime;
	data["StateDuration"] = _StateDuration;
	data["Type"] = MetaType<TorchComponet>::type;

	out["components"].push_back(data);
}

void TorchComponet::DeSerialize(_in json& in)
{
	_isDeSerialize = false;
	std::string instancedName = in["Billboard"]["Begin"]["InstancedName"].get<std::string>();
	_instancedFireInBillboard = _targetScene->AddBillboard(
		instancedName,
		AssetsSystem->Billboards[in["Billboard"]["Begin"]["Key"].get<std::string>()]
	);
	_instancedFireLoopBillboard = _targetScene->AddBillboard(
		in["Billboard"]["Loop"]["InstancedName"].get<std::string>(),
		AssetsSystem->Billboards[in["Billboard"]["Loop"]["Key"].get<std::string>()]
	);
	_instancedID = _instancedFireLoopBillboard->InstancedID;
	_centerOffset = Mathf::jsonToVector3(in["Billboard"]["Center"]);
	if (in["Billboard"].contains("FrameRate"))
	{
		_frameTime = in["Billboard"]["FrameRate"].get<float>();
	}
	if (in.contains("StateDuration"))
	{
		_StateDuration = in["StateDuration"].get<float>();
	}
	if (!in["Billboard"]["size"].empty()) {
		size = in["Billboard"]["size"].get<float>();
	}
	_isDeSerialize = true;
}

void TorchComponet::FireOn()
{
	_instancedFireInBillboard->_isVisible = true;
	_instancedFireLoopBillboard->_isVisible = false;
	_state = StateFire::FireIn;
}

void TorchComponet::FireOff()
{
	_state = StateFire::FireOut;
}


void TorchComponet::FireInUpdate()
{
	
	_instancedFireInBillboard->_isVisible = true;
	if (++_instancedFireInBillboard->currentTexture >= _instancedFireInBillboard->_billboard->Textures.size())
	{
		_state = FireLoop;
		_instancedFireLoopBillboard->_isVisible = true;
		_instancedFireInBillboard->currentTexture = _instancedFireInBillboard->_billboard->Textures.size() - 1;
	}
	
}

void TorchComponet::FireLoopUpdate()
{
		_instancedFireInBillboard->_isVisible = false;
		++_instancedFireLoopBillboard->currentTexture;
		_instancedFireLoopBillboard->currentTexture %= _instancedFireLoopBillboard->_billboard->Textures.size() - 1;	
}

void TorchComponet::FireOutUpdate()
{
	
	_instancedFireInBillboard->_isVisible = true;
	if (_instancedFireInBillboard->currentTexture-- <= 0)
	{
		_state = None;
		_instancedFireInBillboard->_isVisible = false;
		_instancedFireLoopBillboard->_isVisible = false;
		_instancedFireInBillboard->currentTexture = 0;
		_isStart = false;
	}
	
}
