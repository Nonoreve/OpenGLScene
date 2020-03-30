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

#ifndef RENDEREDOBJECT_H
#define RENDEREDOBJECT_H

#include <vector>
#include <stack>

#include "geometry/Geometry.h"
#include "material.h"
#include "animation/Animation.h"
#include "Shader.h"


class RenderedObject {
	private:
		RenderedObject* m_Parent;
		Geometry* m_Geometry;
		Shader* m_Shader; // not implemented
		glm::mat4 m_TempMatrix; // TODO remove if possible
		Material* m_Material;
		unsigned int m_CurrentBuffer;// TODO remove if possible
		std::vector<RenderedObject*> m_Children;
		std::vector<Animation*> m_AnimChilds;
		bool m_Visible;
		unsigned int m_TextureID;// TODO remove if possible

		void CreerBuffer(const char* imgName);

	public:
		inline RenderedObject() : m_TempMatrix(glm::mat4(1.0f)), m_Visible(false) {};
		RenderedObject(RenderedObject& parent); // TODO expecting undefined behavior due to copy constructor overriding
		RenderedObject(RenderedObject& parent, Shader* shader, const char* imgName, Material* material, Geometry* geometry);

		void UpdateAnimations(float currentTime);
		inline void AddChild(RenderedObject* obj) {
			m_Children.push_back(obj); // TODO incredibly messy
		}
		inline void AddChild(Animation* obj) {
			m_AnimChilds.push_back(obj);
		}
		inline void AddAnimation(Animation* anim) {
			AddChild(anim);   // TODO messy, need further refactoring
		}
		void Move(glm::vec3 position);
		void SetScale(glm::vec3 position);
		void Rotate(float angle, glm::vec3 position);
		void SetParent(RenderedObject& obj);
		void ChangeGeometry(Geometry* geometry);
		void AfficherRecursif(std::stack<glm::mat4>& matrices, float currentTime);
		void CleanBufferRecursif();
		void Afficher(std::stack<glm::mat4>& matrices);

};

#endif // RENDEREDOBJECT_H
