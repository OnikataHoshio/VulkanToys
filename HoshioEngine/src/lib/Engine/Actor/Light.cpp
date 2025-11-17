#include "Engine/Actor/Light.h"


namespace HoshioEngine {
	Light::Light(const float intensity, const glm::vec3 lightColor) : intensity(intensity), lightColor(lightColor)
	{
	}

	void Light::SetIntensity(const float intensity)
	{
		this->intensity = intensity;
	}

	void Light::SetLightColor(const glm::vec3 lightColor)
	{
		this->lightColor = lightColor;
	}

	void Light::SetRotation(const glm::vec3 rotation)
	{
		Actor::SetRotation(rotation);
		const glm::vec4 direction = glm::vec4(0.0, -1.0f, 0.0f, 1.0f) * RotationMatrix();
		lightDirection = glm::vec3(direction);
	}

	float Light::GetIntensity() const
	{
		return intensity;
	}

	glm::vec3 Light::GetLightColor() const
	{
		return lightColor;
	}

	glm::vec3 Light::GetLightDirection() const
	{
		return lightDirection;
	}

	glm::vec3 Light::GetLightColorPlusIntensity() const
	{
		return intensity * lightColor;
	}

}