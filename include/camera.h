#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"
#include "color.h"
#include "hittable.h"
/* Needed to resolve IDE warning */
#include "material.h"

#include <iostream>

class camera {
  public:
    /* Public Camera Parameters Here */

    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width  = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;  // Count of random samples for each pixel
    int    max_depth = 10;      // Maximum number of rays bouncing / reflecting
    color  background = color(0.70, 0.80, 1.00);              // Scene background color

    double vfov = 90;                   // Vertical view angle
    point3 lookfrom = point3(0,0,-1);   // Camera position (looking from)
    point3 lookat = point3(0,0,0);      // Camera facing (look at)
    vec3 vup = vec3(0,1,0);             // Camera-relative up direction. Using positive global y base

    double defocus_angle = 0;       // Variation angle of rays
    double focus_dist = 10;         // Distance from lookfrom to focus plane

    /**
     * @brief Render the image and output a ppm-coded image format to std::cout
     * 
     * @param world Objects to be checked for hitting inside the scene
     */
    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0,0,0);
                for (int sample=0; sample<samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_color, samples_per_pixel);
            }
        }

        std::clog << "\rDone.                 \n";
    }

  private:
    /* Private Camera Variables Here */

    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    vec3   u, v, w;        // Camera frame basis
    vec3   defocus_disk_u;
    vec3   defocus_disk_v;

    /**
     * @brief Compute the necessary private fields for the camera to render an image
     * 
     */
    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        // Determine viewport dimensions.
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

        // Compute camera frame basis
        w = unit_vector(lookfrom - lookat);     // Camera facing -w direction
        u = unit_vector(cross(vup, w));         // Pointing camera right
        v = cross(w, u);                        // Pointing camera up

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v; // Viewport basis defined as -v direction

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left =
            center - focus_dist * w - viewport_u/2 - viewport_v/2;
        // Define pixels at middle of each u-v grid
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate defocus disk basis
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    /**
     * @brief Cast the ray into the scene and determine the color at this pixel
     * 
     * @param r Ray to be casted
     * @param world Objects to be checked for hitting against the ray
     * @return Color to be displayed for this ray (pixel)
     */
    color ray_color(const ray& r, int depth, const hittable& world) const {
        hit_record rec;

        if (depth <= 0)
            return color(0,0,0);        // Assume all light is absorbed after max_depth

        // Set tmin=0.001 to ignore possible ray origins below the surface due to round off errors
        // aka "shadow acne"
        if (!world.hit(r, interval(0.001, infinity), rec)) {
            // Return background color if doesn't hit anything in the scene
            return background;
        }

        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered)) {
            // If object doesn't scatter (ie, is a light source)
            return color_from_emission;
        }

        color color_from_scatter = attenuation * ray_color(scattered, depth-1, world);
        return color_from_emission + color_from_scatter;
    }

    /**
     * @brief Get a randomly sampled ray for the pixel at location i, j
     * originating from the defocus disk
     * 
     */
    ray get_ray(int i, int j) const {
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    /**
     * @brief Returns a random point in the square surrounding a pixel at the origin
     * 
     */
    vec3 pixel_sample_square() const {
        auto px = -0.5 + random_double();       // random_double returns [0, 1)
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    point3 defocus_disk_sample() const {
        // Returns random point in defocus disk
        auto p = random_in_unit_disk();
        return center + p[0]*defocus_disk_u + p[1]*defocus_disk_v;
    }
};

#endif