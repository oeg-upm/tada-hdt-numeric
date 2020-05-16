#include "profiler.hpp"

Profiler::Profiler() {
    m_logger = new EasyLogger("profiler.log");
}

Profiler::Profiler(string hdt_file_dir) {
    m_hdt = hdt::HDTManager::mapIndexedHDT(hdt_file_dir.c_str());
    m_logger = new EasyLogger("profiler.log");
}

Profiler::~Profiler() {
    if(m_hdt!=nullptr) {
        delete m_hdt;
    }
}

void Profiler::set_logger(EasyLogger* logger) {
    if(m_logger!=nullptr) {
        delete m_logger ;
    }
    m_logger = logger;
}

void Profiler::set_logger(string logger_dir) {
    if(m_logger!=nullptr) {
        delete m_logger ;
    }
    m_logger = new EasyLogger(logger_dir);
}

void Profiler::profile_classes() {
    std::list<string>* classes;
    classes = get_classes_from_hdt();
    write_classes(classes);
    delete classes;
}

void Profiler::profile_properties() {
    std::list<string>* classes = get_unprocessed_classes();
    for(auto it=classes->cbegin();it!=classes->cend();it++){
        profile_class_properties(*it);
    }
}

void Profiler::profile_class_properties(string class_uri){
    std::list<string>* properties;
    properties = get_class_properties(class_uri);
    // To continue
}

std::list<string>* Profiler::get_class_properties(string class_uri){
    hdt::IteratorTripleString *itt;
    hdt::TripleString * triple;
    std::list<string>* properties = new std::list<string>;
    std::list<string>* instances = get_instances(class_uri);
    std::unordered_map<string,unsigned long> *prop_counts = new std::unordered_map<string,unsigned long>;
    for(auto it_inst=instances->cbegin();it_inst!=instances->cend();it_inst++){
        itt = m_hdt->search(it_inst->c_str(),"", "");
        while(itt->hasNext()){
            triple = itt->next();
            if(prop_counts->find(triple->getPredicate()) != prop_counts->cend()){ // found
                (*prop_counts)[triple->getPredicate()]++;
            }
            else{
                (*prop_counts)[triple->getPredicate()] = 1;
            }
        }
        delete itt;
    }
    m_logger->log("get_class_properties> properties counts: ");
    for(auto it=prop_counts->cbegin();it!=prop_counts->cend();it++){
        m_logger->log(it->first+" - "+to_string(it->second));
        if(it->second >= min_objects){
            properties->push_back(it->first);
        }
    }
    m_logger->log("get_class_properties> included "+to_string(properties->size())+" out of "+to_string(prop_counts->size())+" properties");
    return properties;
}


std::list<string>* Profiler::get_instances(string class_uri){
    std::list<string> *instances = new std::list<string>;
    hdt::IteratorTripleString *it = m_hdt->search("", rdf_type.c_str(),class_uri.c_str());
    hdt::TripleString *triple;
    while(it->hasNext()){
        triple = it->next();
        instances->push_back(triple->getSubject());
    }
    delete it;
    m_logger->log("get_instances> num of instances: "+to_string(instances->size()));
    return instances;
}



void Profiler::set_dir_sep(string sep) {
    m_dir_sep = sep;
}

string Profiler::get_dir_sep() {
    return m_dir_sep;
}

void Profiler::set_classes_fname(string fname) {
    m_classes_fname = fname;
}

string Profiler::get_classes_fname() {
    return m_classes_fname;
}

std::list<string>* Profiler::get_classes_from_hdt() {
    std::list<string>* classes = new std::list<string>;
    string class_uri;
    hdt::IteratorTripleString* itt;
    hdt::TripleString* triple;
    itt = m_hdt->search("", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", rdfs_class.c_str());
    m_logger->log("get_classes> fetching classes from the hdt");
    while(itt->hasNext()) {
        triple = itt->next();
        class_uri = triple->getSubject();
        //        cout<<class_uri<<endl;
        if(class_uri.substr(0, dbo_prefix.size())==dbo_prefix) {
            classes->push_back(class_uri);
        }
    }
    delete itt;
    m_logger->log("get_classes> number of fetched classes: "+to_string(classes->size()));
    return classes;
}

std::list<string>* Profiler::get_unprocessed_classes() {
    std::list<string>* classes = new std::list<string>;
    ifstream input;
    string line, token="", tt;
    istringstream ist;
    m_logger->log("get_classes_from_txt> before the loop");
    input.open(merge_dirs(get_properties_dir(), get_classes_fname()));
    while(getline(input, line)) {
        ist.str(line);
        while(getline(ist, tt, '/')) {
            if(tt!="") {
                token = tt;
            }
        }
        if(!file_exists(merge_dirs(get_properties_dir(), token+".txt"))){
            classes->push_back(line);
        }
        else{
            m_logger->log("get_classes_from_txt> "+line+" is already computed");
        }
    }
    m_logger->log("get_classes_from_txt> number of fetched classes: "+to_string(classes->size()));
    return classes;
}

void Profiler::write_classes(std::list<string>* classes) {
    string classes_fdir;
    classes_fdir = merge_dirs(m_properties_dir, m_classes_fname);
    m_logger->log("write_classes> to write classes to: "+classes_fdir);
    ofstream out_file;
    out_file.open(classes_fdir);
    for(auto it=classes->cbegin(); it!=classes->cend(); it++) {
        out_file << *it <<endl;
    }
    out_file.close();
}

string Profiler::merge_dirs(string base_dir, string relative_dir) {
    string full_dir;
    full_dir = base_dir;
    if(full_dir.back()!=this->m_dir_sep.back()) {
        full_dir += m_dir_sep;
    }
    if(relative_dir.front()==m_dir_sep.back()) {
        full_dir += relative_dir.substr(1);
    }
    else {
        full_dir += relative_dir;
    }
    return full_dir;
}

void Profiler::set_properties_dir(string p_dir) {
    m_properties_dir = p_dir;
}

string Profiler::get_properties_dir() {
    return m_properties_dir;
}

bool Profiler::file_exists(string fdir){
    ifstream input;
    input.open(fdir.c_str());
    return input.good();
}

