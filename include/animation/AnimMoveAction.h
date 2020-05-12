/*
 * Copyright 2020 Nonoreve and Kilian
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

#ifndef ANIMMOVEACTION_H
#define ANIMMOVEACTION_H

#include "animation/animAction.h"

class AnimMoveAction : public AnimAction {
	public:
		void Action(glm::mat4& mat);
		inline AnimMoveAction(glm::vec3 position) : AnimAction(position) {};
};

#endif // ANIMMOVEACTION_H
