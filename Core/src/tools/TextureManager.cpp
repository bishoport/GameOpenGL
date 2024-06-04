#include "TextureManager.h"

namespace libCore
{
    Ref<Texture> TextureManager::LoadTexture(const char* imagePath, TEXTURE_TYPES type, GLuint slot)
    {
        // Intenta obtener la textura desde el AssetsManager
        auto textureFromAssets = AssetsManager::GetInstance().GetTexture(imagePath);
        if (textureFromAssets) 
        {
            //std::cout << "LA TEXTURA " << imagePath << " YA EXISTE EN ASSETS" << std::endl;
            return textureFromAssets;  // Retorna la textura si ya está cargada
        }

        //Cargamos la textura si no existe en la biblioteca:
        std::cout << "Loading Texture->" << imagePath << std::endl;
        auto texture = CreateRef<Texture>(imagePath, type, slot);
        AssetsManager::GetInstance().SetTexture(imagePath, texture);
        return texture;
    }
}
