#pragma once
/**
 * @brief
 * https://github.com/saubanbinusman/Cube3D/blob/master/main.cpp
 */

#include <math.h>
#include <vector>
#include <algorithm>
#include <cmath>

// #include <M5Core2.h>

// #include <LovyanGFX.hpp>

using namespace std;

#define X_AXIS 1
#define Y_AXIS 2
#define Z_AXIS 4

// Utility Functions
inline double mapDouble(double valToMap, double valMin, double valMax,
                        double mappedMin, double mappedMax) {
    return ((valToMap - valMin) / (valMax - valMin)) * (mappedMax - mappedMin) +
           mappedMin;
}

inline double round(double x) {
    return floor(x + 0.5);
}
inline double fPart(double x) {
    return x - floor(x);
}
inline double rfPart(double x) {
    return 1 - fPart(x);
}

inline void plot(int x, int y, double brightness, LGFX_Sprite& renderer) {
    renderer.drawPixel(x, y);
}
inline void plot(int x, int y, double brightness, uint8_t r, uint8_t g,
                 uint8_t b, LGFX_Sprite& renderer) {
    renderer.drawPixel(x, y, RGBColor(r, g, b));
}

inline void wuLine(double x0, double y0, double x1, double y1, uint8_t r,
                   uint8_t g, uint8_t b, LGFX_Sprite& renderer) {
    renderer.drawLine(x0, y0, x1, y1, RGBColor(r, g, b));
}
// inline void wuLine(double x0, double y0, double x1, double y1, uint8_t r,
// uint8_t g, uint8_t b, LGFX_Sprite& renderer)
// {
// 	if (isinf(x0) || isinf(y0) || isinf(x1) || isinf(y1)) return;

// 	bool steep = fabs(y1 - y0) > fabs(x1 - x0);

// 	if (steep)
// 	{
// 		swap(x0, y0);
// 		swap(x1, y1);
// 	}

// 	if (x0 > x1)
// 	{
// 		swap(x0, x1);
// 		swap(y0, y1);
// 	}

// 	double dx = x1 - x0;
// 	double dy = y1 - y0;

// 	double gradient = dx == 0.0  ? 1.0 : dy / dx;

// 	double xEnd = round(x0);
// 	double yEnd = y0 + gradient * (xEnd - x0);
// 	double xGap = rfPart(x0 + 0.5);
// 	double xPixel1 = xEnd;
// 	double yPixel1 = floor(yEnd);

// 	if (steep)
// 	{
// 		plot(yPixel1,     xPixel1, rfPart(yEnd) * xGap, r, g, b, renderer);
// 		plot(yPixel1 + 1, xPixel1,  fPart(yEnd) * xGap, r, g, b, renderer);
// 	}

// 	else
// 	{
// 		plot(xPixel1, yPixel1,    rfPart(yEnd) * xGap, r, g, b, renderer);
// 		plot(xPixel1, yPixel1 + 1, fPart(yEnd) * xGap, r, g, b, renderer);
// 	}

// 	double yIntersection = yEnd + gradient;

// 	xEnd = round(x1);
// 	yEnd = y1 + gradient * (xEnd - x1);
// 	xGap = fPart(x1 + 0.5);
// 	double xPixel2 = xEnd;
// 	double yPixel2 = floor(yEnd);

// 	if (steep)
// 	{
// 		plot(yPixel2,     xPixel2, rfPart(yEnd) * xGap, r, g, b, renderer);
// 		plot(yPixel2 + 1, xPixel2,  fPart(yEnd) * xGap, r, g, b, renderer);

// 		for (int x = xPixel1 + 1; x <= (xPixel2 - 1); x++)
// 		{
// 			plot(yIntersection    , x, rfPart(yIntersection), r, g, b,
// renderer); 			plot(yIntersection + 1, x,  fPart(yIntersection), r, g,
// b, renderer); 			yIntersection += gradient;
// 		}
// 	}

// 	else
// 	{
// 		plot(xPixel2, yPixel2,    rfPart(yEnd) * xGap, r, g, b, renderer);
// 		plot(xPixel2, yPixel2 + 1, fPart(yEnd) * xGap, r, g, b, renderer);

// 		for (int x = xPixel1 + 1; x <= (xPixel2 - 1); x++)
// 		{
// 			plot(x, yIntersection    , rfPart(yIntersection), r, g, b,
// renderer); 			plot(x, yIntersection + 1,  fPart(yIntersection), r, g,
// b, renderer); 			yIntersection += gradient;
// 		}
// 	}
// }

// Topics to Read:
// https://en.wikipedia.org/wiki/3D_projection#Perspective_projection
// https://en.wikipedia.org/wiki/Euler_angles#Tait%E2%80%93Bryan_angles
// https://www.siggraph.org/education/materials/HyperGraph/modeling/mod_tran/3drota.htm
// https://sites.google.com/site/glennmurray/Home/rotation-matrices-and-formulas/rotation-about-an-arbitrary-axis-in-3-dimensions

struct Vector2 {
    // x is the horizontal axis
    // y is the vertical axis
    double x;
    double y;

    Vector2(double x = 0.0, double y = 0.0) {
        this->x = x;
        this->y = y;
    }

    // friend ostream& operator<<(ostream& stream, const Vector2& vector)
    // {
    // 	stream << "Vector2(" << (vector.x) << ", " << (vector.y) << ")";
    // 	return stream;
    // }
};

struct Vector3 {
    // z is the depth
    double x;
    double y;
    double z;

    Vector3(double x = 0.0, double y = 0.0, double z = 0.0) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    double getMagnitude() {
        return hypot(hypot(x, y), z);
    }

    Vector3 getUnitVector() {
        return Vector3(*this) / (this->getMagnitude());
    }

    // friend ostream& operator<<(ostream& stream, const Vector3& vector)
    // {
    // 	stream << "Vector3(" << (vector.x) << ", " << (vector.y) << ", " <<
    // (vector.z) << ")"; 	return stream;
    // }

    friend Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
        return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    friend Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
        return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    friend Vector3 operator*(const Vector3& lhs, const double& rhs) {
        return Vector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
    }

    // Cross Product
    friend Vector3 operator*(const Vector3& lhs, const Vector3& rhs) {
        return Vector3((lhs.y * rhs.z) - (lhs.z * rhs.y),
                       (lhs.z * rhs.x) - (lhs.x * rhs.z),
                       (lhs.x * rhs.y) - (lhs.y * rhs.x));
    }

    double dot(const Vector3& lhs, const Vector3& rhs) {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
    }

    friend Vector3 operator/(const Vector3& lhs, const double& rhs) {
        return Vector3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
    }

    friend void operator+=(Vector3& lhs, const Vector3& rhs) {
        lhs = lhs + rhs;
    }

    friend void operator-=(Vector3& lhs, const Vector3& rhs) {
        lhs = lhs - rhs;
    }
};

struct Cube2D {
    // A 2D Cube is a projection (perspective in our case) of a 3D Cube into 2D
    // space It is defined using 8 points in 2D-Space

    Vector2 point[8];

    Cube2D(Vector2 data[8] = nullptr) {
        if (data == nullptr) {
            point[0] = Vector2(0.0, 0.0);
            point[1] = Vector2(1.0, 0.0);
            point[2] = Vector2(1.0, 1.0);
            point[3] = Vector2(0.0, 1.0);

            point[4] = Vector2(0.0, 0.0);
            point[5] = Vector2(1.0, 0.0);
            point[6] = Vector2(1.0, 1.0);
            point[7] = Vector2(0.0, 1.0);
        }

        else {
            for (int i = 0; i < 8; ++i) {
                point[i] = data[i];
            }
        }
    }
};

struct Cube3D {
    // A 3D Cube can be defined using 8 points in 3D-Space
    // point[0] is the front-bottom-left point,
    // and point[{1, 2, 3}] follow in counter-clock-wise order
    // point[4] is the back-bottom-left point,
    // and point[{5, 6, 7}] follow in counter-clock-wise order
    Vector3 point[8];

    Cube3D(Vector3 data[8] = nullptr) {
        if (data == nullptr) {
            point[0] = Vector3(0.0, 0.0, 0.0);
            point[1] = Vector3(1.0, 0.0, 0.0);
            point[2] = Vector3(1.0, 1.0, 0.0);
            point[3] = Vector3(0.0, 1.0, 0.0);

            point[4] = Vector3(0.0, 0.0, 1.0);
            point[5] = Vector3(1.0, 0.0, 1.0);
            point[6] = Vector3(1.0, 1.0, 1.0);
            point[7] = Vector3(0.0, 1.0, 1.0);
        }

        else {
            for (int i = 0; i < 8; ++i) {
                point[i] = data[i];
            }
        }
    }

    Vector3 getCenter() {
        Vector3 center;

        for (const auto& p : point) {
            center += p;
        }

        return (center / 8.0);
    }

    void translate(Vector3 distance) {
        for (Vector3& p : point) {
            p += distance;
        }
    }

    // Rotates the point 'p' about the line through 'q' with direction (unit)
    // vector 'd' by the angle theta (radians).
    inline Vector3 rotate(const Vector3& p, const Vector3& q, const Vector3& d,
                          double theta) {
        // For readability
        double x = p.x, y = p.y, z = p.z;
        double a = q.x, b = q.y, c = q.z;
        double u = d.x, v = d.y, w = d.z;

        double t1 = u * x - v * y - w * z;  // Reusable Term 1
        double t2 = 1.0 - cos(theta);       // Reusable Term 2

        return {
            (a * (v * v + w * w) - u * (b * v + c * w - t1)) * t2 +
                x * cos(theta) + (-c * v + b * w - w * y + v * z) * sin(theta),
            (b * (u * u + w * w) - v * (a * u + c * w - t1)) * t2 +
                y * cos(theta) + (c * u - a * w + w * x - u * z) * sin(theta),
            (c * (u * u + v * v) - w * (a * u + b * v - t1)) * t2 +
                z * cos(theta) + (-b * u + a * v - v * x + u * y) * sin(theta)};
    }

    void rotateAbout(int axis, double theta) {
        Vector3 p1, p2;

        switch (axis) {
            case X_AXIS:
                p1 = (point[0] + point[3] + point[4] + point[7]) / 4.0;
                p2 = (point[1] + point[2] + point[5] + point[6]) / 4.0;
                break;

            case Y_AXIS:
                p1 = (point[0] + point[1] + point[4] + point[5]) / 4.0;
                p2 = (point[2] + point[3] + point[6] + point[7]) / 4.0;
                break;

            case Z_AXIS:
                p1 = (point[0] + point[1] + point[2] + point[3]) / 4.0;
                p2 = (point[4] + point[5] + point[6] + point[7]) / 4.0;
                break;
        }

        Vector3 d = (p2 - p1).getUnitVector();

        for (Vector3& p : point) {
            p = rotate(p, p1, d, theta / 180.0 * M_PI);
        }
    }
};

// Our projection considers a camera with default orientation of <0, 0, 0>
// The camera is at <0, 0, 0>
// Ref: https://en.wikipedia.org/wiki/3D_projection#Perspective_projection
// Ref: https://en.wikipedia.org/wiki/3D_projection#Diagram
Cube2D perspectiveProjection(const Cube3D& cube, const Vector3& camera,
                             double screenWidth, double screenHeight) {
    static const Vector3 e =
        Vector3(0.0, 0.0, 1.0);  // Corresponds to 90 degrees field of view

    Cube2D cube2D;

    for (int i = 0; i < 8; ++i) {
        Vector3 d = cube.point[i] - camera;

        double bx = ((e.z / d.z) * d.x) - e.x;
        double by = ((e.z / d.z) * d.y) - e.y;

        cube2D.point[i].x = mapDouble(bx, -1.0, 1.0, 0.0, screenWidth);
        cube2D.point[i].y = mapDouble(by, -1.0, 1.0, 0.0, screenHeight);
    }

    return cube2D;
}

void drawCube2D(const Cube2D& cube, LGFX_Sprite& renderer) {
    renderer.fillScreen(TFT_BLACK);

    for (int i = 0, j = 4; i < 2 && j < 6; ++i, ++j) {
        wuLine(cube.point[i].x, cube.point[i].y, cube.point[j].x,
               cube.point[j].y, 0xFF, 0xFF, 0x00, renderer);
    }

    for (int i = 2, j = 6; i < 4 && j < 8; ++i, ++j) {
        wuLine(cube.point[i].x, cube.point[i].y, cube.point[j].x,
               cube.point[j].y, 0x00, 0x9F, 0xFF, renderer);
    }

    for (int i = 0; i < 4; ++i) {
        wuLine(cube.point[i].x, cube.point[i].y,
               cube.point[i + 1 == 4 ? 0 : i + 1].x,
               cube.point[i + 1 == 4 ? 0 : i + 1].y, 0xFF, 0x00, 0x00,
               renderer);
    }

    for (int i = 4; i < 8; ++i) {
        wuLine(cube.point[i].x, cube.point[i].y,
               cube.point[i + 1 == 8 ? 4 : i + 1].x,
               cube.point[i + 1 == 8 ? 4 : i + 1].y, 0x00, 0xFF, 0x00,
               renderer);
    }
}

/**
 * @brief
 * draw cube to lcd
 *
 * @param roll
 * rotate roll (degree)
 * @param pitch
 * rotate pitch (degree)
 * @param yaw
 * rotate yaw (degree)
 * @param x
 * transition x (disabled)
 * @param y
 * transition y (disabled)
 * @param z
 * transision z (disabled)
 * @param lcd
 * LCD instance
 */
inline void cube(float roll, float pitch, float yaw, float x, float y, float z,
                 LGFX_Sprite lcd) {
    static Vector3 cube3D[8];
    static Cube3D cube;
    static float old_roll;
    static float old_pitch;
    static float old_yaw;
    static bool first_flag = false;
    static Vector3 camLoc(2.5, 2.5, -2.5);

    if (!first_flag) {
        cube3D[0] = Vector3(0.0, 0.0, 5.0);
        cube3D[1] = Vector3(5.0, 0.0, 5.0);
        cube3D[2] = Vector3(5.0, 5.0, 5.0);
        cube3D[3] = Vector3(0.0, 5.0, 5.0);

        cube3D[4]  = Vector3(0.0, 0.0, 10.0);
        cube3D[5]  = Vector3(5.0, 0.0, 10.0);
        cube3D[6]  = Vector3(5.0, 5.0, 10.0);
        cube3D[7]  = Vector3(0.0, 5.0, 10.0);
        cube       = Cube3D(cube3D);
        first_flag = true;

        old_roll  = roll;
        old_pitch = pitch;
        old_yaw   = yaw;
    }

    float diff_roll  = roll - old_roll;
    float diff_pitch = pitch - old_pitch;
    float diff_yaw   = yaw - old_yaw;
    old_roll         = roll;
    old_pitch        = pitch;
    old_yaw          = yaw;
    cube.rotateAbout(X_AXIS, diff_roll);
    cube.rotateAbout(Y_AXIS, diff_pitch);
    cube.rotateAbout(Z_AXIS, diff_yaw);
    camLoc = cube.getCenter();
    camLoc.z -= fabs(cube.point[0].z - cube.point[4].z) + 5.0;
    // cube.translate(Vector3(x*0.5,y*0.5,(z-1.06)*0.5));
    drawCube2D(perspectiveProjection(cube, camLoc, lcd.width(), lcd.height()),
               lcd);
}