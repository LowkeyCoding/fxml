#include "sxml.h"

int main(int argc, char** argv) {
    XMLDocument* doc = new_XMLDocument();
    XMLNode* root;
    if (load_File(doc, "test.xml")) {
        root = parse_XML(doc);
        if (root != NULL) {
            print_XMLNode(root, 0);
            printf("Node Count: %d\n", SFXML_NODES->index-1);
            printf("Attribute Count: %d\n", SFXML_ATTRIBUTES->index-1);
        }
    }
    free_XMLNodes();
    printf("Nodes gone\n");
    free_XMLAttributes();
    printf("Attributes gone\n");
    free_XMLDocument(doc);
    printf("Document gone\n");
    return 0;
}