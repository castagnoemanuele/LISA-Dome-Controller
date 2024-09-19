/// \file telescope.h
/// \brief This file contains the Telescope class declaration, which includes functions to check the telescope position, convert Right Ascension (RA) from HH:MM:SS format to degrees, and get the telescope position.

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


