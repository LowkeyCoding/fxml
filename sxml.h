#ifndef SXML_H
#define SXML_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* GLOBALS */
#define EXPAND_LEXER_SIZE 1024
#define NODE_SIZE 2
#define ATTRIBUTE_SIZE 2


/* XML LIST */
typedef struct _XMLList {
    int heap_size;
    int count;
    void** items;
} XMLList;


/* XML ATTRIBUTE */
typedef struct _XMLAttribute {
    char* key;
    char* value;
} XMLAttribute;


/* XML VALUE */
enum XMLType {
    XMLTypeText,
    XMLTypeNode
};

typedef struct _XMLValue {
    enum XMLType type;
    void* value;
} XMLValue;


/* XML NODE */
typedef struct _XMLNode {
    char* tag;
    struct _XMLNode* parent;
    XMLList* inner_xml;
    XMLList* attributes;
    XMLList* children;
} XMLNode;


/* XML DOCUMENT */
typedef struct _XMLDocument {
    char* buffer;
    char* lexer;
    XMLList* info;
    size_t lexer_size;
    size_t lexer_index;
    size_t index;
    size_t file_size;
} XMLDocument;


/* NODE & ATRIBUTE STACK */
XMLList* SXML_NODES;
XMLList* SXML_ATTRIBUTES;
XMLList* SXML_TEXT;


/* LIST IMPLEMENTATION */
XMLList* new_XMLList() {
    XMLList* list = malloc(sizeof(XMLList));
    if (!list) {
        printf("cannot allocate list\n");
        exit(1);
    }
    list->count = 0;
    list->heap_size = NODE_SIZE;
    list->items = malloc(sizeof(void*) * list->heap_size);
    return list;
}

void append_XMLItem(XMLList* list, void* item) {
    if (list->count >= list->heap_size) {
        list->heap_size *= 2;
        list->items = realloc(list->items, sizeof(void*) * list->heap_size);
        if (list->items == NULL) {
            printf("Unable to reallocate list\n");
        }
    }
    list->items[list->count++] = item;
}

void free_XMLList(XMLList* list) {
    if (list) {
        free(list->items);
        free(list);
    }
}


/* VALUE IMPLEMENTATION */
XMLValue* new_XMLValue(void* item, enum XMLType type) {
    XMLValue* value = malloc(sizeof(XMLValue));
    if (!value) {
        printf("Unable to allocate value\n");
        exit(1);
    }
    value->type = type;
    value->value = item;
    return value;
}


/* NODE IMPLEMENTATION */
XMLNode* new_XMLNode(XMLNode* parent) {
    XMLNode* node = malloc(sizeof(XMLNode));
    if (!node) {
        printf("Unable to allocate node\n");
        exit(1);
    }
    node->parent = parent;

    node->inner_xml = new_XMLList();
    node->children = new_XMLList();
    node->attributes = new_XMLList();

    node->tag = NULL;
    if (parent != NULL) {
        append_XMLItem(parent->inner_xml, new_XMLValue(node, XMLTypeNode));
        append_XMLItem(parent->children, node);
    }
    append_XMLItem(SXML_NODES, node);
    return node;
}

void print_XMLNode(XMLNode* node, int indent) {
    printf("%*s%s", 4 * indent, " ", node->tag);
    for (int i = 0; i < node->attributes->count; i++) {
        XMLAttribute* attribute = node->attributes->items[i];
        printf(" %s=\"%s\"", attribute->key, attribute->value);
    }
    printf("\n");
    for (int i = 0; i < node->children->count; i++) {
        print_XMLNode(node->children->items[i], indent + 1);
    }
}

void free_XMLNode(XMLNode* node) {
    /* Free tag & text */
    if (node) {
        if (node->tag) {
            free(node->tag);
        }
        for (int i = 0; i < node->inner_xml->count; i++) {
            free(node->inner_xml->items[i]);
        }
        free_XMLList(node->inner_xml);
        free_XMLList(node->children);
        free_XMLList(node->attributes);

        free(node);
        node = NULL;
    }
}


/* ATTRIBUTE IMPLEMENTATION */
XMLAttribute* new_XMLAttribute(void) {
    XMLAttribute* attribute = malloc(sizeof(XMLAttribute));
    if (!attribute) {
        printf("Unable to allocate attribute\n");
        exit(1);
    }
    attribute->key = NULL;
    attribute->value = NULL;

    append_XMLItem(SXML_ATTRIBUTES, attribute);
    return attribute;
}

XMLAttribute* get_XMLAttribute(XMLNode* node, char* key) {
    for (int i = 0; i < node->attributes->count; i++) {
        XMLAttribute* attribute = node->attributes->items[i];
        if (!strcmp(attribute->key, key)) {
            return attribute;
        }
    }
    return NULL;
}

void free_XMLAttribute(XMLAttribute* attribute) {
    if (attribute) {
        if (attribute->key)
            free(attribute->key);
        if (attribute->value)
            free(attribute->value);
        free(attribute);
    }
}


/* DOCUMENT IMPLEMENTATION */
XMLDocument* new_XMLDocument() {
    XMLDocument* doc = malloc(sizeof(XMLDocument));
    if (doc) {
        doc->lexer_size = EXPAND_LEXER_SIZE;
        doc->lexer_index = 0;
        doc->index = 0;
        doc->buffer = NULL;
        doc->lexer = malloc(sizeof(char) * doc->lexer_size);
        doc->info = NULL;
    }
    return doc;
}

bool load_File(XMLDocument* doc, const char* filename) {
    /* Check if file opened succesfully */
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not load file from '%s'\n", filename);
        return false;
    }

    /* Get the size of the file */
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (size > 0) {

        /* Initialise buffer and ensure it is null terminated */
        doc->file_size = size+1;
        doc->buffer = (char*)calloc(sizeof(char), doc->file_size);

        /* Read file into the buffer */
        fread(doc->buffer, 1, size, file);
        fclose(file);
        return true;
    }
    return false;
}

void free_File(XMLDocument* doc) {
    if (doc) {
        free(doc->buffer);
        free(doc->lexer);
        doc->buffer = NULL;
        doc->lexer = NULL;
        doc->index = 0;
        doc->lexer_index = 0;
        doc->file_size = 0;
    }
}

void free_XMLDocument(XMLDocument* doc) {
    if(doc){
        if (doc->lexer)
            free(doc->lexer);
        if (doc->buffer)
            free(doc->buffer);
        free(doc);
    }
}


/* FREE STACKS */
void free_XMLStacks(void) {
    /* Free XMLNodes */
    if (SXML_NODES) {
        for (int i = 0; i < SXML_NODES->count; i++) {
            free_XMLNode(SXML_NODES->items[i]);
        }
        free(SXML_NODES->items);
        free(SXML_NODES);
        SXML_NODES = NULL;
    }

    /* Free XMLAttributes */
    if (SXML_ATTRIBUTES) {
        for (int i = 0; i < SXML_ATTRIBUTES->count; i++) {
            free_XMLAttribute(SXML_ATTRIBUTES->items[i]);
        }
        free(SXML_ATTRIBUTES->items);
        free(SXML_ATTRIBUTES);
        SXML_ATTRIBUTES = NULL;
    }

    /* Free XML inner text */
    if (SXML_TEXT) {
        for (int i = 0; i < SXML_TEXT->count; i++) {
            free(SXML_TEXT->items[i]);
        }
        free(SXML_TEXT->items);
        free(SXML_TEXT);
        SXML_TEXT = NULL;
    }
}


/* HELPER FUNCTION */

/* Returns true if the end of a string is equal to a given suffix. */
bool ends_with(const char* string, const char* suffix) {
    int string_length = strlen(string);
    int suffix_length = strlen(suffix);
    int end = (string_length - suffix_length);

    /* Check if the string length has the right size. */
    if (string_length >= suffix_length)
        /* Compare the string from the end - the suffix length. */
        if (!memcmp(string + end, suffix, suffix_length))
            return true;
    return false;
}

/* Returns true if a given node is inline. It also adds attributes to the node. */
bool parse_XMLAttributes(XMLDocument* doc, XMLNode* node) {
    XMLAttribute* attribute = 0;
    while (doc->buffer[doc->index] != '>') {
        doc->lexer[doc->lexer_index++] = doc->buffer[doc->index++];

        /* Tag name */
        if (doc->buffer[doc->index] == ' ' && !node->tag) {
            doc->lexer[doc->lexer_index] = '\0';
            node->tag = _strdup(doc->lexer);
            doc->lexer_index = 0;
            doc->index++;
            continue;
        }

        /* Ignore spaces */
        if (doc->lexer[doc->lexer_index - 1] == ' ') {
            doc->lexer_index--;
        }

        /* Attribute Key */
        if (doc->buffer[doc->index] == '=') {

            attribute = new_XMLAttribute();

            /* NULL terminate string then copy it to the attribute */
            doc->lexer[doc->lexer_index] = '\0';
            attribute->key = _strdup(doc->lexer);

            /* Reset lexer */
            doc->lexer_index = 0;
            continue;
        }

        /* Attribute Value */
        if (doc->buffer[doc->index] == '"' || doc->buffer[doc->index] == '\'') {
            if (!attribute->key) {
                fprintf(stderr, "Value has no key\n");
                return false;
            }

            doc->lexer_index = 0;
            doc->index++;

            /* Copy attribute value by looking for end of string either a '"'  or '\'' */
            while (doc->buffer[doc->index] != '"' && doc->buffer[doc->index] != '\'') {
                /* Check if the next char is escaped */
                if (doc->buffer[doc->index] == '\\') {

                    /* Copy the escaped character instead of the '\' */
                    doc->lexer[doc->lexer_index++] = doc->buffer[doc->index + 1];

                    /* Skip over the '\' and the escaped character */
                    doc->index += 2;
                }
                else {
                    doc->lexer[doc->lexer_index++] = doc->buffer[doc->index++];
                }
            }

            /* NULL terminate and add value to attribute */
            doc->lexer[doc->lexer_index++] = '\0';
            attribute->value = _strdup(doc->lexer);

            /* Append attribute to node and reset */
            append_XMLItem(node->attributes, attribute);
            doc->lexer_index = 0;
            doc->index++;
            continue;
        }

        /* Incase attribute does not have a value */
        if ((doc->buffer[doc->index] == ' ' || doc->buffer[doc->index + 1] == '>')
            && node->tag && doc->lexer_index > 0) {

            attribute = new_XMLAttribute();

            /* Copy a the last character incase where the attribute is at the end of the node */
            if (doc->buffer[doc->index + 1] == '>') {
                doc->lexer[doc->lexer_index] = doc->buffer[doc->index];
                doc->lexer[doc->lexer_index + 1] = '\0';
            }
            else doc->lexer[doc->lexer_index] = '\0';

            /* Set attribute key */
            attribute->key = _strdup(doc->lexer);

            /* Append attribute to node and reset */
            append_XMLItem(node->attributes, attribute);
            doc->lexer_index = 0;
            doc->index++;
            continue;
        }

        /* Inline node */
        if (doc->buffer[doc->index - 1] == '/' && doc->buffer[doc->index] == '>') {
            /* Terminate the tag index-1 since we dont want '/' in the tag */
            doc->lexer[doc->lexer_index - 1] = '\0';

            /* Ensure the tag is not already set */
            if (!node->tag)
                node->tag = _strdup(doc->lexer);

            /* Reset lexer and return */
            doc->index++;
            doc->lexer_index = 0;
            return true;
        }
    }
    return false;
}

/* Returns root node on success, on failure NULL ptr is returned */
XMLNode* parse_XML(XMLDocument* doc) {
    SXML_NODES = new_XMLList();
    SXML_ATTRIBUTES = new_XMLList();
    SXML_TEXT = new_XMLList();

    XMLNode* root = new_XMLNode(NULL);
    XMLNode* node = root;
    while (doc->buffer[doc->index] != '\0' && doc->index < doc->file_size) {

        /* Tag start */
        if (doc->buffer[doc->index] == '<') {

            /* Append inner_text to XMLNode OK */
            if (doc->lexer_index > 0) {
                if (!node) {
                    fprintf(stderr, "Text outside of document\n");
                    return NULL;
                }
                doc->lexer[doc->lexer_index] = '\0';
                XMLValue* text = new_XMLValue(_strdup(doc->lexer), XMLTypeText);
                append_XMLItem(node->inner_xml, text);
                append_XMLItem(SXML_TEXT, text->value);
                doc->lexer_index = 0;
            }

            /* End of node */
            if (doc->buffer[doc->index + 1] == '/') {

                /* skip /> */
                doc->index += 2;

                /* Get tag name */
                while (doc->buffer[doc->index] != '>')
                    doc->lexer[doc->lexer_index++] = doc->buffer[doc->index++];
                doc->lexer[doc->lexer_index] = '\0';

                /* Reached root. Free file and return root */
                if (node == root) {
                    free_File(doc);
                    ((XMLNode*)root->children->items[0])->parent = NULL;
                    return root->children->items[0];
                }

                /* Check if tag matches */
                if (node->tag == NULL || strcmp(node->tag, doc->lexer)) {
                    fprintf(stderr, "Mismatched tags (%s != %s)\n", node->tag, doc->lexer);
                    return NULL;
                }

                /* Take a step back to nodes parent */
                node = node->parent;
                doc->lexer_index = 0;
                doc->index++;
                continue;
            }

            /* Special node */
            if (doc->buffer[doc->index + 1] == '!') {
                /* Copy start of special node */
                while (doc->buffer[doc->index] != ' ' && doc->buffer[doc->index] != '>')
                    doc->lexer[doc->lexer_index++] = doc->buffer[doc->index++];
                doc->lexer[doc->lexer_index] = '\0';

                /* Check if special node is a comment */
                if (!strcmp(doc->lexer, "<!--")) {
                    doc->lexer[doc->lexer_index] = '\0';

                    /* Check if we have reached the end of the comment */
                    while (!ends_with(doc->lexer, "-->")) {
                        doc->lexer[doc->lexer_index++] = doc->buffer[doc->index++];
                        doc->lexer[doc->lexer_index] = '\0';
                    }
                    doc->lexer_index = 0;
                    continue;
                }
            }

            /* Declaration tag */
            if (doc->buffer[doc->index + 1] == '?') {
                /* Copy declaration tag name and NULL terminate */
                while (doc->buffer[doc->index] != ' ' && doc->buffer[doc->index] != '>') {
                    doc->lexer[doc->lexer_index++] = doc->buffer[doc->index++];
                }
                doc->lexer[doc->lexer_index] = '\0';

                /* Check if we have a xml declaration tag */
                if (!strcmp(doc->lexer, "<?xml")) {
                    doc->lexer_index = 0;

                    /* Create xml node and parse attributes */
                    XMLNode* declaration = new_XMLNode(NULL);
                    parse_XMLAttributes(doc, declaration);

                    /* Set the attributes of xml document */
                    doc->info = declaration->attributes;

                    continue;
                }
            }

            /* Set current node */
            node = new_XMLNode(node);

            /* Start tag */
            doc->index++;

            /* Incase we have a inline node go back to parent immediately */
            if (parse_XMLAttributes(doc, node)) {
                node = node->parent;
                doc->lexer_index = 0;
                doc->index++;
                continue;
            }

            /* Set tag if not set */
            doc->lexer[doc->lexer_index] = '\0';
            if (node->tag == NULL) {
                node->tag = _strdup(doc->lexer);
            }

            /* Reset lexer */
            doc->lexer_index = 0;
            doc->index++;
            continue;
        }
        else {

            /* Increase lexer_size if inner_text is greater than lexer_size */
            if (doc->lexer_index >= doc->lexer_size) {
                doc->lexer_size += EXPAND_LEXER_SIZE;
                doc->lexer = realloc(doc->lexer, sizeof(char) * doc->lexer_size);
                if (!doc->lexer) {
                    fprintf(stderr, "Unable to reallocate lexer\n");
                    exit(1);
                }
            }

            /* Ensure lexer is not a null ptr */
            if (!doc->lexer) {
                fprintf(stderr, "Lexer is null ptr\n");
                exit(1);
            }

            /* Add inner_text to lexer */
            doc->lexer[doc->lexer_index++] = doc->buffer[doc->index++];
        }
    }
    /* We are done parsing free file and return root */
    free_File(doc);
    if (root->children->count > 0) {
        ((XMLNode*)root->children->items[0])->parent = NULL;
        return root->children->items[0];
    }
    return NULL;
}

#endif /* SXML_H */