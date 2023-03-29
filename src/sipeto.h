#ifndef SIPETO_H
#define SIPETO_H

#include "header.h"

namespace sipeto
{

    class Sipeto
    {
    public:
        Sipeto(){};
        ~Sipeto() = default;

        void readUserInput();
        void returnMediaToUser();

    private:
        const std::string _version = "1.0.0";
    };

}
#endif // SIPETO_H
