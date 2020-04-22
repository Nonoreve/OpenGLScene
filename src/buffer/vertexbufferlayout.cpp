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

#include "buffer/vertexbufferlayout.h"


void VertexBufferLayout::Push(unsigned int count, unsigned int type)
{
	m_Elements.push_back({ type, count, type == GL_UNSIGNED_BYTE });
	m_Stride += VertexBufferElement::GetSizeOfType(type) * count;
}