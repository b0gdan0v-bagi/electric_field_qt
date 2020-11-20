#pragma once

#include <cmath>
#include <string>
#include <ostream>

template <class T>
struct vec2D
{
	T x, y;
	vec2D() : x(0), y(0) {}
	vec2D(T _x, T _y) :x(_x), y(_y) {}
	vec2D(const vec2D& v) : x(v.x), y(v.y) {}
	vec2D& operator=(const vec2D& v) = default;
	T mag() const { return T(std::sqrt(x * x + y * y)); }
	T mag2() const { return x * x + y * y; }
	vec2D  norm() const { T r = 1 / mag(); return vec2D(x * r, y * r); }
	vec2D  perp() const { return vec2D(-y, x); }
	T dot(const vec2D& rhs) const { return this->x * rhs.x + this->y * rhs.y; }
	T cross(const vec2D& rhs) const { return this->x * rhs.y - this->y * rhs.x; }
	vec2D  operator +  (const vec2D& rhs) const { return vec2D(this->x + rhs.x, this->y + rhs.y); }
	vec2D  operator -  (const vec2D& rhs) const { return vec2D(this->x - rhs.x, this->y - rhs.y); }
	vec2D  operator *  (const T& rhs)           const { return vec2D(this->x * rhs, this->y * rhs); }
	vec2D  operator *  (const vec2D& rhs) const { return vec2D(this->x * rhs.x, this->y * rhs.y); }
	vec2D  operator /  (const T& rhs)           const { return vec2D(this->x / rhs, this->y / rhs); }
	vec2D  operator /  (const vec2D& rhs) const { return vec2D(this->x / rhs.x, this->y / rhs.y); }
	vec2D& operator += (const vec2D& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
	vec2D& operator -= (const vec2D& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
	vec2D& operator *= (const T& rhs) { this->x *= rhs; this->y *= rhs; return *this; }
	vec2D& operator /= (const T& rhs) { this->x /= rhs; this->y /= rhs; return *this; }
	vec2D  operator +  () const { return { +x, +y }; }
	vec2D  operator -  () const { return { -x, -y }; }
	bool operator == (const vec2D& rhs) const { return (this->x == rhs.x && this->y == rhs.y); }
	bool operator != (const vec2D& rhs) const { return (this->x != rhs.x || this->y != rhs.y); }
	const std::string str() const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + ")"; }
	friend std::ostream& operator << (std::ostream& os, const vec2D& rhs) { os << rhs.str(); return os; }
	operator vec2D<int32_t>() const { return { static_cast<int32_t>(this->x), static_cast<int32_t>(this->y) }; }
	operator vec2D<float>() const { return { static_cast<float>(this->x), static_cast<float>(this->y) }; }
	operator vec2D<double>() const { return { static_cast<double>(this->x), static_cast<double>(this->y) }; }

};

typedef vec2D<int32_t> vi2D;
typedef vec2D<uint32_t> vu2D;
typedef vec2D<float> vf2D;
typedef vec2D<double> vd2D;