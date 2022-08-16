#ifndef CIA402DRIVEHANDLER_H
#define CIA402DRIVEHANDLER_H
#include "GenericCIA402Drive.h"

class CIA402DriveHandler{
public:
    CIA402DriveHandler();
    void registerDrive(const GenericCIA402Drive& drive);
    void unregisterDrive(const GenericCIA402Drive& drive);

    void run();

private:

};
#endif
