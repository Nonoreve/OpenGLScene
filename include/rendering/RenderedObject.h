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

#include "animation/Animation.h"
#include "geometry/Geometry.h"
#include "rendering/Camera.h"
#include "light.h"
#include "material.h"
#include "Shader.h"
#include "texture.h"
#include "vertexarray.h"


class RenderedObject {
	private:
		VertexArray* m_VertexArray;
		Geometry* m_Geometry;
		Material* m_Material; // TODO transfer texture to material
		Texture* m_Texture;
		glm::mat4 m_Transform;
		bool m_Visible;
		RenderedObject* m_Parent; // TODO think children is enough
		Shader* m_Shader; // TODO why bind shader to the object ?
		std::vector<RenderedObject*> m_Children;
		std::vector<Animation*> m_AnimChilds;

		//void CreerBuffer(const char* imgName);// TODO remove ONLY KEEP isVisible = true;

	public:
		inline RenderedObject() : m_Transform(glm::mat4(1.0f)), m_Visible(false) {};
		RenderedObject(RenderedObject& parent); // TODO expecting undefined behavior due to copy constructor overriding
		RenderedObject(VertexArray& vertexArray, Geometry* geometry, Material& material, Texture& texture, RenderedObject& parent, Shader* shader);

		void UpdateAnimations(float currentTime); // TODO transfer work to animation class
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
		void ChangeGeometry(Geometry* geometry); // TODO remove ???
		void AfficherRecursif(std::stack<glm::mat4>& matrices, float currentTime, Camera camera, Light sun, Texture* caustic = nullptr);
		void Afficher(std::stack<glm::mat4>& matrices, Camera camera, Light sun, Texture* caustic = nullptr);

};

#endif // RENDEREDOBJECT_H
