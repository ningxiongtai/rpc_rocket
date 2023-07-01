#ifndef ROCKET_COMMON_RUN_TIME_H
#define ROCKET_COMMON_RUN_TIME_H


#include <string>
namespace rocket {

class Runtime {

public:
    static Runtime* GetRunTime();
                

public:
    std::string m_msgid;
    std::string m_method_name;

};




}



#endif