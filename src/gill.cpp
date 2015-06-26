#include "primitive.h"
#include "mesh.h"
#include "ray.h"

using namespace std;
using namespace gill::core;

const int width = 512;
const int height = 512;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: gill <file>" << endl;
        cout << "<file>\tPath to an .obj file" << endl;
        return 0;
    }

    const Transform ltow = Transform::Translate(0.0, 0.0, +64.0);
    const Transform wtol = Transform::Translate(0.0, 0.0, -64.0);
    const Mesh *mesh = Mesh::from_obj_file(argv[1]);
    Primitive primitive(mesh, &ltow, &wtol);
    Ray ray(Point(0.0f), Vector(0.0f));

    std::cout << "P3" << std::endl;
    std::cout << width << " " << height << std::endl;
    std::cout << "255" << std::endl;
    for (int y = +height/2; y > -height/2; y--) {
        for (int x = -width/2; x < width/2; x++) {
            ray.d = normalize(Point(x, y, 64.0) - ray.o);
            if (primitive.intersect(ray, nullptr)) {
                std::cout << "255 255 255 ";
            } else {
                std::cout << "0 0 0 ";
            }
        }
        std::cout << std::endl;
    }

    delete mesh;
    return 0;
}
