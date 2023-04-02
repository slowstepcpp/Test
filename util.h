#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "/home/slowstep/LinuxLearning/project/Boost_search/ThirdPart/cppjieba/src/MPSegment.hpp"
#include "/home/slowstep/LinuxLearning/project/Boost_search/ThirdPart/cppjieba/src/HMMSegment.hpp"
#include "/home/slowstep/LinuxLearning/project/Boost_search/ThirdPart/cppjieba/src/MixSegment.hpp"
#include "/home/slowstep/LinuxLearning/project/Boost_search/ThirdPart/cppjieba/src/KeywordExtractor.hpp"
using namespace CppJieba;
using std::vector;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
// 工具类
class FileUtil { // FileUtil中提供文件的相关操作
public:
    static bool ReadFile(const string &filepath, string &result) {
        // 将filepath中的内容读取到result中
        ifstream in(filepath);
        if (!in.is_open()) {
            cerr << "打开文件 " << filepath << " 失败" << endl;
            return false;
        }
        string tmp;
        while (std::getline(in, tmp)) { // getline的返回值是一个istream的对象,其内部重载了强制类型转换
            result += tmp;
        }
        in.close();
        return true;
    }
};
class StringUtil {
public:
    static void CutString(const string &str, vector<string> &vs, string sep /*分隔符*/) {
        boost::split(vs /*切分完毕放到vs中*/, str /*要切分的串*/, boost::is_any_of(sep) /*分隔符*/, boost::token_compress_on);
    }
};
// 词库
static const char *const TEST_FILE = "/home/slowstep/LinuxLearning/project/Boost_search/ThirdPart/cppjieba/test/testdata/testlines.utf8";
static const char *const JIEBA_DICT_FILE = "/home/slowstep/LinuxLearning/project/Boost_search/ThirdPart/cppjieba/dict/jieba.dict.utf8";
static const char *const HMM_DICT_FILE = "/home/slowstep/LinuxLearning/project/Boost_search/ThirdPart/cppjieba/dict/hmm_model.utf8";
static const char *const USER_DICT_FILE = "/home/slowstep/LinuxLearning/project/Boost_search/ThirdPart/cppjieba/dict/user.dict.utf8";
class JiebaUtil {
public:
    static void CutString(const string &words, vector<string> &vs) {
        seg.cut(words, vs);
    }

private:
    inline static MixSegment seg = MixSegment(JIEBA_DICT_FILE, HMM_DICT_FILE);
};