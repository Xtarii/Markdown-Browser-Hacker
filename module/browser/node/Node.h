#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

namespace Parser {
    enum NodeType {
        NO_VALUE,
        LIST,
        STAR,
        TITLE,
        PARAGRAPH,
        WORD,
        SPACE,
        NEWLINE,
    };

    struct Node {
        NodeType type = NO_VALUE;
        int intValue;
        std::string value;
        std::vector<Node> tree;
        [[nodiscard]] std::string toString() const;
    };
}

#endif //NODE_H
