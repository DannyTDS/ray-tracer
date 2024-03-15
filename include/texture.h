#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
#include "color.h"
#include "rtw_stb_image.h"

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
        checker_texture(double _scale, color c1, color c2)
            : inv_scale(1.0/_scale),
            even(make_shared<solid_color>(c1)),
            odd(make_shared<solid_color>(c2))
        {}

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

class image_texture : public texture {
  public:
    image_texture(const char* filename) : image(filename) {}

    color value(double u, double v, const point3& p) const override {
        (void) p;   // Suppress unused parameter warning
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image.height() <= 0) return color(0,1,1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = interval(0,1).clamp(u);
        v = 1.0 - interval(0,1).clamp(v);  // Flip V to image coordinates

        // u in [0,1] leads to i in [0,width-1], same for v and j
        auto i = static_cast<int>(u * image.width());
        auto j = static_cast<int>(v * image.height());
        auto pixel = image.pixel_data(i,j);

        auto color_scale = 1.0 / 255.0;
        return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }

  private:
    rtw_image image;
};

#endif