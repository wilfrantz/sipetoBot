
#include "sipeto.h"

int main(int argc, char **argv)
{
    spdlog::set_level(spdlog::level::debug);

    sipeto::Sipeto sipeto;
    sipeto.readInput();

    return 0;
}