#include "rocket/common/run_time.h"

namespace rocket {


thread_local Runtime* t_run_time = NULL;

Runtime* Runtime::GetRunTime() {
    if(t_run_time) {
        return t_run_time;
    }
    t_run_time = new Runtime();
    return t_run_time;

}



}