#include <gtest/gtest.h>
#include "profiler.hpp"
#include <HDTSpecification.hpp>
#include <HDT.hpp>
#include <HDTManager.hpp>
#include <cstdio>

string input_file = "test.ttl";
string log_file = "test.log";
string hdt_file = "test.hdt";
string p_test_dir = "generated_files_test";

//string dbr_prefix = "http://dbpedia.org/resource/";
//string dbo_prefix = "http://dbpedia.org/ontology/";


void ttl_to_hdt(string ttl_dir) {
    hdt::HDTSpecification spec("");
    string baseUri="<file://"+ttl_dir+">";
    string output_file;
    hdt::HDT* hdt = hdt::HDTManager::generateHDT(ttl_dir.c_str(), baseUri.c_str(), hdt::TURTLE, spec, NULL);
    output_file = ttl_dir.substr(0, ttl_dir.find_last_of(".")) +  ".hdt"; // same name but with .hdt instread of .ttl
    ofstream out;
    // Save HDT
    out.open(output_file.c_str(), ios::out | ios::binary | ios::trunc);
    if(!out.good()) {
        throw std::runtime_error("Could not open output file.");
    }
    hdt->saveToHDT(out, NULL);
    string index_file=hdt_file+".index.v1-1";
    if(remove(index_file.c_str())!=0) {
        cout << "Error deleting "<<index_file<<endl;
    }
    out.close();
}



namespace {

    TEST(ProfilerTest, hello) {
        ASSERT_TRUE(true);
    }

    TEST(ProfilerTest, empty) {
        Profiler* profiler;
        profiler = new Profiler();
        profiler->set_logger(log_file);
        delete profiler;
    }

    TEST(ProfilerTest, fetch_classes) {
        Profiler* profiler;
        profiler = new Profiler(hdt_file);
        profiler->set_logger(log_file);
        std::list<string>* classes = profiler->get_classes_from_hdt();
        ASSERT_EQ(classes->size(), 12);
        delete profiler;
        delete classes;
    }

    TEST(ProfilerTest, profile_classes) {
        Profiler* profiler;
        profiler = new Profiler(hdt_file);
        profiler->set_logger(log_file);
        profiler->set_base_gen_dir(p_test_dir);
        ASSERT_STREQ(profiler->get_base_gen_dir().c_str(), p_test_dir.c_str());
        profiler->profile_classes();
        ifstream input;
        string classes_f_dir;
        string classes_fname ="classes.txt" ;
        profiler->set_classes_fname(classes_fname);
        classes_f_dir = profiler->merge_dirs(profiler->get_base_gen_dir(), profiler->get_classes_fname());
        input.open(classes_f_dir.c_str());
        ASSERT_TRUE(input.good());
        string content="", line;
        while(getline(input, line)) {
            content += line+"\n";
        }
        //cout <<"content: \n"<<content<<endl;
        string golden_classes_content;
        golden_classes_content = "http://dbpedia.org/ontology/Agent\n"
                                 "http://dbpedia.org/ontology/AmateurBoxer\n"
                                 "http://dbpedia.org/ontology/Athlete\n"
                                 "http://dbpedia.org/ontology/BaseballPlayer\n"
                                 "http://dbpedia.org/ontology/Boxer\n"
                                 "http://dbpedia.org/ontology/Cyclist\n"
                                 "http://dbpedia.org/ontology/GolfPlayer\n"
                                 "http://dbpedia.org/ontology/Organisation\n"
                                 "http://dbpedia.org/ontology/Person\n"
                                 "http://dbpedia.org/ontology/Rower\n"
                                 "http://dbpedia.org/ontology/Wrestler\n"
                                 "http://dbpedia.org/ontology/Company\n";
        ASSERT_STREQ(content.c_str(), golden_classes_content.c_str());
        delete profiler;
    }

    TEST(ProfilerTest, profile_properties) {
        Profiler* profiler;
        profiler = new Profiler(hdt_file);
        profiler->set_logger(log_file);
        profiler->set_base_gen_dir(p_test_dir);
        ASSERT_STREQ(profiler->get_base_gen_dir().c_str(), p_test_dir.c_str());
        profiler->profile_classes();
        profiler->profile_properties();
        ifstream input;
        string boxer_properties_dir;
        boxer_properties_dir = profiler->merge_dirs(profiler->get_base_gen_dir(), profiler->get_properties_all_dir());
        boxer_properties_dir = profiler->merge_dirs(boxer_properties_dir, "Boxer.txt");
        cout << "Boxer properties dir: "<<boxer_properties_dir<<endl;
        input.open(boxer_properties_dir.c_str());
        ASSERT_TRUE(input.good());
        string content="", line;
        while(getline(input, line)) {
            content += line+"\n";
        }
        cout <<"content: \n"<<content<<endl;
        ASSERT_NE(content.find("http://dbpedia.org/property/height"), std::string::npos);
        ASSERT_NE(content.find("http://dbpedia.org/property/weight"), std::string::npos);
        ASSERT_NE(content.find("http://www.w3.org/2000/01/rdf-schema#label"), std::string::npos);
        ASSERT_NE(content.find("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"), std::string::npos);
        ASSERT_EQ(content.find("http://dbpedia.org/property/NEW"), std::string::npos);
        delete profiler;
    }

    TEST(ProfilerTest, profile_numeric_properties) {
        Profiler* profiler;
        profiler = new Profiler(hdt_file);
        profiler->set_logger(log_file);
        profiler->set_base_gen_dir(p_test_dir);
        ASSERT_STREQ(profiler->get_base_gen_dir().c_str(), p_test_dir.c_str());
        profiler->profile_classes();
        profiler->profile_properties();
        profiler->profile_numeric_properties();
        ifstream input;
        string boxer_properties_dir;
        boxer_properties_dir = profiler->merge_dirs(profiler->get_base_gen_dir(), profiler->get_properties_num_dir());
        boxer_properties_dir = profiler->merge_dirs(boxer_properties_dir, "Boxer.txt");
        cout << "Boxer properties dir: "<<boxer_properties_dir<<endl;
        input.open(boxer_properties_dir.c_str());
        ASSERT_TRUE(input.good());
        string content="", line;
        while(getline(input, line)) {
            content += line+"\n";
        }
        cout <<"content: \n"<<content<<endl;
        ASSERT_NE(content.find("http://dbpedia.org/property/height"), std::string::npos);
        ASSERT_NE(content.find("http://dbpedia.org/property/weight"), std::string::npos);
        ASSERT_EQ(content.find("http://www.w3.org/2000/01/rdf-schema#label"), std::string::npos);
        ASSERT_EQ(content.find("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"), std::string::npos);
        ASSERT_EQ(content.find("http://dbpedia.org/property/NEW"), std::string::npos);
        delete profiler;
    }

    TEST(ProfilerTest, merge_dirs) {
        Profiler* profiler;
        profiler = new Profiler();
        profiler->set_logger(log_file);
        ASSERT_STREQ("/home/ubuntu/docs", profiler->merge_dirs("/home/ubuntu", "docs").c_str());
        ASSERT_STREQ("/home/ubuntu/docs", profiler->merge_dirs("/home/ubuntu/", "docs").c_str());
        ASSERT_STREQ("/home/ubuntu/docs", profiler->merge_dirs("/home/ubuntu", "/docs").c_str());
        ASSERT_STREQ("/home/ubuntu/docs", profiler->merge_dirs("/home/ubuntu/", "/docs").c_str());
    }

    TEST(ProfilerTest, str_to_double) {
        Profiler* profiler;
        profiler = new Profiler();
        profiler->set_logger(log_file);
        string s = "\"204.0\"^^<http://dbpedia.org/datatype/centimetre>";
        ASSERT_TRUE(profiler->is_double(s));
//        ASSERT_EQ(v, 204.0);
        ASSERT_FALSE(profiler->is_double("'a2.0"));
        ASSERT_FALSE(profiler->is_double("_a2.0"));
        ASSERT_TRUE(profiler->is_double("204a.4"));
//        ASSERT_EQ(204, v);
        ASSERT_FALSE(profiler->is_double(".123aa"));
        delete profiler;
    }


}// namespace


int main(int argc, char** argv) {
    ttl_to_hdt(input_file);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
