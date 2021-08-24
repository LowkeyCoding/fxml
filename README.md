# sxml
A simple xml parser.

## Features
- Single header C99
- Parsing xml :D
- Keeps track of the order of when inner_text and nodes are inserted into nodes.
- The ability to escape `"` and `'` in attrubte values using `\`.
- Attributes without values.

## Usage

### Parse xml file
___
First to load a document you have to initialise a document by doing `XMLDocument* doc = new_XMLDocument()`.  
Then load the document using `load_File(doc, file_path)` this will return `1` if it succeed in loading the file.  
Lastly to parse file run `XMLNode* root = parse_XML(doc)` now you got the xml root node.  

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
    char* inner_text;       // Inner_text of node.
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
- [x] Make lists generic using void pointers.
- [x] Store the order of inline text and tags.
- [x] Add a better way for user to be able to itterat over attributes & nodes.
  - [x] this was done by having better names. No more (`node->attributes->attributes`) instead we have (`node->attributes->items`) and (`attributes->index`) has been replaced with (`attributes->count`) making it eaisier to read when making loops.
