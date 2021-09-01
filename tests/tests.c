#include "../sxml.h"
#include "CuTest.h"

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
    CuAssertIntEquals(tc, 1,  load_file(gdoc, "./files/doc.xml"));
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
    CuAssertIntEquals(tc, 1, load_file(gdoc, "./files/declaration.xml"));
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
    CuAssertIntEquals(tc, 1, load_file(gdoc, "./files/attributes.xml"));
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

void test_parse_example(CuTest* tc) {
    gdoc = new_XMLDocument();
    CuAssertPtrNotNull(tc, gdoc);
    CuAssertIntEquals(tc, 1, load_file(gdoc, "./files/example.xml"));
    CuAssertPtrNotNull(tc, gdoc->buffer);
    if (gdoc->buffer)
        groot = parse_xml(gdoc);

    CuAssertIntEquals(tc, 15, SXML_NODES->count);
    CuAssertIntEquals(tc, 29, SXML_ATTRIBUTES->count);
    CuAssertIntEquals(tc, 20, SXML_TEXT->count);

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