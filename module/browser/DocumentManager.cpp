#include "DocumentManager.h"

#include <fstream>
#include <cstring>

namespace Parser {
    std::vector<Node> parseTree(const char* document) {
        return { Node(PARAGRAPH, 0, "", {
            Node(WORD, static_cast<int>(strlen(document)), document, {})
        }) };
    }
}





DocumentManager::DocumentManager() {
    loadDocument(std::string("res/pages/Home.md"));
}



int DocumentManager::applyScroll(const int scroll) {
    const int bf = this->scroll;
    this->scroll += scroll;
    if(this->scroll > 0) this->scroll = 0;
    return abs(bf - this->scroll); // Returns delta scroll
}



bool DocumentManager::loadDocument(const char* document) {
    nodeTree = Parser::parseTree(document);
    scroll = 0;
    return true; // Status return
}



bool DocumentManager::loadDocument(const std::string& path) {
    std::string content;
    if(!loadFile(path, content)) {
        const char* document = "# Error\r\nCould not find " + *path.c_str();
        loadDocument(document);
        return false;
    }
    return loadDocument(_strdup(content.c_str()));
}





bool DocumentManager::loadFile(const std::string& path, std::string& out) {
    std::ifstream file = std::ifstream(path);
    if(!file.is_open()) return false;

    out.clear(); // Clears old data
    std::string line;
    while(std::getline(file, line)) {
        out += line + "\r\n";
    }
    file.close();
    return true;
}
