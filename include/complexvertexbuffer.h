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

#ifndef COMPLEXVERTEXBUFFER_H
#define COMPLEXVERTEXBUFFER_H

#include "vertexbuffer.h"
#include <cstdarg>

/**
 * A vertex buffer made of several data arrays. Use of glBufferSubData();
 */
class ComplexVertexBuffer :  VertexBuffer {
	ComplexVertexBuffer(unsigned int size, int count, ...);
};

struct SubData{
	unsigned int size;
	const void* data;
}
#endif // COMPLEXVERTEXBUFFER_H
