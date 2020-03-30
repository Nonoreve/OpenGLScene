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

#include "animation/AnimRotateAction.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#define TO_RADIANS(x) (float)(x * (M_PI / 180)) // TODO remove

void AnimRotateAction::Action(glm::mat4& mat) {
	mat = glm::rotate(mat, glm::radians(m_RotAngle), m_Position);
}
