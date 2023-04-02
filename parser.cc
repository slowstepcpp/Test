#include <iostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include "/home/slowstep/LinuxLearning/project/Boost_search/util.h"
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;
struct DockInfo {
    string title;   // 网页标题
    string content; // 摘要
    string url;     // 在boost官网中的URL
};
const string src_path = "/home/slowstep/LinuxLearning/project/Boost_search/data/input/";           // 数据源所在的文件夹
const string dst_file = "/home/slowstep/LinuxLearning/project/Boost_search/data/raw_html/raw.txt"; // 要写入的目标文件
vector<string> file_list;                                                                          // 保存所有HTML文件的name
vector<DockInfo> results;                                                                          // 保存每一个文件去标签之后的结果
bool EnumFiles() {                                                                                 // 将src_path下的所有.html文件的文件名放入file_list中
    namespace fs = boost::filesystem;
    fs::path root_path(src_path); // 定义起始目录,从起始目录开始遍历
    if (!fs::exists(root_path)) {
        cerr << "路径不存在" << endl;
        return false;
    }
    fs::recursive_directory_iterator end;              // 定义结束迭代器
    fs::recursive_directory_iterator start(root_path); // 定义起始迭代器
    while (start != end) {                             // 递归遍历文件夹
        if (!fs::is_regular(*start)) {                 // 如果是一个目录
            start++;
            continue;
        }
        // 是普通文件
        if (start->path().extension() == ".html") { // 如果文件后缀是html,就添加到file_list中
            // cout << start->path().string() << endl; // 测试
            file_list.push_back(start->path().string());
        }
        start++;
    }
    return true;
}
static bool ParseTitle(const string &result, string &title) {
    size_t x = result.find("<title>");
    if (x == string::npos) {
        cerr << "找不到标题的开始" << endl;
        return false;
    }
    size_t start = x + 7; // 标题的开始下标
    size_t y = result.find("</title>");
    if (y == string::npos) {
        cerr << "找不到标题的结束" << endl;
        return false;
    }
    size_t end = y - 1; // 标题的结束下标
    title = result.substr(start, end - start + 1);
    return true;
}
static bool ParseContent(const string &result, string &content) {
    enum status {
        LABLE,  // 标签
        CONTENT // 摘要
    };
    // 使用一个简单的状态机完成去标签
    enum status s = LABLE;
    for (char c : result) {
        switch (s) {
        case LABLE:
            if (c == '>') { // 表示当前标签处理完毕
                s = CONTENT;
            }
            break;
        case CONTENT:
            if (c == '<') { // 表示又遇到了一个标签
                s = LABLE;
            } else {
                if (c == '\n') { // 文本中的'\n'将其去除,因为我们使用'\n'作为解析之后的文本分隔符
                    c = ' ';
                }
                content += c; // 添加到摘要中
            }
            break;
        default:
            break;
        }
    }
    return true;
}
static bool ParseUrl(const string &filepath, string &url) {
    string url_head = "https://www.boost.org/doc/libs/1_81_0/doc/html/";
    // 使用boost库提供的FileSystem进行文件遍历得到的filepath的格式是"./data/input/..."
    url += url_head;
    int size = src_path.size();
    url += filepath.substr(size); // 从./data/input/后面一直截取到结尾
    return true;
}
bool ParseHtml() {                       // 对.html文件进行解析,将结果保存到results中
    for (const auto &file : file_list) { // file表示文件的路径
        // 1.将文件中的内容读出来
        string result;
        if (!FileUtil::ReadFile(file, result)) {
            cerr << "读取文件内容失败" << endl;
            continue;
        }
        // 读取文件内容成功
        DockInfo temp;
        // 2.提取标题、提取摘要、构建URL
        if (!ParseTitle(result, temp.title)) {
            cerr << "提取标题失败" << endl;
            continue;
        }
        if (!ParseContent(result, temp.content)) {
            cerr << "提取摘要失败" << endl;
            continue;
        }
        if (!ParseUrl(file, temp.url)) {
            cerr << "构建Url失败" << endl;
            continue;
        }
        // 调试
        // cout << "标题: " << temp.title << endl;
        // cout << "摘要: " << temp.content << endl;
        // cout << "URL: " << temp.url << endl;
        // break;

        // 3.将结果push到result
        results.push_back(std::move(temp));
    }
    return true;
}
bool SaveHtml() { // 将results的结果回写到dst_file
    std::ofstream out(dst_file, std::ios::out | std::ios::binary);
    if (!out.is_open()) {
        cerr << "打开文件失败" << endl;
        return false;
    }
    for (const auto &doc : results) {
        out << doc.title << '\3' << doc.content << '\3' << doc.url << '\n';
    }
    out.close();
    return true;
}
int main() {
    // 1.递归遍历文件
    if (!EnumFiles()) {
        cerr << "递归遍历文件失败" << endl;
        exit(0);
    }
    cout << "递归遍历文件成功" << endl;
    // 2.解析
    if (!ParseHtml()) {
        cerr << "解析失败" << endl;
        exit(0);
    }
    cout << "解析成功" << endl;
    // 3.回写
    if (!SaveHtml()) { // 将results的结果回写到dst_file
        cerr << "回写到dst_file失败" << endl;
        exit(0);
    }
    cout << "回写到dst_file成功" << endl;
    return 0;
}