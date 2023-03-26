#pragma once
#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>
#include "../Camera.h"
#include "Ray.h"

class Renderer
{
public:
	Renderer() = default;

	void Render(const Camera& camera);
	void OnResize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetFinalImage() { return m_FinalImage; }
private:
	glm::vec4 TraceRay(const Ray& ray);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;

	/// <summary>
	/// CPU-Side Image Data 
	/// </summary>
	uint32_t* m_ImageData = nullptr;
};

