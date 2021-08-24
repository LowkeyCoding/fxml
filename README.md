# sxml
A simple xml parser.

## Features
- Parsing xml :D
- The ability to escape `"` and `'` in attrubte values using `\`.
- Attributes without values.

## Usage

### Parse xml file
First to load a document you have to initialise a document by doing `XMLDocument* doc = new_XMLDocument()`.  
Then load the document using `load_File(doc, file_path)` this will return `1` if it succeed in loading the file.  
Lastly to parse file run `XMLNode* root = parse_XML(doc)` now you got the xml root node.

### XML node structure
```c
struct XMLNode {
    char* tag;                        // Name of node.
    char* inner_text;                 // Inner_text of node.
    XMLNode* parent;                  // Reference to the parent node (NULL if no parent exists).
    XMLAttributeList* attributes;     // A struct containing the allocated size, current index and array of attributes.
    XMLNodeList* children;            // A struct containing the allocated size, current index and array of nodes. 
};
```

### XML attribute list structure
```c
struct XMLAttributeList {
    int heap_size;                // Allocated size for the attribute array.
    int index;                    // Current attribute count.
    XMLAttribute** attributes;    // Attribute array.
};
```

### XML node list structure
```c
struct XMLNodeList {
    int heap_size;         // Allocated size for the node array.
    int index;             // Current node count.
    XMLNode** children;    // Node array.
};
```

## Todo
- [ ] Make lists generic using void pointers.
- [ ] Store the order of inline text and tags.
- [ ] Add a better way for user to be able to itterat over attributes & nodes.
