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

#include "animation/Animation.h"
#include <glm/glm.hpp>
#include <iostream>

Animation::Animation(float startTime, float endTime, AnimAction* action) {
	if(startTime > endTime) {
		std::cerr << "le temps de debut doit etre avant celui de fin ..." << std::endl;
		return;
	}
	m_StartTime = startTime;
	m_EndTime = endTime;
	m_Action = action;
}

bool Animation::CheckAnimation(glm::mat4& matModif, float currentTime) {
	if(currentTime > m_StartTime && currentTime < m_EndTime) {
		m_Action->Action(matModif);
	} else if(currentTime > m_EndTime) {
		return false;
	}
	return true;
}
