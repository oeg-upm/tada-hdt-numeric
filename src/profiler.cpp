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
    delete classes;
}

void Profiler::profile_class_properties(string class_uri){
    std::list<string>* properties;
    string fname, fdir;
    properties = get_class_properties(class_uri);
    m_logger->log("profile_class_properties> "+to_string(properties->size()) + " for the class: "+class_uri);
    fname = get_fname_from_uri(class_uri);
    fdir = merge_dirs(m_base_gen_dir,m_properties_all_dir);
    fdir = merge_dirs(fdir, fname);
    ofstream out;
    out.open(fdir);
    for(auto it=properties->cbegin();it!=properties->cend();it++){
        out << *it << endl;
    }
    out.close();
    delete properties;
}

std::list<string>* Profiler::get_class_properties(string class_uri){
    hdt::IteratorTripleString *itt;
    hdt::TripleString * triple;
    std::list<string>* properties = new std::list<string>;
    std::list<string>* instances = get_instances(class_uri);
    std::unordered_map<string,long> *prop_counts = new std::unordered_map<string,long>;
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
        if(it->second >= m_min_objects){
            properties->push_back(it->first);
        }
    }
    m_logger->log("get_class_properties> included "+to_string(properties->size())+" out of "+to_string(prop_counts->size())+" properties");
    delete instances;
    delete prop_counts;
    return properties;
}


std::list<string>* Profiler::get_instances(string class_uri){
    std::list<string> *instances = new std::list<string>;
    hdt::IteratorTripleString *it;
    hdt::TripleString *triple;
    m_logger->log("get_instances> get instances of the class: <"+class_uri+"> ");
    it = m_hdt->search("", rdf_type.c_str(),class_uri.c_str());
    while(it->hasNext()){
        triple = it->next();
        instances->push_back(triple->getSubject());
    }
    delete it;
    m_logger->log("get_instances> num of instances: "+to_string(instances->size()));
    return instances;
}

string Profiler::get_fname_from_uri(string s){
    string tt, token;
    istringstream ist;
    ist.str(s);
    while(getline(ist, tt, '/')) {
        if(tt!="") {
            token = tt;
        }
    }
    ist.str(token);
    while(getline(ist, tt, '#')) {
        if(tt!="") {
            token = tt;
        }
    }
    return token+".txt";
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
    m_logger->log("get_classes_from_hdt> fetching classes from the hdt");
    while(itt->hasNext()) {
        triple = itt->next();
        class_uri = triple->getSubject();
        //        cout<<class_uri<<endl;
        if(class_uri.substr(0, dbo_prefix.size())==dbo_prefix) {
            classes->push_back(class_uri);
        }
    }
    delete itt;
    m_logger->log("get_classes_from_hdt> number of fetched classes: "+to_string(classes->size()));
    return classes;
}

std::list<string>* Profiler::get_unprocessed_classes() {
    std::list<string>* classes = new std::list<string>;
    ifstream input;
    string line, fname, prop_dir;
    m_logger->log("get_classes_from_txt> before the loop");
    input.open(merge_dirs(m_base_gen_dir, get_classes_fname()));
    while(getline(input, line)) {
        fname = get_fname_from_uri(line);
        prop_dir = merge_dirs(m_base_gen_dir, m_properties_all_dir);
        prop_dir = merge_dirs(prop_dir, fname);
        if(!file_exists(prop_dir)){
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
    classes_fdir = merge_dirs(m_base_gen_dir, m_classes_fname);
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

void Profiler::set_base_gen_dir(string p_dir) {
    m_base_gen_dir = p_dir;
}

string Profiler::get_base_gen_dir() {
    return m_base_gen_dir;
}

bool Profiler::file_exists(string fdir){
    ifstream input;
    input.open(fdir.c_str());
    return input.good();
}

void Profiler::set_min_objects(long min_objects){
    m_min_objects = min_objects;
}
long Profiler::get_min_objects(){
    return m_min_objects;
}

string Profiler::get_properties_all_dir(){
    return m_properties_all_dir;
}

string Profiler::get_properties_num_dir(){
    return m_properties_num_dir;
}

void Profiler::profile_numeric_properties(){
    std::list<string>* classes = get_unfiltered_classes();
    for(auto it=classes->cbegin();it!=classes->cend();it++){
        profile_numeric_class_properties(*it);
    }
    delete classes;
}

std::list<string>* Profiler::get_unfiltered_classes(){
    std::list<string>* classes = new std::list<string>;
    ifstream input;
    string classes_fdir, line, class_dir, base_dir;
    classes_fdir = merge_dirs(get_base_gen_dir(),get_classes_fname());
    input.open(classes_fdir);
    if(input.good()){
        base_dir = merge_dirs(get_base_gen_dir(),get_properties_num_dir());
        while(getline(input,line)){
            class_dir = merge_dirs(base_dir, get_fname_from_uri(line));
            if(!file_exists(class_dir)){
                classes->push_back(line);
            }
            else{
                m_logger->log("get_unfiltered_classes> "+line+" is already computed");
            }
        }//while
    }//if
    else{
        m_logger->log("get_unfiltered_classes> error: unable to open: "+classes_fdir);
    }
    m_logger->log("get_unfiltered_classes> num of unfiltered classes: "+to_string(classes->size()));
    return classes;
}


void Profiler::profile_numeric_class_properties(string class_uri){
    std::list<string> *properties = get_properties_from_file(merge_dirs((merge_dirs(get_base_gen_dir(),get_properties_all_dir())),get_fname_from_uri(class_uri)));
    std::list<string>* instances = get_instances(class_uri);
    hdt::IteratorTripleString *itt;
    hdt::TripleString *triple;
    std::list<string> *objects = new std::list<string>;
    string fdir;
    ofstream out;
    fdir = merge_dirs(get_base_gen_dir(),get_properties_num_dir());
    fdir = merge_dirs(fdir, get_fname_from_uri(class_uri));
    out.open(fdir);
    m_logger->log("profile_numeric_class_properties> class_uri: "+class_uri);
    m_logger->log("profile_numeric_class_properties> num of instances: "+to_string(instances->size()));
    for(auto it=properties->cbegin();it!=properties->cend();it++){
        for(auto it2=instances->cbegin();it2!=instances->cend();it2++){
            itt = m_hdt->search((*it2).c_str(), (*it).c_str(),"");
            while(itt->hasNext()){
                triple = itt->next();
                objects->push_back(triple->getObject());
            }
            delete itt;
        }
        if(are_numeric(objects)){
             m_logger->log("profile_numeric_class_properties> num of objects for the numeric property: "+(*it)+" is "+to_string(objects->size()));
            out << (*it) <<endl;
        }
        else{
            m_logger->log("profile_numeric_class_properties> num of objects for the non-numeric property: "+(*it)+" is "+to_string(objects->size()));
        }
        objects->clear();

    }
    out.close();
    delete objects;
    delete properties;
    delete instances;
}

bool Profiler::are_numeric(std::list<string>* objects){
    long nums=0, literals=0;
    for(auto it=objects->cbegin();it!=objects->cend();it++){
        if(is_double(*it)){
            nums++;
        }
        else{
            literals++;
        }
    }
    return nums > literals;
}


bool Profiler::is_double(string s){
    int i;
    string num_str="";
    bool got_dec=false;
    for(i=0;i<s.length();i++){
        if(s[i]>='0' && s[i]<='9'){
            num_str += s[i];
        }
        else if(s[i]=='.' && !got_dec){
                num_str += s[i];
                got_dec = true;
                if(num_str[0]=='.'){ // if the str starts with a .
                    return false;
                }
        }
        else if(s[i]!='\'' && s[i]!='"'){
            break;
        }
    }
    if(num_str=="" || num_str=="."){
        return false;
    }
    else{
        strtod(num_str.c_str(), nullptr); // just to make sure it fails if the string is not double
//        val = strtod(num_str.c_str(), nullptr);
        return true;
    }
}

std::list<string>* Profiler::get_properties_from_file(string fdir){
    ifstream input;
    string line;
    std::list<string>* properties = new std::list<string>;
    input.open(fdir);
    if(input.good()){
        m_logger->log("get_properties_from_file> "+fdir);
        while(getline(input, line)){
            properties->push_back(line);
        }
    }
    else{
        m_logger->log("get_properties_from_file> error opening the file: "+fdir);
    }
    m_logger->log("get_properties_from_file> num of properties: "+to_string(properties->size()));
    return properties;
}








