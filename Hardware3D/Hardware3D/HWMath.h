#pragma once
#include <cmath>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template <typename T>
constexpr T Sq(const T& x)
{
	return x * x;
}

template<typename T>
T WrapAngle(T theta)
{
	const T modded = fmod(theta, (T)2.0 * (T)PI_D);
	return (modded > (T)PI_D) ? (modded - (T)2.0 * (T)PI_D) : modded;
}

template<typename T>
constexpr T Interpolate(const T& src, const T& dst, const float alpha)
{
	return src + (dst - src) * alpha;
}

template<typename T>
constexpr T ToRad(T deg)
{
	return deg * PI / (T)180.0f;
}