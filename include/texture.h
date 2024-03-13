#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
#include "color.h"

class texture {
  public:
    virtual ~texture() = default;
    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color: public texture {
    public:
        solid_color(color c): color_value(c) {}
        solid_color(double r, double g, double b): solid_color(color(r, g, b)) {}

        color value(double u, double v, const point3& p) const override {
            (void) u; (void) v; (void) p;
            return color_value;
        }
    
    private:
        color color_value;
};

class checker_texture : public texture {
    public:
        checker_texture(double _scale, shared_ptr<texture> _even, shared_ptr<texture> _odd)
            : inv_scale(1.0/_scale), even(_even), odd(_odd) {}
        checker_texture(double _scale, color c1, color c2): inv_scale(1.0/_scale) {
            even = make_shared<solid_color>(c1);
            odd = make_shared<solid_color>(c2);
        }

        color value(double u, double v, const point3& p) const override {
            auto xFloor = static_cast<int>(std::floor(inv_scale * p.x()));
            auto yFloor = static_cast<int>(std::floor(inv_scale * p.y()));
            auto zFloor = static_cast<int>(std::floor(inv_scale * p.z()));

            bool isEven = (xFloor + yFloor + zFloor) % 2 == 0;
            return isEven ? even->value(u, v, p) : odd->value(u, v, p);
        }

    private:
        double inv_scale;
        shared_ptr<texture> even;
        shared_ptr<texture> odd;
};

#endif