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

struct _XMLAttribute {
    char* key;
    char* value;
};

struct _XMLAttributeList {
    int heap_size;
    int index;
    struct _XMLAttribute** attributes;
};

typedef struct _XMLAttributeList XMLAttributeList;

typedef struct _XMLAttribute XMLAttribute;

XMLAttributeList* new_XMLAttributeList(void);
void append_XMLAttribute(XMLAttributeList*, XMLAttribute*);
void free_XMLAttributeList(XMLAttributeList* list);

struct _XMLNode {
    char* tag;
    char* inner_text;
    struct _XMLNode* parent;
    XMLAttributeList* attributes;
    struct _XMLNodeList* children;
};

struct _XMLNodeList {
    int heap_size;
    int index;
    struct _XMLNode** children;
};

/* XML NODE */

typedef struct _XMLNodeList XMLNodeList;

typedef struct _XMLNode XMLNode;

XMLNodeList* new_XMLNodeList(void);

XMLNode* new_XMLNode(XMLNode*);

void append_XMLNode(XMLNodeList*, XMLNode*);

void free_XMLNodeList(XMLNodeList*);

/* XML DOCUMENT */

typedef struct _XMLDocument {
    char* buffer;
    char* lexer;
    XMLAttributeList* info;
    size_t lexer_size;
    size_t lexer_index;
    size_t index;
    size_t file_size;
} XMLDocument;

/* NODE & ATRIBUTE STACK */
XMLNodeList* SFXML_NODES;
XMLAttributeList* SFXML_ATTRIBUTES;

/* NODE IMPLEMENTATION */
XMLNode* new_XMLNode(XMLNode* parent) {
    XMLNode* node = malloc(sizeof(XMLNode));
    if (!node) {
        printf("Unable to allocate node\n");
        exit(1);
    }
    node->parent = parent;

    node->children = new_XMLNodeList();
    node->attributes = new_XMLAttributeList();

    node->tag = NULL;
    node->inner_text = NULL;
    if (parent != NULL)
        append_XMLNode(parent->children, node);
    append_XMLNode(SFXML_NODES, node);
    return node;
}

void print_XMLNode(XMLNode* node, int indent) {
    printf("%*s%s", 4 * indent, " ", node->tag);
    for (int i = 0; i < node->attributes->index; i++) {
        printf(" %s=\"%s\"", node->attributes->attributes[i]->key, node->attributes->attributes[i]->value);
    }
    printf("\n");
    for (int i = 0; i < node->children->index; i++) {
        print_XMLNode(node->children->children[i], indent + 1);
    }
}

void free_XMLNode(XMLNode* node) {
    /* Free tag & text */
    if (node->tag)
        free(node->tag);
    if (node->inner_text)
        free(node->inner_text);

    free_XMLNodeList(node->children);
    free_XMLAttributeList(node->attributes);
    free(node);
}

/* NODE LIST IMPLEMENTATION */
XMLNodeList* new_XMLNodeList(void) {
    XMLNodeList* list = malloc(sizeof(XMLNodeList));
    if (!list) {
        printf("cannot allocate list\n");
        exit(1);
    }
    list->index = 0;
    list->heap_size = NODE_SIZE;
    list->children = malloc(sizeof(XMLNode*) * list->heap_size);
    return list;
}

void append_XMLNode(XMLNodeList* list, XMLNode* node) {
    if (list->index >= list->heap_size) {
        list->heap_size *= 2;
        list->children = realloc(list->children, sizeof(XMLNode*) * list->heap_size);
        if (list->children == NULL) {
            printf("Unable to reallocate node list\n");
        }
    }
    list->children[list->index++] = node;
}

void free_XMLNodeList(XMLNodeList* list) {
    free(list->children);
    free(list);
}

void free_XMLNodes(void) {
    for (int i = 0; i < SFXML_NODES->index; i++) {
        free_XMLNode(SFXML_NODES->children[i]);
    }
    free(SFXML_NODES->children);
    free(SFXML_NODES);
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

    append_XMLAttribute(SFXML_ATTRIBUTES, attribute);
    return attribute;
}

XMLAttribute* get_XMLAttribute(XMLNode* node, char* key) {
    for (int i = 0; i < node->attributes->index; i++) {
        XMLAttribute* attribute = node->attributes->attributes[i];
        if (!strcmp(attribute->key, key)) {
            return attribute;
        }
    }
    return NULL;
}

void free_XMLAttribute(XMLAttribute* attribute) {
    if (attribute->key)
        free(attribute->key);
    if (attribute->value)
        free(attribute->value);
    free(attribute);
}

/* ATTRIBUTE LIST IMPLEMENTATION */
XMLAttributeList* new_XMLAttributeList(void) {
    XMLAttributeList* list = malloc(sizeof(XMLAttributeList));
    if (!list) {
        printf("cannot allocate attribute list\n");
        exit(1);
    }
    list->index = 0;
    list->heap_size = ATTRIBUTE_SIZE;
    list->attributes = malloc(sizeof(XMLAttribute*) * list->heap_size);
    return list;
}

void append_XMLAttribute(XMLAttributeList* list, XMLAttribute* attribute) {
    if (list->index >= list->heap_size) {
        list->heap_size *= 2;
        list->attributes = realloc(list->attributes, sizeof(XMLAttribute*) * list->heap_size);
        if (!list->attributes) {
            printf("Unable to reallocate node list\n");
        }
    }
    list->attributes[list->index++] = attribute;
}

void free_XMLAttributeList(XMLAttributeList* list) {
    if (list->attributes) {
        free(list->attributes);
    }
    free(list);
}

void free_XMLAttributes(void) {
    for (int i = 0; i < SFXML_ATTRIBUTES->index; i++) {
        free_XMLAttribute(SFXML_ATTRIBUTES->attributes[i]);
    }
    free(SFXML_ATTRIBUTES->attributes);
    free(SFXML_ATTRIBUTES);
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
        doc->buffer = (char*)calloc(sizeof(char), size + 1);
        doc->file_size = size;
        
        /* Read file into the buffer */
        fread(doc->buffer, 1, size, file);
        fclose(file);
        return true;
    }
    return false;
}

void free_XMLDocument(XMLDocument* doc) {
    if (doc->lexer)
        free(doc->lexer);
    if (doc->buffer)
        free(doc->buffer);
    free(doc);
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
    XMLAttribute* attribute = new_XMLAttribute();
    while (doc->buffer[doc->index] != '>') {
        doc->lexer[doc->lexer_index++] = doc->buffer[doc->index++];
        
        /* Tag name */
        if (doc->buffer[doc->index] == ' ' && !node->tag) {
            doc->lexer[doc->lexer_index] = '\0';
            node->tag = strdup(doc->lexer);
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
            
            /* NULL terminate string then copy it to the attribute */
            doc->lexer[doc->lexer_index] = '\0';
            attribute->key = strdup(doc->lexer);
            
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
            bool escaped = false;
            while (doc->buffer[doc->index] != '"' && doc->buffer[doc->index] != '\'') {
                
                /* Check if the next char is escaped */
                if (doc->buffer[doc->index] == '\\') {
                    escaped = true;

                    /* Copy the escaped character instead of the '\' */
                    doc->lexer[doc->lexer_index++] = doc->buffer[doc->index+1];
                    
                    /* Skip over the '\' and the escaped character */
                    doc->index += 2;
                } else {
                    escaped = false;
                    doc->lexer[doc->lexer_index++] = doc->buffer[doc->index++];
                }
            }

            /* NULL terminate and add value to attribute */
            doc->lexer[doc->lexer_index++] = '\0';
            attribute->value = strdup(doc->lexer);

            /* Append attribute to node and reset */
            append_XMLAttribute(node->attributes, attribute);
            attribute = new_XMLAttribute();
            doc->lexer_index = 0;
            doc->index++;
            continue;
        }

        /* Incase attribute does not have a value */
        if ((doc->buffer[doc->index] == ' ' || doc->buffer[doc->index + 1] == '>') 
            && node->tag && doc->lexer_index > 0) {
            
            /* Copy a the last character incase where the attribute is at the end of the node */
            if (doc->buffer[doc->index + 1] == '>') {
                doc->lexer[doc->lexer_index] = doc->buffer[doc->index];
                doc->lexer[doc->lexer_index + 1] = '\0';
            } else doc->lexer[doc->lexer_index] = '\0';
            
            /* Set attribute key */
            attribute->key = strdup(doc->lexer);

            /* Append attribute to node and reset */
            append_XMLAttribute(node->attributes, attribute);
            attribute = new_XMLAttribute();
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
                node->tag = strdup(doc->lexer);

            /* Reset lexer and return */
            doc->index++;
            doc->lexer_index = 0;
            return true;
        }

    }

    return false;
}

/* Returns root node on succes, on failure NULL ptr is returned */
XMLNode* parse_XML(XMLDocument* doc) {
    SFXML_NODES = new_XMLNodeList();
    SFXML_ATTRIBUTES = new_XMLAttributeList();

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
                /* TODO: Append text to inner_text if inner_text is not NULL */
                if (node->inner_text == NULL) {
                    doc->lexer[doc->lexer_index] = '\0';
                    node->inner_text = strdup(doc->lexer);
                }
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
                
                /* Reached root */
                if (node == root) {
                    return root->children->children[0];
                }

                /* Check if tag matches */
                if (node->tag == NULL || strcmp(node->tag, doc->lexer)) {
                    fprintf(stderr, "Mismatched tags (%s != %s)\n", node->tag, doc->lexer);
                    return root->children->children[0];
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
                node->tag = strdup(doc->lexer);
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
    
    /* We are done parsing return root */
    return root->children->children[0];
}

#endif /* SXML_H */