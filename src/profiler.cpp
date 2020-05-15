#include "profiler.hpp"

Profiler::Profiler(){
    m_logger = new EasyLogger("profiler.log");
}

Profiler::Profiler(string hdt_file_dir){
    m_hdt = hdt::HDTManager::mapIndexedHDT(hdt_file_dir.c_str());
    m_logger = new EasyLogger("profiler.log");
}

void Profiler::set_logger(EasyLogger* logger){
    if(m_logger!=nullptr){
        delete m_logger ;
    }
    m_logger = logger;
}
