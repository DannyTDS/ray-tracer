#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "utils.h"
#include "hittable.h"
#include "hittables.h"
#include "sphere.h"

#include <iostream>

color ray_color(const ray& r, const hittable& world) {
	hit_record rec;
	if (world.hit(r, interval(0, infinity), rec)) {
		return 0.5 * (rec.normal + color(1, 1, 1));
	}

	vec3 unit_direction = unit_vector(r.direction());
	// Lerp color
	auto a = 0.5*(unit_direction.y() + 1.0);
	return (1.0-a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}


int main() {
	// Image
	auto aspect_ratio = 16.0 / 9.0;
	int image_width = 400;

	// Compute image height, which is integer and at least 1
	int image_height = static_cast<int>(image_width / aspect_ratio);
	image_height = max(1, image_height);

	// World
	hittable_list world;
	world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
	world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

	// Camera (viewport + camera center)
	auto focal_length = 1.0;			// Distance from cam center to viewport
	auto viewport_height = 2.0;
	auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
	auto camera_center = point3(0, 0, 0);

	// Compute vectors across the horizontal and down the vertical viewport edges
	auto viewport_u = vec3(viewport_width, 0, 0);		// Horizontal
	auto viewport_v = vec3(0, -viewport_height, 0);		// Vertical, negative y direction

	// Delta vectors: spacing between pixels
	auto pixel_delta_u = viewport_u / image_width;
	auto pixel_delta_v = viewport_v / image_height;

	// Compute location of upper left pixel (pixel coord 0,0)
	// Very upper left corner of the viewport
	auto viewport_upper_left = camera_center - vec3(0,0,focal_length) - viewport_u/2 - viewport_v/2;
	// Offset by half a pixel to get pixel00's location
	auto pixel00_loc = viewport_upper_left + 0.5*(pixel_delta_u + pixel_delta_v);
	
	// Render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
	
	for (int j=0; j<image_height; ++j) {
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
		for (int i=0; i<image_width; ++i) {
			auto pixel_center = pixel00_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
			// Points from cam center to pixel center
			auto ray_direction = pixel_center - camera_center;

			ray r(camera_center, ray_direction);

			// Determine pixel color by ray tracing
			color pixel_color = ray_color(r, world);
			write_color(std::cout, pixel_color);
		}
	}

	std::clog << "\rDone.                 \n";
}
