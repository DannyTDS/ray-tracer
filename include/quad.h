#ifndef QUAD_H
#define QUAD_H

#include "utils.h"
#include "hittable.h"
#include "hittable_list.h"

class quad : public hittable {
    public:
        quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
            : Q(Q), u(u), v(v), mat(mat) {
                set_bounding_box();
                // Compute plane parameters
                auto n = cross(u, v);           // not normalized
                normal = unit_vector(n);
                D = dot(normal, Q);
                w = n / dot(n, n);
            }

        aabb bounding_box() const override {
            return bbox;
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // If ray is (near) parallel to the plane,
            auto denom = dot(r.direction(), normal);
            if (fabs(denom) < 1e-8) {
                return false;
            }

            // Solve for intersection t
            auto t = (D - dot(r.origin(), normal)) / denom;
            if (!ray_t.contains(t)) {
                return false;
            }
            auto intersection = r.at(t);

            // Basis factorization
            auto p = intersection - Q;
            auto alpha = dot(w, cross(p, v));
            auto beta  = dot(w, cross(u, p));
            if (!is_interior(alpha, beta, rec)) {
                return false;
            }

            // Intersection point falls inside the shape
            rec.t = t;
            rec.p = intersection;
            rec.mat = mat;
            rec.set_face_normal(r, normal);

            return true;
        }

        virtual void set_bounding_box() {
            auto bbox_diag1 = aabb(Q, Q + u + v);
            auto bbox_diag2 = aabb(Q + u, Q + v);
            bbox = aabb(bbox_diag1, bbox_diag2);
        }

        virtual bool is_interior(double alpha, double beta, hit_record& rec) const {
            if (!interval::unit.contains(alpha) || !interval::unit.contains(beta)) {
                return false;
            }

            rec.u = alpha;
            rec.v = beta;
            return true;
        }

    private:
        point3 Q;
        vec3 u, v;
        vec3 w;                 // For basis factorization
        shared_ptr<material> mat;
        aabb bbox;
        vec3 normal;            // Normal to the plane
        double D;               // Distance from the origin to the plane
};


inline shared_ptr<hittable_list> box(const point3&a, const point3&b, shared_ptr<material> mat) {
    // Compute opposite vertices
    auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));
    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    // Construct 6 sides
    auto sides = make_shared<hittable_list>();
    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat));    // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat));    // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat));    // left
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat));    // right
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat));    // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat));    // bottom
    return sides;
}

#endif