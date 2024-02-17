#include "camera.h"
#include "hittables.h"
#include "sphere.h"

#include <iostream>

int main() {
	// World
	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = make_shared<dialectric>(1.5);
	auto material_left   = make_shared<dialectric>(1.5);
	auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

	world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
	world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

	// Camera
	camera cam;
	cam.aspect_ratio = 16.0/9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 50;

	cam.render(world);
}
