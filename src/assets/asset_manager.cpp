#include "assets/asset_manager.h"

#include <SDL_image.h>

bool AssetManager::load_texture(SDL_Renderer* renderer, const std::string& name, const std::string& path){
    // Prevents loading two textures with the same name
    if (texture_index_by_name_.contains(name)){
        SDL_Log("Texture alreaddy loaded: %s", name.c_str());
        return false;
    }

    // IMG_Load loads image data from disk into CPU memory as an SDL_Surface
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface){
        SDL_Log("Failed to load texture image '%s': %s", path.c_str(), IMG_GetError());
        return false;
    }

    // Converts the surface into a texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Gets rid of surface
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Failed to create SDL_Texture from '%s': %s", path.c_str(), SDL_GetError());
        return false;
    }

    // Stores the texture and remembers where it lives in the vector
    texture_index_by_name_[name] = textures_.size();
    textures_.push_back(texture);

    return true;
}

bool AssetManager::load_font(const std::string& name, const std::string& path, int size){
    // Prevents loading two fonts with the same name
    if (font_index_by_name_.contains(name)){
        SDL_Log("Font already loaded: %s", name.c_str());
        return false;
    }

    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (!font){
        SDL_Log("Failed to load font '%s': %s", path.c_str(), TTF_GetError());
        return false;
    }

    // Stores the font and remembers where it lives in the vectr
    font_index_by_name_[name] = fonts_.size();
    fonts_.push_back(font);

    return true;
}

bool AssetManager::load_sound(const std::string& name, const std::string& path){
    if (sound_index_by_name_.contains(name)){
        SDL_Log("Sound already loaded: %s", name.c_str());
        return false;
    }

    // Mix_LoadWAV is for short sound effects
    // Later, music should use Mix_Music instead of Mix_Chunk
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());

    if (!sound){
        SDL_Log("Failed to load sound '%s': %s", path.c_str(), Mix_GetError());
        return false;
    }

    // Stores the sound and remembers where it lives in the vector
    sound_index_by_name_[name] = sounds_.size();
    sounds_.push_back(sound);

    return true;
}

SDL_Texture* AssetManager::get_texture(const std::string& name) const{
    auto it = texture_index_by_name_.find(name);

    if (it == texture_index_by_name_.end()){
        SDL_Log("Texture not found: %s", name.c_str());
        return nullptr;
    }

    return textures_[it->second];
}

TTF_Font* AssetManager::get_font(const std::string& name) const{
    auto it = font_index_by_name_.find(name);

    if (it == font_index_by_name_.end()){
        SDL_Log("Font not found: %s", name.c_str());
        return nullptr;
    }

    return fonts_[it->second];
}

Mix_Chunk* AssetManager::get_sound(const std::string& name) const{
    auto it = sound_index_by_name_.find(name);

    if (it == sound_index_by_name_.end()){
        SDL_Log("Sound not found: %s", name.c_str());
        return nullptr;
    }

    return sounds_[it->second];
}

void AssetManager::cleanup(){
    // Destroy all GPU textures
    for (SDL_Texture* texture : textures_){
        SDL_DestroyTexture(texture);
    }
    // Close all loaded fonts
    for (TTF_Font* font : fonts_){
        TTF_CloseFont(font);
    }
    // Free all loaded sound effects
    for (Mix_Chunk* sound : sounds_){
        Mix_FreeChunk(sound);
    }

    // Clear both the storage vectors and lookup maps
    textures_.clear();
    texture_index_by_name_.clear();

    fonts_.clear();
    font_index_by_name_.clear();

    sounds_.clear();
    sound_index_by_name_.clear();
}

void AssetManager::unload_texture(const std::string& name){
    auto it = texture_index_by_name_.find(name);

    // If the texture is not loaded, there is nothing to unload.
    if (it == texture_index_by_name_.end()){
        return;
    }

    size_t index = it->second;

    // Destroy the SDL texture before removing it from our storage.
    // This prevents a memory leak when changing/reloading maps.
    SDL_DestroyTexture(textures_[index]);

    // Remove the texture from the vector.
    textures_.erase(textures_.begin() + index);

    // Remove the name lookup.
    texture_index_by_name_.erase(it);

    // Rebuild indices because erasing from a vector shifts later elements.
    for (auto& pair : texture_index_by_name_){
        if (pair.second > index){
            pair.second--;
        }
    }
}