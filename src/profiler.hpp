#ifndef PROF_H
#define PROF_H

#include <string>
#include <iostream>
#include <HDT.hpp>
#include <HDTManager.hpp>
#include <easy_logger/easy_logger.h>

using namespace std;

const string rdf_type = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";
const string rdfs_subclassof = "http://www.w3.org/2000/01/rdf-schema#subClassOf";

class Profiler{
  public:

    Profiler();
    Profiler(string hdt_dir);
    void set_logger(EasyLogger* logger);
  private:
    EasyLogger* m_logger=nullptr;
    hdt::HDT* m_hdt=nullptr;

};
#endif // PROF_H
