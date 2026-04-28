#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <cstddef>

class AssetManager{
    public:
        bool load_texture(SDL_Renderer* renderer, const std::string& name, const std::string& path);
        bool load_font(const std::string& name, const std::string& path, int size);
        bool load_sound(const std::string& name, const std::string& path);

        SDL_Texture* get_texture(const std::string& name) const;
        TTF_Font* get_font(const std::string& name) const;
        Mix_Chunk* get_sound(const std::string& name) const;

        void cleanup();
    
    private:
        std::vector<SDL_Texture*> textures_;
        std::unordered_map<std::string, size_t> texture_index_by_name_;

        std::vector<TTF_Font*> fonts_;
        std::unordered_map<std::string, size_t> font_index_by_name_;

        std::vector<Mix_Chunk*> sounds_;
        std::unordered_map<std::string, size_t> sound_index_by_name_;
};