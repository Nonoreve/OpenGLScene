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
#include <algorithm>

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
	if(glm::all(glm::equal(position, glm::vec3(0.0, 0.0, 0.0))))
		return; // TODO print error ?

	m_View = glm::rotate(m_View, glm::radians(angle), position); // We rotate via an axis and an angle around this axis
}
