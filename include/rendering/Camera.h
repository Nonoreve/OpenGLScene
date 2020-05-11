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

#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "animation/Animation.h"
#include "rendering/renderer.h"
#include "texture.h"

class Camera {
	private:
		std::vector<Animation*> m_AnimChilds;
		const float m_Speed;
		glm::vec3 m_Front;
		glm::vec3 m_Up;
		glm::vec3 m_Position; // should be bottom of matrix std::stack

		float m_Width;
		float m_Height;
		float m_Fov;

		glm::mat4 m_Projection;
		glm::mat4 m_View;

	public:
		// TODO add some kind of inheritance with camera and RenderedObject (SceneObject ?)
		// TODO adapt values, add them as parameters
		inline Camera() :
			m_Speed(0.2f),
			m_Front(glm::vec3(0.0f, 0.0f, -1.0f)),
			m_Up(glm::vec3(0.0f, 1.0f, 0.0f)),
			m_Position(glm::vec3(0.0f, 0.0f, 0.0f)),
			m_Width(1024.0f),
			m_Height(768.0f),
			m_Fov(120.0f),
			m_Projection(glm::perspective(glm::radians(70.0f), (float)(WIDTH) / (float)(HEIGHT), 0.01f, 1000.0f)),
			//m_Projection(glm::ortho(0.0f, (float)(WIDTH), 0.0f, (float)(HEIGHT), -100.0f, 100.0f)),
			// Head is up (set to 0,-1,0 to look upside-down)
			//m_View(glm::lookAt(m_Position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))) {}
			m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))) {}

		void SetView(glm::mat4 view);

		void SetPosition(glm::vec3 pos);
		void UpdateView();
		void UpdateAnimations(float currentTime);
		
		inline void AddChild(Animation* obj) {
			m_AnimChilds.push_back(obj);
		}
		inline void AddAnimation(Animation* anim) {
			AddChild(anim);   // TODO messy need further refactoring
		}
		
		void Rotate(float angle, glm::vec3 position);
		
		inline glm::mat4 getProjectionM() {
			return m_Projection;
		}
		inline glm::mat4 getViewM() {
			return m_View;
		}
		inline glm::vec3 getPosition() {
			return m_Position;
		}
		
		void inputMove(const SDL_Event &event);
		
};

#endif // CAMERA_H
