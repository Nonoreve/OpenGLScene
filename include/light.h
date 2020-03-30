#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light{
private:
	glm::vec3 m_Position;
	glm::vec3 m_Color;
	
public :
	inline Light(glm::vec3 position, glm::vec3 color) : m_Position(position), m_Color(color){}
	
};

#endif
