# sxml
A simple xml parser.

## Features
- Single header C99
- Parsing xml :D
- Keeps track of the order of when inner_text and nodes are inserted into nodes.
- The ability to escape `"` and `'` in attrubte values using `\`.
- Attributes without values.

## Build demo and test

To build the tests run `make test` this will create the `test` executable.  
Running `./test` will output if all tests passes:
```
Runing 9 tests:

1) INIT_XMLDOCUMENT:          Passed
2) LOAD_XMLDOCUMENT:          Passed
3) PARSE_XMLDOCUMENT:         Passed
4) PARSE_DECLARATION_XMLNODE: Passed
5) PARSE_ATTRIBUTES:          Passed
6) PARSE_INLINE:              Passed
7) INNER_XML:                 Passed
8) PARSE_EXAMPLE:             Passed
9) FREE_XMLSTACKS:            Passed

Runs: 9 Passes: 9 Fails: 0
```
To build the demo run `make demo` this will create the `demo` executable.  
Running `./demo` will output:
```
 DOC title="document"
    window auto="(null)" title=""Window 1'" width="200" height="200" x="0" y="0" notitle="(null)"
        p
    window title="Window 2" width="400" height="200" x="200" y="0"
        p
        br
        layout rows="2" widths="46,-1"
            label
            slider min="0" max="255" step="1" value="42"
            label
            slider min="0" max="255" step="1" value="69"
            label
            slider min="0" max="255" step="1" value="128"
```

## Usage

### Parse xml file
___
First to load a document you have to initialise a document by doing `XMLDocument* doc = new_XMLDocument()`.  
Then load the document using `load_file(doc, file_path)` this will return `1` if it succeed in loading the file.  
Lastly to parse file run `XMLNode* root = parse_xml(doc)` now you got the xml root node.  

### Free
___
Use `free_XMLStacks()` to free all `XMLNodes` and `XMLAttributes`.  
Use `free_XMLDocument(doc)` to free the `XMLDocument`.  
The document buffer and lexer are freed when `parse_XML()` is done parsing. 

### XML node
___
```c
struct XMLNode {
    char* tag;              // Name of node.
    XMLNode* parent;        // Reference to the parent node (NULL if no parent exists).
    XMLList* inner_xml;     // Current index and array of inner_xml.
    XMLList* attributes;    // Current index and array of attributes.
    XMLList* children;      // Current index and array of nodes. 
};
```

### XML attribute 
___
```c
struct XMLAttribute {
    char* key;      // Name of attribute.
    char* value;    // Value of attribute.
}
```

### XML list
___
```c
struct XMLList {
    int heap_size;  // The amount of items allocated on the heap.
    int count;      // The amount of items.
    void** items;   // Array of items.
};
```

### XML value
___
`XMLValue` is used with `node->inner_xml` to keep track of when nodes and inner text is added to a node.
```c
enum XMLType {
    XMLTypeText,
    XMLTypeNode
};

typedef struct _XMLValue {
    enum XMLType type;  // The type of value.
    void* value;        // The actual value as a null pointer. 
} XMLValue;
```

## Todo
- [x] Add tests
    - [x] node
    - [x] inline node
    - [x] attributes
    - [x] children
    - [x] inner_xml
    - [x] document
- [x] Make lists generic using void pointers.
- [x] Store the order of inline text and tags.
- [x] Add a better way for user to be able to itterat over attributes & nodes.
  - [x] this was done by having better names. No more (`node->attributes->attributes`) instead we have (`node->attributes->items`) and (`attributes->index`) has been replaced with (`attributes->count`) making it eaisier to read when making loops.
