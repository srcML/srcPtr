#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <srcSAXHandler.hpp>
#include <srcPtrUtilities.hpp>
#include <CallPolicy.hpp>
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

class TestCallPolicy : public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener{
    public:

        ~TestCallPolicy() { }

        TestCallPolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners) { }

        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            calldata = *policy->Data<CallPolicy::CallData>();
            datatotest.push_back(calldata);
        }

		void RunTest(){
			assert(datatotest[0].fnName == "bar.f");
		}

    protected:

        void * DataInner() const override {
            return (void*)0;
        }

    private:

        CallPolicy callpolicy;
        CallPolicy::CallData calldata;
        std::vector<CallPolicy::CallData> datatotest;

};

int main(int argc, char** filename){
	std::string codestr = "class foo { \n public: \n void f() { int y; } \n int x;}; \n int main() \n { \n foo bar; \n bar.f();}";
	std::string srcmlstr = StringToSrcML(codestr);


	try {
    	TestCallPolicy calldata;
    	srcSAXController control(srcmlstr);
    	srcSAXEventDispatch::srcSAXEventDispatcher<CallPolicy> handler { &calldata };
    	control.parse(&handler); //Start parsing
    	calldata.RunTest();
    } catch(SAXError e) {
		std::cerr << e.message;
	}
}
