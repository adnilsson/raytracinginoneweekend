//==================================================================================================
// Written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is distributed
// without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication along
// with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==================================================================================================

#define  _USE_MATH_DEFINES

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <cfloat>
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "utils.h"

typedef uint8_t byte;


vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) { 
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
             return attenuation.cwiseProduct(color(scattered, world, depth+1));
        }
        else {
            return vec3(0,0,0);
        }
    }
    else {
        vec3 unit_direction = vec3(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}


hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = utils::randf();
            vec3 center(a+0.9*utils::randf(),0.2,b+0.9*utils::randf()); 
            if ((center-vec3(4,0.2,0)).norm() > 0.9) { 
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(utils::randf()*utils::randf(), utils::randf()*utils::randf(), utils::randf()*utils::randf())));
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                            new metal(vec3(0.5*(1 + utils::randf()), 0.5*(1 + utils::randf()), 0.5*(1 + utils::randf())),  0.5*utils::randf()));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list,i);
}

int main() {
  const int RGB_CHANNELS = 3;

  int nx = 1200;
  int ny = 800;
  int ns = 10;
  byte *rgb_image = new byte[nx*ny*RGB_CHANNELS];


  hitable *list[5];
  list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
  list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
  list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
  list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
  list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
  hitable *world = new hitable_list(list, 5);
  world = random_scene();

  vec3 lookfrom(13, 2, 3);
  vec3 lookat(0, 0, 0);
  float dist_to_focus = 10.0;
  float aperture = 0.1;

  camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

  int i = 0;
  for (int iy = 0; iy < ny; iy++) {
    for (int ix = 0; ix < nx; ix++) {
      vec3 c(0, 0, 0);
      for (int is = 0; is < ns; is++) {
        float u = (float(ix) + utils::randf()) / float(nx);
        float v = (float(ny - iy - 1) + utils::randf()) / float(ny);

        ray r = cam.get_ray(u, v);
        vec3 p = r.point_at_parameter(2.0);
        c += color(r, world, 0);
      }

      c /= float(ns);
      rgb_image[i] = byte(sqrt(c.x()) * 255.99);
      rgb_image[i + 1] = byte(sqrt(c.y()) * 255.99);
      rgb_image[i + 2] = byte(sqrt(c.z()) * 255.99);
      i += RGB_CHANNELS;
    }
  }
  stbi_write_png("ch12.png", nx, ny, RGB_CHANNELS, rgb_image, 0);


  // de-allocation 
  delete[] rgb_image;
  delete world; //owns its list of Hitables and deletes them in its destructor
}

