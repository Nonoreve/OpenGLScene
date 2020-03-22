/*
 * Copyright 2020 Nonoreve noe.boonstra@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "renderer.h"
#include <SDL2/SDL_image.h>

#define __RESSOURCE_FILE "../resources/"
#define RESOURCE_FILE(x) __RESSOURCE_FILE x

class Texture {
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	SDL_Surface* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	
public:
	Texture(const std::string& path);
	~Texture();
	
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;
	
	inline int GetWidth() const {return m_Width;}
	inline int GetHeight() const {return m_Height;}
};

#endif // TEXTURE_H
