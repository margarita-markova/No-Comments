#include <iostream>
#include <fstream>
#include <queue>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
 
bool HandleComment(std::string &buffer) {
    //we need to check which part of sentence is comment
    std::queue<size_t> pos_commas;
    std::queue<size_t> pos_semicolon;
    std::queue<size_t> pos_comment;
    std::queue<size_t> pos_mcomment;
    std::string buff = buffer;
 
    //std::cout << type << " type" << std::endl;
 
    //find all important signs
    while (buff.find("\"") != std::string::npos) {
        pos_commas.push(buff.find("\""));
        buff.replace(buff.find("\""), 1, "&");
    }
    while (buff.find(";") != std::string::npos) {
        pos_semicolon.push(buff.find(";"));
        buff.replace(buff.find(";"), 1, "&");
    }
    while (buff.find("//") != std::string::npos) {
        pos_comment.push(buff.find("//"));
        buff.replace(buff.find("//"), 2, "&");
    }
    while (buff.find("/*") != std::string::npos) {
        pos_mcomment.push(buff.find("/*"));
        buff.replace(buff.find("/*"), 2, "&");
    }
 
    //which of semicolons is not part of sentence
    if (pos_commas.size() >= 2 && (pos_commas.front() < pos_comment.front() || pos_commas.front() < pos_mcomment.front())) {
        pos_commas.pop();
        for (int i = 0; i < (pos_commas.size()+1)/2; i++) {
            while (pos_semicolon.front() < pos_commas.front()) {
                if (i != 0) pos_commas.pop();
                while (pos_semicolon.front() < pos_commas.front()) {
                    pos_semicolon.pop();
                }
            }
        }
        std::cout << pos_semicolon.front() << " " << pos_mcomment.front() << std::endl;
        while (pos_comment.front() < pos_semicolon.front() && pos_comment.size() > 0) {
            pos_comment.pop();
        }
         while  (pos_mcomment.front() < pos_semicolon.front() && pos_mcomment.size() > 0) {
            pos_mcomment.pop();
        }
 
        std::cout << pos_semicolon.front() << " " << pos_mcomment.front() << std::endl;
            if (!pos_comment.empty())
                buffer = buffer.substr(0, pos_comment.front() + 1);
 
            if (!pos_mcomment.empty()) {
                buffer = buffer.substr(0, pos_mcomment.front() + 1);
                return boost::find_last(buffer, "*/") ? false : true;
            }
    }
    else
        buffer = buffer.substr(0, pos_comment.front()+1);
    return false;
}
 
 
int main(int argc, char* argv[]) {
    if (argc < 1) {
        std::cout << "You need an argument: please, write path of source file" << std::endl;
    }
    if (!boost::iends_with(argv[1], ".txt"))
        std::cout << "Your file must have extension .txt" << std::endl;
    std::ifstream source_file(argv[1]);
    if (!(source_file.is_open())) {
        std::cout << "File not exists" << std::endl;
    }
 
    std::string buffer;
    bool multiline_flag = false;
 
    while (getline(source_file, buffer)) {
        //in the case of space before comment we need to trim incoming string
        boost::trim(buffer);
        if (multiline_flag) {
            if (boost::find_last(buffer, "*/")) multiline_flag = false;
            buffer.erase(buffer.begin(), buffer.end());
            //check buffer is empty after */
        }
        //in case of simple comment from the beginning of string
        else if(boost::starts_with(buffer, "//"))
            buffer.erase(buffer.begin(), buffer.end());
        else if (boost::starts_with(buffer, "/*")) {
            if (!boost::find_last(buffer, "*/"))
                multiline_flag = true;
            buffer.erase(buffer.begin(), buffer.end());
        }
        //in case of comment after command, for ex: "print(";//;"); //comment"
        else {
            if (HandleComment(buffer)) multiline_flag = true;
        }
        if (!buffer.empty()) {
            std::cout << buffer << std::endl;
            //write to file
        }
    }
    return 0;
}
