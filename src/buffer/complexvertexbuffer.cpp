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
#include "rendering/renderer.h"

ComplexVertexBuffer::ComplexVertexBuffer(unsigned int elements, int count, ...) : VertexBuffer() {
	va_list args;
	va_start(args, count);
	unsigned int size = 0;
	// compute total size
	for (int i = 0; i < count; i++) {
		SubData bufData = va_arg(args, SubData);
		size += bufData.sizePerVertex * elements;
	}
	va_end(args);
	Init(NULL, size, elements);
	va_list args2;
	va_start(args2, count);
	unsigned int offset = 0;
	for (int i = 0; i < count; i++) {
		SubData bufData = va_arg(args2, SubData);
		unsigned int bufSize = bufData.sizePerVertex * elements;
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, bufSize, bufData.data));
		offset += bufSize;
	}
	va_end(args2);
}
