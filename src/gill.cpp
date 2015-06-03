#include "ray.h"
#include "sphere.h"

using namespace gill::core;
using namespace gill::geometry;

const int width = 512;
const int height = 512;

int main(int argc, char *argv[]) {
    Ray ray(Point(0.0f), Vector(0.0f));
    const Transform ltow = Transform::Translate(0.0, 0.0, +96.0);
    const Transform wtol = Transform::Translate(0.0, 0.0, -96.0);
    Sphere geometry(75, &ltow, &wtol);

    std::cout << "P3" << std::endl;
    std::cout << width << " " << height << std::endl;
    std::cout << "255" << std::endl;
    Hit hit;
    for (int y = -height/2; y < height/2; y++) {
        for (int x = -width/2; x < width/2; x++) {
            ray.d = normalize(Point(x, y, 128.0) - ray.o);
            if (geometry.intersect(ray, &hit)) {
                std::cout
                    << (int)((hit.n.x + 1.0) * 128) << " "
                    << (int)((hit.n.y + 1.0) * 128) << " "
                    << (int)((hit.n.z + 1.0) * 128) << " ";
            } else {
                std::cout << "0 0 0 ";
            }
        }
        std::cout << std::endl;
    }
    return 0;
}
