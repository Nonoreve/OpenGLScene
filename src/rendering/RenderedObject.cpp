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
#include "light.h"

#include <algorithm>
#include <iostream>
#include <glm/detail/type_mat.hpp>
#include <glm/gtc/matrix_transform.hpp>

RenderedObject::RenderedObject(RenderedObject& parent) : m_Transform(glm::mat4(1.0f)), m_Visible(false), m_Parent(&parent) {
	parent.AddChild(this);
}

RenderedObject::RenderedObject(VertexArray& vertexArray, Geometry* geometry, Material& material, Texture& texture, RenderedObject& parent, Shader* shader) : m_VertexArray(&vertexArray), m_Geometry(geometry), m_Material(&material), m_Texture(&texture), m_Transform(glm::mat4(1.0f)), m_Visible(true),  m_Parent(&parent), m_Shader(shader) {
	parent.AddChild(this);
}

void RenderedObject::UpdateAnimations(float currentTime) {
	// TODO I think erase is redundant
	m_AnimChilds.erase(
	    std::remove_if(
	        m_AnimChilds.begin(),
	        m_AnimChilds.end(),
	[&](Animation * const & p) {
		return !(p->CheckAnimation(m_Transform, currentTime));
	}
	    ),
	m_AnimChilds.end()
	);
}

void RenderedObject::Move(glm::vec3 position) {
	m_Transform = glm::translate(m_Transform, position);
}

void RenderedObject::SetScale(glm::vec3 position) { // TODO rename vector
	m_Transform = glm::scale(m_Transform, position);
}

void RenderedObject::Rotate(float angle, glm::vec3 position) {
	if(glm::all(glm::equal(position, glm::vec3(0.0, 0.0, 0.0))))
		return; // TODO print error ?

	m_Transform = glm::rotate(m_Transform, glm::radians(angle), position);
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
}

void RenderedObject::AfficherRecursif(std::stack<glm::mat4>& matrices, float currentTime, Camera camera) { // TODO refactoring needed
	UpdateAnimations(currentTime);
	if(matrices.size() == 0)
		matrices.push(m_Transform);
	else
		matrices.push(matrices.top() * m_Transform);

	if(m_Visible)
		Afficher(matrices, camera);

	for(RenderedObject* tempObj : m_Children)
		tempObj->AfficherRecursif(matrices, currentTime, camera);

	matrices.pop();
}

void RenderedObject::CleanBufferRecursif() {
	// TODO clash with my code (glDeleteBuffer)
	for(RenderedObject* tempObj : m_Children)
		tempObj->CleanBufferRecursif();
}

void RenderedObject::Afficher(std::stack<glm::mat4>& matrices, Camera camera) {
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos(0.0f, 1.0f, 0.0f);

	Light sun = Light(lightPos, lightColor);

	glm::mat4 modelView = camera.getViewM() * matrices.top();
	glm::mat4 mvp = camera.getProjectionM() * camera.getViewM() * matrices.top();

	m_VertexArray->Bind();
	m_Texture->Bind();
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("u_MVP", mvp);
	m_Shader->SetUniformMat4f("u_ModelView", modelView);
	m_Shader->SetUniform1i("u_Texture", 0); // TEXTURE_SLOT
	m_Shader->SetUniformVec4f("u_K", m_Material->getComponents());
	m_Shader->SetUniformVec3f("u_Color", m_Material->getColor());
	m_Shader->SetUniformVec3f("u_LightPosition", sun.m_Position);
	m_Shader->SetUniformVec3f("u_LightColor", sun.m_Color);
	m_Shader->SetUniformVec3f("u_CameraPosition", camera.getPosition());

	glDrawArrays(GL_TRIANGLES, 0, m_Geometry->getNbVertices());
}
