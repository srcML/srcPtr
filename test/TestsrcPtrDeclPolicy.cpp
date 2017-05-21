#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <srcSAXHandler.hpp>

#include <srcPtrDeclPolicy.hpp>
#include <srcPtrPolicy.hpp>
#include <srcPtrPolicyTemplates.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <cassert>

#include <srcml.h>

std::string StringToSrcML(std::string str){
	struct srcml_archive* archive;
	struct srcml_unit* unit;
	size_t size = 0;

	char *ch = new char[str.size()];

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
	
	return std::string(ch);
}

srcPtrDeclPolicy::srcPtrDeclData Analyze(std::string codestr) {
	std::string srcmlstr = StringToSrcML(codestr);

	srcPtrDeclPolicy *declpolicy = new srcPtrDeclPolicy();
	srcSAXController control(srcmlstr);
	srcSAXEventDispatch::srcSAXEventDispatcher<> handler{declpolicy};
	control.parse(&handler);
	
	return declpolicy->GetData();
}

void RunTests() {
	{
		srcPtrDeclPolicy::srcPtrDeclData data = Analyze("int main() {\nint x;\nint * y;\ny = &x;\n}");
		assert(data.functionTracker.GetFunction("main", 0).functionName == "main");
	}
	std::cout << std::endl << "Finished Testing srcPtrDeclPolicy" << std::endl;
}

int main() {
	RunTests();
	return 0;
}
