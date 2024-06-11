#include "Model.h"

namespace libCore
{

	void Model::Draw(const std::string& shader)
	{
		//Bind Textures
		for (unsigned int i = 0; i < materials.size(); i++)
		{
			materials[i]->albedoMap->Bind(shader);
			//materials[i]->normalMap->Bind(shader);
			//materials[i]->metallicMap->Bind(shader);
			//materials[i]->rougnessMap->Bind(shader);
			//materials[i]->aOMap->Bind(shader);
		}
		
		//Draw
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			libCore::ShaderManager::Get(shader)->setMat4("model", transform.getMatrix());
			meshes[i]->Draw();
		}
	}
	
}
