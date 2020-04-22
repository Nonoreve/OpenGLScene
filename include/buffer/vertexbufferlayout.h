/*
 * Copyright 2020 <copyright holder> <email>
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

#ifndef VERTEXBUFFERLAYOUT_H
#define VERTEXBUFFERLAYOUT_H

#include <vector>
#include <GL/glew.h>

#include "rendering/renderer.h"

struct VertexBufferElement{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;
	
	static unsigned int GetSizeOfType(unsigned int type){
		switch(type){
			case GL_FLOAT: return 4;
			case GL_UNSIGNED_INT: return 4;
			case GL_UNSIGNED_BYTE: return 1;
		}
		ASSERT(false);
		return 0;
	}
};


class VertexBufferLayout {
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
	
public:
	VertexBufferLayout() : m_Stride(0){}
	

	void Push(unsigned int count, unsigned int type);

	
	inline const std::vector<VertexBufferElement> GetElements() const& {return m_Elements;}
	inline unsigned int GetStride() const {return m_Stride;}
};

#endif // VERTEXBUFFERLAYOUT_H
