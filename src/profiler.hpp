#ifndef PROF_H
#define PROF_H

#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <HDT.hpp>
#include <HDTManager.hpp>
#include <easy_logger/easy_logger.h>

using namespace std;

const string owl_class = "http://www.w3.org/2002/07/owl#";
const string rdfs_class = "http://www.w3.org/2000/01/rdf-schema#Class";
const string rdf_type = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";
const string rdfs_subclassof = "http://www.w3.org/2000/01/rdf-schema#subClassOf";
const string dbo_prefix = "http://dbpedia.org/ontology/";
const string dbp_predix = "http://dbpedia.org/property/";

class Profiler{
  public:

    Profiler();
    Profiler(string hdt_dir);
    ~Profiler();
    void set_logger(EasyLogger* logger);
    void set_logger(string);
    void profile_classes();
    void profile_properties();
    void profile_class_properties(string);
    void profile_numeric_properties();
    void profile_numeric_class_properties(string);
    std::list<string>* get_unfiltered_classes();
    std::list<string>* get_classes_from_hdt();
    std::list<string>* get_unprocessed_classes();
    std::list<string>* get_instances(string class_uri);
    std::list<string>* get_class_properties(string class_uri);
    std::list<string>* get_properties_from_file(string);
    void set_dir_sep(string sep);
    string get_dir_sep();
    string merge_dirs(string base_dir, string relative_dir);
    void set_base_gen_dir(string);
    string get_base_gen_dir();
    void set_classes_fname(string);
    string get_classes_fname();
    void set_min_objects(long);
    long get_min_objects();
    bool file_exists(string);
    string get_fname_from_uri(string);
    string get_properties_all_dir();
    string get_properties_num_dir();
//    bool str_to_double(string s, double & val);
    bool is_double(string s);
    bool are_numeric(std::list<string>*);
  private:
    void write_classes(std::list<string>*);
    EasyLogger* m_logger=nullptr;
    hdt::HDT* m_hdt=nullptr;
    string m_classes_fname="classes.txt";
    string m_base_gen_dir="generated_files";
    string m_properties_all_dir="all_properties";
    string m_properties_num_dir="num_properties";
    string m_dir_sep="/";
    long m_min_objects=2;
};
#endif // PROF_H
