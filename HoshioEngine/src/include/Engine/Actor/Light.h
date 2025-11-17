#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Actor.h"

namespace HoshioEngine {
	class Light : public Actor {
	protected:
		float intensity = 1.0f;

		glm::vec3 lightColor = glm::vec3(1.0f);

		glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);

	public:
		Light() = default;
		Light(const float intensity, const glm::vec3 lightColor);

		void SetIntensity(const float intensity);

		void SetLightColor(const glm::vec3 lightColor);

		virtual void SetRotation(const glm::vec3 rotation) override;

		float GetIntensity() const;

		glm::vec3 GetLightColor() const;

		glm::vec3 GetLightDirection() const;

		glm::vec3 GetLightColorPlusIntensity() const;

	private:
		
	};
}


#endif // !_LIGHT_H_

