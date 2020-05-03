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

#include "rendering/Camera.h"
#include "animation/Animation.h"
#include <iostream>
#include <algorithm>


void Camera::SetView(glm::mat4 view)
{
	m_View = view;

	//toVerif

	m_Position = glm::vec3(view[3][0], view[3][1], view[3][2]);

}

void Camera::SetPosition(glm::vec3 pos)
{
	m_Position = pos;
}

void Camera::UpdateView() {
	m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::UpdateAnimations(float currentTime) {
	// TODO I think erase is redundant
	m_AnimChilds.erase(
	    std::remove_if(
	        m_AnimChilds.begin(),
	        m_AnimChilds.end(),
	[&](Animation * const & p) {
		return !(p->CheckAnimation(m_View, currentTime));
	}
	    ),
	m_AnimChilds.end()
	);
}

void Camera::Rotate(float angle, glm::vec3 position) {
	if (glm::all(glm::equal(position, glm::vec3(0.0, 0.0, 0.0))))
	{
		std::cout << "can't rotate" << std::endl;
		return; // TODO print error ?
	}

	m_View = glm::rotate(m_View, glm::radians(angle), position); // We rotate via an axis and an angle around this axis
}

// TODO move as field
float yaw = -90.0f;
float pitch = 0.0f;
bool changingLook = false; // changing angles only if right cliking

void Camera::inputMove(const SDL_Event& event) {
	// TODO add velocity
	int xPos = event.motion.x;
	int yPos = event.motion.y;
	switch(event.type) {
	case SDL_KEYDOWN:
		switch(event.key.keysym.sym) {
		case SDLK_z:
		case SDLK_UP:
			m_Position += m_Speed * m_Front;
			break;
		case SDLK_s:
		case SDLK_DOWN:
			m_Position -= m_Speed * m_Front;
			break;
		case SDLK_q:
		case SDLK_LEFT:
			m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * m_Speed;
			break;
		case SDLK_d:
		case SDLK_RIGHT:
			m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * m_Speed;
			break;
		case SDLK_SPACE:
			m_Position += m_Speed * m_Up;
			break;
		case SDLK_LESS:
			m_Position -= m_Speed * m_Up;
			break;
		default:
			std::cout << "[Camera Warning] Unhandeld key pressed " << event.key.keysym.sym << std::endl;
			break;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		switch(event.button.button) {
		case SDL_BUTTON_LEFT:
			std::cout << "[Camera Warning] Left click at coordinates (" << event.button.x << ", " << event.button.y << ")" << std::endl;
			break;
		case SDL_BUTTON_MIDDLE:
			std::cout << "[Camera Warning] Middle click at coordinates (" << event.button.x << ", " << event.button.y << ")" << std::endl;
			break;
		case SDL_BUTTON_RIGHT:
			std::cout << "[Camera Warning] Right click at coordinates (" << event.button.x << ", " << event.button.y << ")" << std::endl;
			changingLook = true;
			break;
		default:
			std::cout << "[Camera Warning] Unhandeld mouse button pressed " << event.button.button << std::endl;
			break;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		switch(event.button.button) {
		case SDL_BUTTON_LEFT:
			break;
		case SDL_BUTTON_MIDDLE:
			break;
		case SDL_BUTTON_RIGHT:
			changingLook = false;
			break;
		default:
			break;
		}
		break;
	case SDL_MOUSEMOTION:
		//std::cout << "Mouse moved to (" << event.motion.x << ", " << event.motion.y << ")" << std::endl;
		if(xPos <= 0 || yPos <= 0 || xPos > WIDTH || yPos > HEIGHT) {
			break;
		}
		if(!changingLook)
			break;
		yaw += event.motion.xrel * m_Speed;
		pitch += event.motion.yrel * m_Speed;
		if(pitch >= 90.0f)
			pitch = 89.0f;
		if(pitch <= -90.0f)
			pitch = -89.0f;
		m_Front = glm::normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
		break;
	case SDL_MOUSEWHEEL:
		std::cout << "Scrolled " << event.wheel.y << " steps" << std::endl;
		break;

	}
}
