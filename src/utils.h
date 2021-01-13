//
// Created by Christoph on 22.10.20.
//

#ifndef POWERWALL_UTILS_H
#define POWERWALL_UTILS_H

#include <cmath>
using std::sin;
using std::cos;

// set_euler_yxz expects a vector containing the Euler angles in the format
// (ax,ay,az), where ax is the angle of rotation around x axis,
// and similar for other axes.
// This implementation uses YXZ convention (Z is the first rotation).
void quat_set_euler_yxz(godot_quat *quat, const godot_vector3 &p_euler) {
    double half_a1 = api->godot_vector3_get_axis(&p_euler, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y) * 0.5;
    double half_a2 = api->godot_vector3_get_axis(&p_euler, godot_vector3_axis::GODOT_VECTOR3_AXIS_X) * 0.5;
    double half_a3 = api->godot_vector3_get_axis(&p_euler, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z) * 0.5;

    // R = Y(a1).X(a2).Z(a3) convention for Euler angles.
    // Conversion to quaternion as listed in https://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770024290.pdf (page A-6)
    // a3 is the angle of the first rotation, following the notation in this reference.

    double cos_a1 = cos(half_a1);
    double sin_a1 = sin(half_a1);
    double cos_a2 = cos(half_a2);
    double sin_a2 = sin(half_a2);
    double cos_a3 = cos(half_a3);
    double sin_a3 = sin(half_a3);

    api->godot_quat_set_x(quat, sin_a1 * cos_a2 * sin_a3 + cos_a1 * sin_a2 * cos_a3);
    api->godot_quat_set_y(quat, sin_a1 * cos_a2 * cos_a3 - cos_a1 * sin_a2 * sin_a3);
    api->godot_quat_set_z(quat, -sin_a1 * sin_a2 * cos_a3 + cos_a1 * cos_a2 * sin_a3);
    api->godot_quat_set_w(quat, sin_a1 * sin_a2 * sin_a3 + cos_a1 * cos_a2 * cos_a3);
}


void TransformToCam(const godot_transform& in, godot_real *p_projection) {
    godot_basis basis;
    api->godot_basis_new(&basis);
    godot_vector3 r1, r2, r3, off;
    api->godot_vector3_new(&off, 0, 0, 0);

    basis = api->godot_transform_get_basis(&in);
    r1 = api->godot_basis_get_row(&basis, 0);
    r2 = api->godot_basis_get_row(&basis, 1);
    r3 = api->godot_basis_get_row(&basis, 2);
    off = api->godot_transform_get_origin(&in);

    p_projection[0] = api->godot_vector3_get_axis(&r1, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[1] = api->godot_vector3_get_axis(&r1, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[2] = api->godot_vector3_get_axis(&r1, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[3] = 0;

    p_projection[4] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[5] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[6] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[7] = 0;

    p_projection[8] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[9] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[10] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[11] = -1;

    p_projection[12] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[13] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[14] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[15] = 0;
}

void TransformToCamT(const godot_transform& in, godot_real *p_projection) {
    godot_basis basis;
    api->godot_basis_new(&basis);
    godot_vector3 r1, r2, r3, off;
    api->godot_vector3_new(&off, 0, 0, 0);

    basis = api->godot_transform_get_basis(&in);
    r1 = api->godot_basis_get_row(&basis, 0);
    r2 = api->godot_basis_get_row(&basis, 1);
    r3 = api->godot_basis_get_row(&basis, 2);
    off = api->godot_transform_get_origin(&in);

    p_projection[0] = api->godot_vector3_get_axis(&r1, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[1] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[2] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[3] = 0;

    p_projection[4] = api->godot_vector3_get_axis(&r1, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[5] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[6] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[7] = 0;

    p_projection[8] = api->godot_vector3_get_axis(&r1, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[9] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[10] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[11] = -1;

    p_projection[12] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[13] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[14] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[15] = 0;
}

void printVector(godot_vector3 *v) {
    printf("[%f, %f, %F]\n",
           api->godot_vector3_get_axis(v, godot_vector3_axis::GODOT_VECTOR3_AXIS_X),
           api->godot_vector3_get_axis(v, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y),
           api->godot_vector3_get_axis(v, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z));
}

void printRect(godot_rect2 *r) {
    godot_vector2 p = api->godot_rect2_get_position(r);
    godot_vector2 s = api->godot_rect2_get_size(r);
    printf("[%f, %f (%f, %f)]\n",
           api->godot_vector2_get_x(&p),
           api->godot_vector2_get_y(&p),
           api->godot_vector2_get_x(&s),
           api->godot_vector2_get_y(&s));
}

void printCam(godot_real *m) {
    printf("%f %f %f %f\n", m[0],m[4], m[8], m[12]);
    printf("%f %f %f %f\n", m[1],m[5], m[9], m[13]);
    printf("%f %f %f %f\n", m[2],m[6], m[10], m[14]);
    printf("%f %f %f %f\n", m[3],m[7], m[11], m[15]);
}

void printTransform (godot_transform &p_transform) {
    float projectionMatrix[16];
    TransformToCam(p_transform, projectionMatrix);
    // overwrite the camera matrix specific values
    projectionMatrix[11] = 1;
    projectionMatrix[15] = 0;
    printCam(projectionMatrix);
}

#endif //POWERWALL_UTILS_H
