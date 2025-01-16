#include "VirtualDisk.h"

int main() {
    VirtualDisk virtualDisk(67108864);
    virtualDisk.saveToFile("disk");
    VirtualDisk virtualDisk2 = VirtualDisk::loadFromFile("disk");
}
