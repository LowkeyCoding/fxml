# sxml
A simple xml parser.

## Features
- Single header C99
- Parsing xml :D
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
```c
struct XMLNode {
    char* tag;              // Name of node.
    char* inner_text;       // Inner_text of node.
    XMLNode* parent;        // Reference to the parent node (NULL if no parent exists).
    XMLList* attributes;    // A struct containing the allocated size, current index and array of attributes.
    XMLList* children;      // A struct containing the allocated size, current index and array of nodes. 
};
```

### XML attribute 
```c
struct XMLAttribute {
    char* key;      // Name of attribute.
    char* value;    // Value of attribute.
}
```

### XML list
```c
struct XMLList {
    int heap_size;  // The amount of items allocated on the heap.
    int count;      // The amount of items.
    void** items;   // Array of items.
};
```

## Todo
- [x] Make lists generic using void pointers.
- [ ] Store the order of inline text and tags.
- [x] Add a better way for user to be able to itterat over attributes & nodes.
  - [x] this was done by having better names. No more (`node->attributes->attributes`) instead we have (`node->attributes->items`) and (`attributes->index`) has been replaced with (`attributes->count`) making it eaisier to read when making loops.
