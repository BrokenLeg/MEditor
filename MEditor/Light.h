#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Light
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct AttenuationData
{
	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight
{
	glm::vec3 direction;
	Light base;
};

struct PointLight
{
	Light base;
	AttenuationData attenuation;
};

struct SpotLight
{
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;

	AttenuationData attenuation;
	Light base;
};