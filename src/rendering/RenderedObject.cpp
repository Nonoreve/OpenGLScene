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

#include "rendering/RenderedObject.h"
#include <algorithm>
#include <iostream>
#include <glm/detail/type_mat.hpp>
#include <glm/gtc/matrix_transform.hpp>

RenderedObject::RenderedObject(RenderedObject& parent) : m_TempMatrix(glm::mat4(1.0f)), m_Parent(&parent), m_Visible(false) {
	parent.AddChild(this);
}

RenderedObject::RenderedObject(RenderedObject& parent, Shader* shader, const char* imgName, Material* material, Geometry* geometry) : m_TempMatrix(glm::mat4(1.0f)), m_Parent(&parent), m_Geometry(geometry), m_Material(material), m_Shader(shader), m_Visible(false) {
	if(geometry != nullptr)
		CreerBuffer(imgName);
	parent.AddChild(this);
}

void RenderedObject::UpdateAnimations(float currentTime) {
	// TODO I think erase is redundant
	m_AnimChilds.erase(
	    std::remove_if(
	        m_AnimChilds.begin(),
	        m_AnimChilds.end(),
	[&](Animation * const & p) {
		return !(p->CheckAnimation(m_TempMatrix, currentTime));
	}
	    ),
	m_AnimChilds.end()
	);
}

void RenderedObject::Move(glm::vec3 position) {
	m_TempMatrix = glm::translate(m_TempMatrix, position);
}

void RenderedObject::SetScale(glm::vec3 position) { // TODO rename vector
	m_TempMatrix = glm::scale(m_TempMatrix, position);
}

void RenderedObject::Rotate(float angle, glm::vec3 position) {
	if(glm::all(glm::equal(position, glm::vec3(0.0, 0.0, 0.0))))
		return; // TODO print error ?

	m_TempMatrix = glm::rotate(m_TempMatrix, glm::radians(angle), position);
}

// Warning with parent circle, can't check by position
void RenderedObject::SetParent(RenderedObject& obj) {
	if(m_Parent != nullptr)
		m_Parent->m_Children.erase(std::remove(m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this), m_Parent->m_Children.end());

	m_Parent = &obj;
	obj.AddChild(this);
	// TODO check parent is not in children
}

void RenderedObject::ChangeGeometry(Geometry* geometry) {
	if(m_Geometry != nullptr) {
		std::cerr << "Unimplemented behavior !" << std::endl;
		return;
	}

	m_Geometry = geometry;
	CreerBuffer(""); // TODO wtf
}

void RenderedObject::AfficherRecursif(std::stack<glm::mat4>& matrices, float currentTime) {
	UpdateAnimations(currentTime);
	if(matrices.size() == 0)
		matrices.push(m_TempMatrix);
	else
		matrices.push(matrices.top() * m_TempMatrix);

	if(m_Visible)
		Afficher(matrices);

	for(RenderedObject* tempObj : m_Children)
		tempObj->AfficherRecursif(matrices, currentTime);

	matrices.pop();
}

void RenderedObject::CleanBufferRecursif() {
	// TODO clash with my code (glDeleteBuffer)
	for(RenderedObject* tempObj : m_Children)
		tempObj->CleanBufferRecursif();
}

void RenderedObject::Afficher(std::stack<glm::mat4>& matrices) {
	// TODO clash with my code
}

void RenderedObject::CreerBuffer(const char* imgName) {
	// TODO clash with my code
}
