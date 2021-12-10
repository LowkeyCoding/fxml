#include "sxml.h"

int main(int argc, char** argv) {
    XMLDocument* doc = new_XMLDocument();
    XMLNode* root;
    if (load_file(doc, "../tests/example.xml")) {
        root = parse_xml(doc);
        if (root != NULL) {
            print_XMLNode(root, 0);
        }
        free_XMLStacks();
    }
    free_XMLDocument(doc);
    return 0;
}
