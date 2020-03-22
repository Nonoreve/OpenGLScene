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

#include "texture.h"
#include <cstring>

Texture::Texture(const std::string& path) : m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0) {
	SDL_Surface* img = IMG_Load(path.c_str());
	if (!img) {
		std::cerr << "[SDL_image Error] Can't read file '" << path << "'." << std::endl;
	}
	m_LocalBuffer = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGBA8888, 0);
	SDL_FreeSurface(img);
	m_Width = m_LocalBuffer->w;
	m_Height = m_LocalBuffer->h;

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	unsigned char* flippedImg = (unsigned char*)malloc(m_Width * m_Height * 4); // nbPixels * nbChannels * bytePerChannel(1)
	const unsigned int SIZE_ROW = m_Width * 4;
	for(int row = 0; row < m_Height; row++){
		// copying an entire row
		memcpy(flippedImg + (m_Height - row) * SIZE_ROW, (unsigned char*)m_LocalBuffer->pixels + row * SIZE_ROW, SIZE_ROW);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)flippedImg);

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	if (m_LocalBuffer)
		SDL_FreeSurface(m_LocalBuffer);
	
}

Texture::~Texture() {
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const {
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const {
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
