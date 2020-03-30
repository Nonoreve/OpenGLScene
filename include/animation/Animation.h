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

#ifndef ANIMATION_H
#define ANIMATION_H

#include "animation/animAction.h"

class Animation {
	private:
		float m_StartTime;
		float m_EndTime;
		AnimAction* m_Action;

	public:
		Animation(float startTime, float endTime, AnimAction* action);
		bool CheckAnimation(glm::mat4 &matModif, float currentTime);

};

#endif // ANIMATION_H
