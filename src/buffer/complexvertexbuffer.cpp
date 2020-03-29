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

#include "buffer/complexvertexbuffer.h"
#include <iostream>
#include "renderer.h"

ComplexVertexBuffer::ComplexVertexBuffer(unsigned int size, unsigned int elements, int count, ...) : VertexBuffer(NULL, size, elements) {
	va_list args;
	unsigned int offset = 0;
	va_start(args, count);
	for (int i = 0; i < count; i++) {
		SubData bufData = va_arg(args, SubData);
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, bufData.size, bufData.data));
		offset += bufData.size;
	}
	va_end(args);
}
