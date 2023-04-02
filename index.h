#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include "/home/slowstep/LinuxLearning/project/Boost_search/util.h"
using std::getline;
using std::ifstream;
using std::unordered_map;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::size;
using std::move;
namespace ns_index {
    struct DocInfo {    // 正排索引对应结构体
        string title;   // 文档标题
        string content; // 文档内容
        string url;     // 在Boost官网中的URL
        size_t doc_id;  // 文档id
    };
    struct InvertedElme { // 倒排索引对应结构体
        int doc_id;       // 文档id
        string word;      // 关键字
        size_t weight;    // 文档id的权值
    };
    // 倒排拉链
    using InvertedList = vector<InvertedElme>;
    class Index {
    public:
        DocInfo *GetForwardIndex(const size_t doc_id) { // 根据文档id获取文档相关信息
            if (doc_id < size(forward_index)) {
                return forward_index.data() + doc_id;
            }
            return nullptr;
        }
        InvertedList *GetInvertedList(const string &word) { // 根据关键字获得倒排拉链
            if (inverted_index.count(word) == 0) {
                return nullptr;
            }
            return &inverted_index[word];
        }
        // 根据raw.txt文件的内容构建索引,raw.txt中存放的是所有HTML文件解析完毕之后的数据
        bool BulidIndex(const string &raw_filepath) {
            // 1.按行读取文件中的内容
            ifstream ifs(raw_filepath, std::ios::in | std::ios::binary);
            if (!ifs.is_open()) {
                cerr << "打开文件失败" << endl;
                return false;
            }
            string line; // 按行读取
            int cnt = 0;
            while (getline(ifs, line)) {
                // 2.建立正排索引
                DocInfo *doc = BulidForwardIndex(line);
                if (doc == nullptr) {
                    cerr << "建立正排索引失败" << endl;
                    continue;
                }
                // 3.建立倒排索引
                if (BulidInvertedIndex(doc) == false) {
                    cerr << "建立倒排索引失败" << endl;
                }
                cnt++;
                if (cnt % 100 == 0) {
                    cout << "已经建立完成" << cnt << "个索引" << endl;
                }
            }
            cout << "构建索引完毕" << endl;
            return true;
        }
        static Index *GetInst() {
            static Index sig;
            return &sig;
        }

    private:
        DocInfo *BulidForwardIndex(const string line) {
            DocInfo info;
            // 1.将line按照\3为分割进行切分
            vector<string> vs; // 切分完成的数据放到vs中
            StringUtil::CutString(line, vs, "\3");
            if (size(vs) != 3) {
                return nullptr;
            }
            // 2.解析完毕的内容填入到DocInfo中
            info.title = move(vs[0]);
            info.content = move(vs[1]);
            info.url = move(vs[2]);
            info.doc_id = size(forward_index);
            // 3.插入到vector中
            forward_index.push_back(move(info));
            return &forward_index.back();
        }
        bool BulidInvertedIndex(DocInfo *doc) { // 建立倒排索引
            // 1.提取关键词
            vector<string> title_words;
            JiebaUtil::CutString(doc->title, title_words);
            vector<string> content_words;
            JiebaUtil::CutString(doc->content, content_words);
            // 2.进行词频统计
            struct word_cnt {
                size_t title_cnt = 0;   // 某一个关键词在标题中出现的次数
                size_t content_cnt = 0; // 某一个关键词在内容中出现的次数
            };
            unordered_map<string, word_cnt> word_hash;
            for (string &word : title_words) { // 统计标题中的关键词出现的次数
                boost::to_lower(word);         // 转化为小写，忽略大小写
                word_hash[word].title_cnt++;
            }
            for (string &word : content_words) { // 统计内容中的关键词出现的次数
                boost::to_lower(word);           // 转化为小写，忽略大小写
                word_hash[word].content_cnt++;
            }
            // 3.自定义相关性
            for (auto &w_pair : word_hash) {
                InvertedElme temp;
                temp.doc_id = doc->doc_id;
                temp.word = w_pair.first;
#define X 10
#define Y 1
                // 认为如果关键词出现在了标题中,那么权值计算大一点
                temp.weight = w_pair.second.title_cnt * X + w_pair.second.content_cnt;
                // 将其添加到倒排拉链中(倒排索引:根据词找到doc_id)
                inverted_index[temp.word].push_back(move(temp));
            }
            return true;
        }

    private:
        Index() = default;
        Index(const Index &) = delete;
        Index &operator=(const Index &) = delete;

    private:
        vector<DocInfo> forward_index;                      // 正排索引,forward_index[i]表示文档id为i的DocInfo信息
        unordered_map<string, InvertedList> inverted_index; // 倒排索引,一个关键字对应多个InvertedElme结构(多个文档id)
    };
} // namespace ns_index