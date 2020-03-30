#ifndef ANIMACTION_H
#define ANIMACTION_H

#include <glm/glm.hpp>

class AnimAction {
	protected:
		glm::vec3 m_Position;

	public:
		inline AnimAction(glm::vec3 position) : m_Position(position) {}
		virtual void Action(glm::mat4& mat) = 0;

};

#endif
