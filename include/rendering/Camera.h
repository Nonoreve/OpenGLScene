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
		// TODO adapt values
		inline Camera() :
			m_Speed(0.5f),
			m_Front(glm::vec3(0.0f, 0.0f, -1.0f)),
			m_Up(glm::vec3(0.0f, 1.0f, 0.0f)),
			m_Position(glm::vec3(0.0f, 0.0f, 3.0f)),
			m_Width(1024.0f),
			m_Height(768.0f),
			m_Fov(120.0f),
			m_Projection(glm::perspective(glm::radians(70.0f), (float)m_Width / (float)m_Height, 0.1f, 100.0f)),
			// Head is up (set to 0,-1,0 to look upside-down)
			m_View(glm::lookAt(m_Position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))) {}

		void UpdateView();
		void UpdateAnimations(float currentTime);
		inline void AddChild(Animation* obj){m_AnimChilds.push_back(obj);}
		inline void AddAnimation(Animation* anim){AddChild(anim);} // TODO messy need further refactoring
		void Rotate(float angle, glm::vec3 position);

};

#endif // CAMERA_H
