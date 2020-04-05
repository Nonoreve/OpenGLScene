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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

struct lightComponents {
    float ambient;
    float diffuse;
    float specular;
    float alpha;

    lightComponents ( float initAmbient, float initDiffuse, float initSpecular, float initAlpha ) : ambient ( initAmbient ), diffuse ( initDiffuse ), specular ( initSpecular ), alpha ( initAlpha ) {}
    lightComponents() : ambient ( 0.5f ), diffuse ( 0.5f ), specular ( 0.5f ), alpha ( 500.0f ) {}

    glm::vec4 asVector(){
		return glm::vec4(ambient, diffuse, specular, alpha);
	}
};

class Material {
    private:
    lightComponents m_Light;
    glm::vec3 m_Color;

    public:
    inline Material() : m_Light(), m_Color ( glm::vec3 ( 1.0f, 1.0f, 1.0f ) ){};
    inline Material ( glm::vec3 color, glm::vec4 lightComponents ) : m_Light ( {lightComponents.x, lightComponents.y, lightComponents.z, lightComponents.w} ), m_Color ( color ){};
	inline glm::vec4 getComponents(){
		return m_Light.asVector();
	}
	inline glm::vec3 getColor(){
		return m_Color;
	}
};

#endif // MATERIAL_H
