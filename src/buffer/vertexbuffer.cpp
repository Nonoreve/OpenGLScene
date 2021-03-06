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

#include <iostream>
#include "buffer/vertexbuffer.h"
#include "rendering/renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size, unsigned int elements){
	Init(data, size, elements);
}

VertexBuffer::VertexBuffer(){
	// To let complexVertexBuffer do stuff before initialising
}

VertexBuffer::~VertexBuffer(){
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Init(const void* data, unsigned int size, unsigned int elements){
	m_Elements = elements;
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

void VertexBuffer::Bind() const{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
