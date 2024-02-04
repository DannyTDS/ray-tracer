#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class hit_record {
  public:
    point3 p;           // Point of hit
    vec3 normal;        // Surface normal
    double t;           // Linear factor along the casted ray
    bool front_face;    // Which side of the surface did the ray hit

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // Sets the hit record normal vector
        front_face = dot(r.direction(), outward_normal) < 0;
        // We configure the surface normal to always point against the casted ray
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
  public:
    virtual ~hittable() = default;

    // tmin and tmax are helpful in determining the closest object hit along the ray
    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif