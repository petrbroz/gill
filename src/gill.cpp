#include "primitive.h"
#include "mesh.h"
#include "ray.h"

using namespace std;
using namespace gill::core;

const int frame_minx = -128;
const int frame_maxy = 128;
const int frame_maxx = 128;
const int frame_miny = -128;

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
    float t;

    std::cout << "P3" << std::endl;
    std::cout << (frame_maxx - frame_minx) << " " << (frame_maxy - frame_miny) << std::endl;
    std::cout << "255" << std::endl;
    for (int y = frame_maxy; y > frame_miny; y--) {
        for (int x = frame_minx; x < frame_maxx; x++) {
            ray.d = normalize(Point(x, y, 64.0) - ray.o);
            if (primitive.intersect(ray, t, nullptr)) {
                int c = (int)t * 3;
                if (c < 0) c = 0;
                if (c > 255) c = 255;
                std::cout << c << " " << c << " " << c << " ";
            } else {
                std::cout << "0 0 0 ";
            }
        }
        std::cout << std::endl;
    }

    delete mesh;
    return 0;
}
