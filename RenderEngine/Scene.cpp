#include "Scene.h"

Scene::~Scene()
{
	instancedKeys.clear();
	billboardInstancedKeys.clear();
	ModelsData.clear();
	billboards.clear();
}
