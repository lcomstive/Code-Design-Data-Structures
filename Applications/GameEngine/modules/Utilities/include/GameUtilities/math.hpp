#pragma once
#include <raymath.h>

// --- Vector2 --- //
inline Vector2 operator +(Vector2 a, Vector2 const& b) { return Vector2Add(a, b); }
inline Vector2 operator -(Vector2 a, Vector2 const& b) { return Vector2Subtract(a, b); }
inline Vector2 operator *(Vector2 a, Vector2 const& b) { return Vector2Multiply(a, b); }
inline Vector2 operator /(Vector2 a, Vector2 const& b) { return Vector2Divide(a, b); }

inline Vector2 operator *(Vector2 a, float const& b) { return Vector2{ a.x * b, a.y * b }; }
inline Vector2 operator /(Vector2 a, float const& b) { return Vector2{ a.x / b, a.y / b }; }
inline Vector2 operator *(float b, Vector2 const& a) { return Vector2{ a.x * b, a.y * b }; }
inline Vector2 operator /(float b, Vector2 const& a) { return Vector2{ a.x / b, a.y / b }; }

inline Vector2 operator +=(Vector2& a, Vector2 const& b) { return a = (a + b); }
inline Vector2 operator -=(Vector2& a, Vector2 const& b) { return a = (a - b); }
inline Vector2 operator *=(Vector2& a, Vector2 const& b) { return a = (a * b); }
inline Vector2 operator /=(Vector2& a, Vector2 const& b) { return a = (a / b); }

inline Vector2 operator *=(Vector2& a, float const& b) { return a = a * b; }
inline Vector2 operator /=(Vector2& a, float const& b) { return a = a / b; }
inline Vector2 operator *=(const float& b, Vector2& a) { return a = a * b; }
inline Vector2 operator /=(const float& b, Vector2& a) { return a = a / b; }

// --- VECTOR3 --- //
inline Vector3 operator +(Vector3 a, Vector3 const& b) { return Vector3Add(a, b); }
inline Vector3 operator -(Vector3 a, Vector3 const& b) { return Vector3Subtract(a, b); }
inline Vector3 operator *(Vector3 a, Vector3 const& b) { return Vector3Multiply(a, b); }
inline Vector3 operator /(Vector3 a, Vector3 const& b) { return Vector3Divide(a, b); }

inline Vector3 operator *(Vector3 a, float const& b) { return Vector3 { a.x * b, a.y * b, a.z * b }; }
inline Vector3 operator /(Vector3 a, float const& b) { return Vector3 { a.x / b, a.y / b, a.z / b }; }
inline Vector3 operator *(float b, Vector3 const& a) { return Vector3 { a.x * b, a.y * b, a.z * b }; }
inline Vector3 operator /(float b, Vector3 const& a) { return Vector3 { a.x / b, a.y / b, a.z / b }; }

inline Vector3 operator +=(Vector3 a, Vector3 const& b) { return a + b; }
inline Vector3 operator -=(Vector3 a, Vector3 const& b) { return a - b; }
inline Vector3 operator *=(Vector3 a, Vector3 const& b) { return a * b; }
inline Vector3 operator /=(Vector3 a, Vector3 const& b) { return a / b; }

inline Vector3 operator *=(Vector3 a, float const& b) { return a * b; }
inline Vector3 operator /=(Vector3 a, float const& b) { return a / b; }
inline Vector3 operator *=(float b, Vector3 const& a) { return a * b; }
inline Vector3 operator /=(float b, Vector3 const& a) { return a / b; }

// Vector3 @ Vector2
inline Vector3 operator +(Vector3 a, Vector2 const& b) { return Vector3 { a.x + b.x, a.y + b.y, a.z }; }
inline Vector3 operator -(Vector3 a, Vector2 const& b) { return Vector3 { a.x - b.x, a.y - b.y, a.z }; }

inline Vector3 operator +=(Vector3 a, Vector2 const& b) { return a + b; }
inline Vector3 operator -=(Vector3 a, Vector2 const& b) { return a - b; }