#ifndef BOBOT_H
#define BOBOT_H

#include "header.h"
namespace bobot
{

    class Bobot
    {
    public:
        Bobot();
        Bobot(int bobot);
        ~Bobot() = default;
        int getBobot() const;
        void setBobot(int bobot);

    private:
        int _bobot;
    };

}
#endif /* BOBOT_H */
