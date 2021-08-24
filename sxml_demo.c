#include "sxml.h"

int main(int argc, char** argv) {
    XMLDocument* doc = new_XMLDocument();
    XMLNode* root;
    if (load_File(doc, "test.xml")) {
        root = parse_XML(doc);
        if (root != NULL) {
            print_XMLNode(root, 0);
        }
        free_XMLStacks();
    }
    free_XMLDocument(doc);
    return 0;
}
