#include "sxml.h"
#include "./libs/CuTest.h"

XMLDocument* gdoc;
XMLNode* groot;

void test_init_XMLDocument(CuTest* tc){
    gdoc = new_XMLDocument();
    CuAssertPtrNotNull(tc, gdoc);
    CuAssertPtrNotNull(tc, gdoc->lexer);
    
    CuAssertIntEquals(tc, EXPAND_LEXER_SIZE,    gdoc->lexer_size);
    CuAssertIntEquals(tc, 0,                    gdoc->lexer_index);
    CuAssertIntEquals(tc, 0,                    gdoc->index);
    CuAssertPtrEquals(tc, NULL,                 gdoc->buffer);
    CuAssertPtrEquals(tc, NULL,                 gdoc->info);
}

void test_load_XMLDocument(CuTest* tc){
    CuAssertIntEquals(tc, 1,  load_file(gdoc, "./tests/doc.xml"));
    CuAssertPtrNotNull(tc, gdoc->buffer);
    CuAssertIntEquals(tc, 12, gdoc->file_size);
    
    CuAssertStrEquals(tc, "<DOC></DOC>\0",  gdoc->buffer);
}

void test_parse_XMLDocument(CuTest* tc){
    if(gdoc->buffer)
        groot = parse_xml(gdoc);

    CuAssertPtrNotNullMsg(tc, "Failed to parse XMLDocument", groot);
    CuAssertPtrNotNull(tc, groot->children);
    CuAssertPtrNotNull(tc, groot->attributes);
    CuAssertPtrNotNull(tc, groot->inner_xml);
    CuAssertPtrNotNullMsg(tc, "Failed to initialise Node stack", SXML_NODES);
    CuAssertPtrNotNullMsg(tc, "Failed to initialise Attributes stack", SXML_ATTRIBUTES);
    CuAssertPtrNotNullMsg(tc, "Failed to initialise Text stack", SXML_TEXT);
    
    CuAssertIntEquals(tc, 2, SXML_NODES->count);
    CuAssertIntEquals(tc, 0, SXML_ATTRIBUTES->count);
    CuAssertIntEquals(tc, 0, SXML_TEXT->count);

    CuAssertPtrEquals(tc, NULL, groot->parent);

    CuAssertIntEquals(tc, 0, groot->children->count);
    CuAssertIntEquals(tc, 0, groot->attributes->count);
    CuAssertIntEquals(tc, 0, groot->children->count);
    
    CuAssertStrEquals(tc, "DOC", groot->tag);
    free_XMLDocument(gdoc);
    free_XMLStacks();
}

void test_parse_declaration_XMLNode(CuTest* tc){
    gdoc = new_XMLDocument();
    CuAssertPtrNotNull(tc, gdoc);
    CuAssertIntEquals(tc, 1, load_file(gdoc, "./tests/declaration.xml"));
    CuAssertPtrNotNull(tc, gdoc->buffer);
    if(gdoc->buffer)
        groot = parse_xml(gdoc);

    CuAssertIntEquals(tc, 2, SXML_NODES->count);
    CuAssertIntEquals(tc, 2, SXML_ATTRIBUTES->count);
    CuAssertIntEquals(tc, 0, SXML_TEXT->count);

    CuAssertPtrNotNull(tc, gdoc->info);
    CuAssertIntEquals(tc, 2, gdoc->info->count);
    
    
    XMLAttribute* version = gdoc->info->items[0];
    CuAssertStrEquals(tc, "version", version->key);
    CuAssertStrEquals(tc, "1.0", version->value);

    XMLAttribute* encoding = gdoc->info->items[1];
    CuAssertStrEquals(tc, "encoding", encoding->key);
    CuAssertStrEquals(tc, "utf-8", encoding->value);
    free_XMLDocument(gdoc);
    free_XMLStacks();
}

void test_parse_attributes(CuTest* tc) {
    gdoc = new_XMLDocument();
    CuAssertPtrNotNull(tc, gdoc);
    CuAssertIntEquals(tc, 1, load_file(gdoc, "./tests/attributes.xml"));
    CuAssertPtrNotNull(tc, gdoc->buffer);
    if (gdoc->buffer)
        groot = parse_xml(gdoc);

    CuAssertIntEquals(tc, 2, SXML_NODES->count);
    CuAssertIntEquals(tc, 6, SXML_ATTRIBUTES->count);
    CuAssertIntEquals(tc, 0, SXML_TEXT->count);

    XMLAttribute* svalue = get_XMLAttribute(groot, "svalue");
    XMLAttribute* title = get_XMLAttribute(groot, "title");
    XMLAttribute* escape = get_XMLAttribute(groot, "escape");
    XMLAttribute* mvalue = get_XMLAttribute(groot, "mvalue");
    XMLAttribute* single = get_XMLAttribute(groot, "single");
    XMLAttribute* evalue = get_XMLAttribute(groot, "evalue");

    CuAssertPtrNotNull(tc, svalue);
    CuAssertPtrNotNull(tc, title);
    CuAssertPtrNotNull(tc, escape);
    CuAssertPtrNotNull(tc, mvalue);
    CuAssertPtrNotNull(tc, single);
    CuAssertPtrNotNull(tc, evalue);

    CuAssertPtrEquals(tc, NULL, svalue->value);
    CuAssertStrEquals(tc, "Good window", title->value);
    CuAssertStrEquals(tc, "aaa\"test\'bbb", escape->value);
    CuAssertPtrEquals(tc, NULL, mvalue->value);
    CuAssertStrEquals(tc, "works", single->value);
    CuAssertPtrEquals(tc, NULL, evalue->value);

    free_XMLDocument(gdoc);
    free_XMLStacks();
}

void test_parse_inline(CuTest* tc) {
    gdoc = new_XMLDocument();
    CuAssertPtrNotNull(tc, gdoc);
    CuAssertIntEquals(tc, 1, load_file(gdoc, "./tests/inline.xml"));
    CuAssertPtrNotNull(tc, gdoc->buffer);
    if (gdoc->buffer)
        groot = parse_xml(gdoc);

    CuAssertIntEquals(tc, 2, SXML_NODES->count);
    CuAssertIntEquals(tc, 5, SXML_ATTRIBUTES->count);
    CuAssertIntEquals(tc, 0, SXML_TEXT->count);

    CuAssertStrEquals(tc, "br", groot->tag);

    CuAssertPtrNotNull(tc, groot->children);
    CuAssertPtrNotNull(tc, groot->attributes);

    CuAssertIntEquals(tc, 0, groot->children->count);
    CuAssertIntEquals(tc, 5, groot->attributes->count);

    XMLAttribute* sv = get_XMLAttribute(groot, "sv");
    XMLAttribute* va = get_XMLAttribute(groot, "va");
    XMLAttribute* mv = get_XMLAttribute(groot, "mv");
    XMLAttribute* vb = get_XMLAttribute(groot, "vb");
    XMLAttribute* ev = get_XMLAttribute(groot, "ev");

    CuAssertPtrNotNull(tc, sv);
    CuAssertPtrNotNull(tc, va);
    CuAssertPtrNotNull(tc, mv);
    CuAssertPtrNotNull(tc, vb);
    CuAssertPtrNotNull(tc, ev);

    CuAssertPtrEquals(tc, NULL, sv->value);
    CuAssertStrEquals(tc, "aaa", va->value);
    CuAssertPtrEquals(tc, NULL, mv->value);
    CuAssertStrEquals(tc, "bbb", vb->value);
    CuAssertPtrEquals(tc, NULL, ev->value);

    free_XMLDocument(gdoc);
    free_XMLStacks();
}

void test_inner_xml(CuTest* tc) {
    gdoc = new_XMLDocument();
    CuAssertPtrNotNull(tc, gdoc);
    CuAssertIntEquals(tc, 1, load_file(gdoc, "./tests/inner.xml"));
    CuAssertPtrNotNull(tc, gdoc->buffer);
    if (gdoc->buffer)
        groot = parse_xml(gdoc);

    CuAssertIntEquals(tc, 4, SXML_NODES->count);
    CuAssertIntEquals(tc, 1, SXML_ATTRIBUTES->count);
    CuAssertIntEquals(tc, 4, SXML_TEXT->count);

    CuAssertStrEquals(tc, "DOC", groot->tag);

    CuAssertPtrNotNull(tc, groot->children);
    CuAssertPtrNotNull(tc, groot->attributes);
    CuAssertPtrNotNull(tc, groot->inner_xml);

    CuAssertIntEquals(tc, 2, groot->children->count);
    CuAssertIntEquals(tc, 0, groot->attributes->count);
    CuAssertIntEquals(tc, 5, groot->inner_xml->count);

    int inner_xml_types[] = { 0,1,0,1,0 };
    char inner_xml_text[5][50] = {
        "Hello this is the first string",
        "br",
        "This is the second string\nhas multiple lines",
        "p",
        "final string",
    };

    for (int i = 0; i < groot->inner_xml->count; i++) {
        XMLValue* item = groot->inner_xml->items[i];
        CuAssertIntEquals(tc, inner_xml_types[i], item->type);
        if (item->type)
            CuAssertStrEquals(tc, inner_xml_text[i], ((XMLNode*)item->value)->tag);
        else
            CuAssertStrEquals(tc, inner_xml_text[i], (char*)item->value);
    }

    free_XMLDocument(gdoc);
    free_XMLStacks();
}

void test_parse_example(CuTest* tc) {
    gdoc = new_XMLDocument();
    CuAssertPtrNotNull(tc, gdoc);
    CuAssertIntEquals(tc, 1, load_file(gdoc, "./tests/example.xml"));

    CuAssertPtrNotNull(tc, gdoc->buffer);
    if (gdoc->buffer)
        groot = parse_xml(gdoc);

    CuAssertIntEquals(tc, 15, SXML_NODES->count);
    CuAssertIntEquals(tc, 29, SXML_ATTRIBUTES->count);
    CuAssertIntEquals(tc, 5, SXML_TEXT->count);

    free_XMLDocument(gdoc);
}

void test_free_XMLStacks(CuTest* tc){
    free_XMLStacks();
    CuAssertPtrEquals(tc, NULL, SXML_NODES);
    CuAssertPtrEquals(tc, NULL, SXML_ATTRIBUTES);
    CuAssertPtrEquals(tc, NULL, SXML_TEXT);
}

/* Add all the tests to the test suite. */
CuSuite* test_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_init_XMLDocument);
    SUITE_ADD_TEST(suite, test_load_XMLDocument);
    SUITE_ADD_TEST(suite, test_parse_XMLDocument);
    SUITE_ADD_TEST(suite, test_parse_declaration_XMLNode);
    SUITE_ADD_TEST(suite, test_parse_attributes);
    SUITE_ADD_TEST(suite, test_parse_inline);
    SUITE_ADD_TEST(suite, test_inner_xml);
    SUITE_ADD_TEST(suite, test_parse_example);
    SUITE_ADD_TEST(suite, test_free_XMLStacks);
    return suite;
}

/* Runs all the tests and prints the result. */
void all_tests() {
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	CuSuiteAddSuite(suite, test_suite());

	CuSuiteRun(suite);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}


int main(void) {
  all_tests();
  return 0;
}