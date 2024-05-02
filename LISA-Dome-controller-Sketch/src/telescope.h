#ifndef TELESCOPE_H
#define TELESCOPE_H

class Telescope {
    private:
        int telescopePosition;
    public:
        void checkTelescopePosition();
        int convertRA(char* ra);
        int getTelescopePosition();

};

#endif // TELESCOPE_H


