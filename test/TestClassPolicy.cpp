#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <srcSAXHandler.hpp>
#include <srcPtrUtilities.hpp>
#include <ClassPolicy.hpp>
#include <cassert>
#include <srcml.h>

std::string StringToSrcML(std::string str){
	struct srcml_archive* archive;
	struct srcml_unit* unit;
	size_t size = 0;

	char * ch;

	archive = srcml_archive_create();
	srcml_archive_enable_option(archive, SRCML_OPTION_POSITION);
	srcml_archive_write_open_memory(archive, &ch, &size);

	unit = srcml_unit_create(archive);
	srcml_unit_set_language(unit, SRCML_LANGUAGE_CXX);
	srcml_unit_set_filename(unit, "testsrcType.cpp");

	srcml_unit_parse_memory(unit, str.c_str(), str.size());
	srcml_archive_write_unit(archive, unit);

	srcml_unit_free(unit);
	srcml_archive_close(archive);
	srcml_archive_free(archive);

	std::string srcml;
	srcml.append(ch, size);

	return srcml;
}

class TestClassPolicy : public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener{
    public:

        ~TestClassPolicy() { }

        TestClassPolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners) { }

        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            datatotest = *policy->Data<std::vector<Class>>();
        }

		void RunTest(){
			assert(datatotest[0].className == "foo_data");
			assert(datatotest[0].members[0].nameofidentifier == "z");
			assert(datatotest[0].members[1].nameofidentifier == "y");

			assert(datatotest[1].className == "foo");
			assert(datatotest[1].members[0].nameofidentifier == "x");
			assert(datatotest[1].methods[0].functionName == "Print");
		}
    protected:

        void * DataInner() const override {
            return (void*)0;
        }

    private:

        ClassPolicy classpolicy;
        Class classdata;
        std::vector<Class> datatotest;

};

int main(int argc, char** filename){
	std::string codestr = "class foo { \n public: \n void Print() { }; \n int x; \n struct foo_data { \n int z; \n int y; \n }; \n };";
	std::string srcmlstr = StringToSrcML(codestr);
	
	try {
    	TestClassPolicy classdata;
    	srcSAXController control(srcmlstr);
    	srcSAXEventDispatch::srcSAXEventDispatcher<ClassPolicy> handler { &classdata };
    	control.parse(&handler); //Start parsing
    	classdata.RunTest();
    } catch(SAXError e) {
		std::cerr << e.message;
	}
}
