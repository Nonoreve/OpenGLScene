#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light {
	public :
		glm::vec3 m_Position;
		glm::vec3 m_Color;
		inline Light(glm::vec3 position, glm::vec3 color) : m_Position(position), m_Color(color) {}

};

#endif
