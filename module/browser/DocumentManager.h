#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <string>
#include <vector>

#include "node/Node.h"

struct DocumentManager {
    int scroll = 0;
    std::vector<Parser::Node> nodeTree = std::vector<Parser::Node>();

    DocumentManager();

    int applyScroll(int scroll);
    bool loadDocument(const char* document);
    bool loadDocument(const std::string& path);

    [[nodiscard]] std::vector<Parser::Node> getDocument() const { return nodeTree; }
    [[nodiscard]] int getScroll() const { return scroll; }

    static bool loadFile(const std::string& path, std::string& out);
};

#endif //DOCUMENTMANAGER_H
