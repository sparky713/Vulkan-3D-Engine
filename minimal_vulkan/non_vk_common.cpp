#include "non_vk_common.h"

RenderInfo::RenderInfo() {}

MyVertex::MyVertex() {}

MyVertex::MyVertex(double x, double y, double z) : x(x), y(y), z(z) {}

File::File() {}

void File::print() {
    std::cout << "File::print()================================================= start" << std::endl;
    std::cout << "fullPath: " << fullPath << std::endl;
    std::cout << "filename: " << filename << std::endl;
    std::cout << "fileType: " << fileType << std::endl;
    std::cout << "File::print()================================================= end" << std::endl;
}
