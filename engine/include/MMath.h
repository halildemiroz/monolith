#pragma once
#include <glm/glm.hpp>

namespace Monolith {

	struct Vec2{
		float x = 0.0f;
		float y = 0.0f;

		Vec2() = default;
		Vec2(float x, float y) : x(x), y(y){}
		Vec2(glm::vec2 v) : x(v.x), y(v.y){}

		operator glm::vec2() const { return { x, y }; }

		Vec2 operator-() const { return {-x, -y}; }
		Vec2 operator+(Vec2 o) const { return {x + o.x, y + o.y}; }
		Vec2 operator-(Vec2 o) const { return {x - o.x, y - o.y}; }
		Vec2 operator*(Vec2 o) const { return {x * o.x, y * o.y}; }
		Vec2 operator/(Vec2 o) const { return {x / o.x, y / o.y}; }
		Vec2 operator*(float s) const { return {x * s, y * s}; }
		Vec2 operator/(float s) const { return {x / s, y / s}; }

		Vec2& operator+=(Vec2 o) { x += o.x; y += o.y; return *this; }
		Vec2& operator-=(Vec2 o) { x -= o.x; y -= o.y; return *this; }
		Vec2& operator*=(float s) { x *= s; y *= s; return *this; }
		
		bool operator==(Vec2 o) const { return x == o.x && y == o.y; }
		bool operator!=(Vec2 o) const { return !(*this == o); }
	};

	inline Vec2 Lerp(Vec2 a, Vec2 b, float t) {
		return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
	}
	inline Vec2 mix(Vec2 a, Vec2 b, float t) {
		return Lerp(a, b, t);
	}

	struct IVec2{
		int x = 0;
		int y = 0;

		IVec2() = default;
		IVec2(int x, int y) : x(x), y(y){}
		IVec2(glm::ivec2 v) : x(v.x), y(v.y){}
		
		operator glm::ivec2() const { return{x, y}; }

		IVec2 operator+(IVec2 o) const { return{x + o.x, y + o.y}; }
		IVec2 operator-(IVec2 o) const { return{ x - o.x, y - o.y }; }
		IVec2 operator*(int s) const { return{x * s, y * s}; }

		bool operator==(IVec2 o) const { return x == o.x && y == o.y; }
		bool operator!=(IVec2 o) const { return !(*this == o); }
	};
	
	struct Vec4{
		float x, y, z, w;
		
		Vec4() = default;
		Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w){}
		Vec4(glm::vec4 v) : x(v.x), y(v.y), z(v.z), w(v.w){}

		operator glm::vec4() const { return{x,y,z,w}; }
	};

	struct Color{
		float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

		Color() = default;
		Color(float r, float g, float b, float a) : r(r), b(b), g(g), a(a){}
		
		operator glm::vec4() const { return {r, g, b, a}; }
		
		static const Color White;
		static const Color Black;
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Yellow;
		static const Color Transparent;
	};

	inline const Color Color::White{1, 1, 1, 1};
	inline const Color Color::Black{0, 0, 0, 1};
	inline const Color Color::Red{1, 0, 0, 1};
	inline const Color Color::Green{0, 1, 0, 1};
	inline const Color Color::Blue{0, 0, 1, 1};
	inline const Color Color::Yellow{1, 1, 0, 1};
	inline const Color Color::Transparent{0, 0, 0, 0};
}	
