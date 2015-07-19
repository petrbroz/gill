#include "core/parser.h"

using namespace std;
using namespace gill::core;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << "<scene_yaml_file>" << endl;
        return 0;
    }

    Parser parser(argv[1]);
    while (auto scene = parser.next_scene()) {
        scene->capture();
    }
    return 0;
}
