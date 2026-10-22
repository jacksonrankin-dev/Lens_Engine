
#include <memory>
#include <vector>

#include "camera.hpp"
#include "emmisive.hpp"
#include "lambertian.hpp"
#include "mirror.hpp"
#include "mat_list.hpp"
#include "optic.hpp"
#include "plane.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "srd.hpp"
#include "vec3.hpp"

int main() {
    // ************************************************ Front Standard *************************************************
    auto optic = std::make_shared<Pinhole>(0.0);
    FrontStandard front_standard(optic);

    // ************************************************* Rear Standard *************************************************
    double width = 36.0e-3;
    double height = 24.0e-3;

    int res_width = 1200;

    RearStandard rear_standard(width, height, res_width);

    // ********************************************** Camera Construction **********************************************
    Vec3 pos(0.0, -5.0, 1.5);
    Vec3 target(0.0, 0.0, 0.5);
    Vec3 up(0.0, 0.0, 1.0);

    double standard_separation = 25.0e-3;

    Camera cam(pos, target, up, front_standard, rear_standard, standard_separation);

    // Apply movements
    cam.front_standard_.SetMovements(0.0, 0.0, 0.0, 0.0, 0.0);
    cam.front_standard_.ApplyMovements();

    cam.rear_standard_.SetMovements(0.0, 0.0, 0.0, 0.0, 0.0);
    cam.rear_standard_.ApplyMovements();

    // ********************************************** Scene Construction ***********************************************
    std::vector<std::shared_ptr<Object>> objects;

    auto      p1 = std::make_shared<Plane>(Vec3(), Vec3(0.0, 0.0,  1.0), Neutral3_5, std::make_shared<Lambertian>(1.0));
    auto      s1 = std::make_shared<Sphere>(Vec3(0.0, 0.0, 0.5),   0.5,  Green,      std::make_shared<Mirror>(1.0, 0.0));
    auto      s2 = std::make_shared<Sphere>(Vec3(0.0, 0.0, 1.25),  1.25, White,      N_SF11);
    auto      s3 = std::make_shared<Sphere>(Vec3(1.5, 0.0, 0.5),   0.5,  Blue,       std::make_shared<Lambertian>(1.0));
    auto diffuse = std::make_shared<Sphere>(Vec3(5.0, 5.0, 10.0),   1.0,  White,      std::make_shared<Emmisive>(1e2));

    objects.push_back(p1);
    // objects.push_back(s1);
    objects.push_back(s2);
    // objects.push_back(s3);
    objects.push_back(diffuse);

    // **************************************************** Render *****************************************************
    Scene scene(cam, objects, 100, 8, 40);

    scene.RenderImage("test.ppm", 10);
}
