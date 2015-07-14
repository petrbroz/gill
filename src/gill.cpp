#include "parser.h"
#include "primitive.h"
#include "mesh.h"
#include "ray.h"

using namespace std;
using namespace gill;
using namespace gill::core;

const int frame_minx = -128;
const int frame_maxy = 128;
const int frame_maxx = 128;
const int frame_miny = -128;

int main(int argc, char *argv[]) {
    Parser parser;
    while (auto scene = parser.next_scene()) {
        cerr << *scene << endl;
        scene->capture();
    }
    return 0;

    /*
    if (argc < 2) {
        cout << "Usage: gill <file>" << endl;
        cout << "<file>\tPath to an .obj file" << endl;
        return 0;
    }

    auto ltow = Transform::translate(0.0, 0.0, 0.0);
    auto wtol = Transform::translate(0.0, 0.0, 0.0);
    auto mesh = Mesh::from_obj_file(argv[1]);
    Primitive primitive(mesh, ltow, wtol);
    Ray ray(Point(0.0f, 16.0f, 64.0f), Vector(0.0f));
    Primitive::Intersection pi;
    float t;

    cout << "P3" << endl;
    cout << (frame_maxx - frame_minx) << " " << (frame_maxy - frame_miny) << endl;
    cout << "255" << endl;
    for (int y = frame_maxy; y > frame_miny; y--) {
        for (int x = frame_minx; x < frame_maxx; x++) {
            ray.d = normalize(Point(x, y, 0.0f) - ray.o);
            if (primitive.intersect(ray, t, &pi)) {
                cout << (int)(pi.mi.triangle_isec.n.x * 128 + 127) << " ";
                cout << (int)(pi.mi.triangle_isec.n.y * 128 + 127) << " ";
                cout << (int)(pi.mi.triangle_isec.n.z * 128 + 127) << " ";
            } else {
                cout << "0 0 0 ";
            }
        }
        cout << endl;
    }

    return 0;
    */
}
