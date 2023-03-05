#include "Renderer.h"
#include "Walnut/Random.h"

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}


void Renderer::Render() {
	//逐像素设置图片信息
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); ++y) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); ++x) {
			glm::vec2 coord = {
				(float)x / m_FinalImage->GetWidth(),
				(float)y / m_FinalImage->GetHeight()
			};
			coord = coord * 2.0f - 1.0f;// 屏幕UV 转换到 -1 -> 1

			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y*m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}


	m_FinalImage->SetData(m_ImageData);
}

void Renderer::OnResize(uint32_t width,uint32_t height) {
	if (m_FinalImage) {
		//No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height) {
			return;
		}
		m_FinalImage->Resize(width, height);
	}
	else {
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

/// <summary>
/// 可以理解为片元着色器
/// </summary>
/// <param name="coord"></param>
/// <returns></returns>
glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	//射线开始的地方,在这里同样是摄像机的位置
	glm::vec3 rayOrigin(0.0f,0.0f,2.0f);
	//只在2D空间内进行,但是给一个虚拟的z轴(固定的)
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;

	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	// Quadratic forumula discriminant(求解的个数);
	// b^2 - 4ac

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant < 0.0f)
		return glm::vec4(0,0,0,1);

	//hit的t的解(二次方程的根)
	//(-b +- sqrt(discriminant)) / 2a
	float t0 = (-b + glm::sqrt(discriminant)) / 2.0f * a;
	float closestT = (-b - glm::sqrt(discriminant)) / 2.0f * a;

	//碰撞到的点
	glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;
	//假设法线就是原点到碰撞点的连线
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

	//正常颜色=法线点积光线
	float d = glm::max(glm::dot(normal, -lightDir), 0.0f);

	glm::vec3 sphereColor(1, 0, 1);
	sphereColor *= d;
	return glm::vec4(sphereColor, 1.0f);
}
