#ifndef PROF_H
#define PROF_H

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <HDT.hpp>
#include <HDTManager.hpp>
#include <easy_logger/easy_logger.h>

using namespace std;

const string owl_class = "http://www.w3.org/2002/07/owl#";
const string rdfs_class = "http://www.w3.org/2000/01/rdf-schema#Class";
const string rdf_type = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";
const string rdfs_subclassof = "http://www.w3.org/2000/01/rdf-schema#subClassOf";
const string dbo_prefix = "http://dbpedia.org/ontology/";


class Profiler{
  public:

    Profiler();
    Profiler(string hdt_dir);
    ~Profiler();
    void set_logger(EasyLogger* logger);
    void set_logger(string);
    void profile_classes();
    std::list<string>* get_classes_from_hdt();
    void set_dir_sep(string sep);
    string get_dir_sep();
    string merge_dirs(string base_dir, string relative_dir);
    void set_properties_dir(string);
    string get_properties_dir();
    void set_classes_fname(string);
    string get_classes_fname();
  private:
    void write_classes(std::list<string>*);
    EasyLogger* m_logger=nullptr;
    hdt::HDT* m_hdt=nullptr;
    string m_classes_fname="classes.txt";
    string m_properties_dir="generated_files";
    string m_dir_sep="/";
};
#endif // PROF_H
