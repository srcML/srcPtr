#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <srcSAXHandler.hpp>

#include <srcPtrDeclPolicy.hpp>
#include <srcPtrPolicy.hpp>
#include <srcPtrPolicyData.hpp>

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

srcPtrData * Analyze(std::string codestr) {
	std::string srcmlstr = StringToSrcML(codestr);

	// First Run
	srcPtrDeclPolicy *declpolicy = new srcPtrDeclPolicy();
	srcSAXController control(srcmlstr);
	srcSAXEventDispatch::srcSAXEventDispatcher<> handler{declpolicy};
	control.parse(&handler);

	// Second Run
	srcPtrPolicy *policy = new srcPtrPolicy(declpolicy->GetData(), new srcPtrDataMap());
	srcSAXController control2(srcmlstr);
	srcSAXEventDispatch::srcSAXEventDispatcher<> handler2{policy};
	control2.parse(&handler2);

	srcPtrData *data = policy->GetData()->Clone();

	return data;
}

void RunTests() {
	{
		srcPtrData* data = Analyze("int main() {\nint x;\nint * y;\ny = &x;\n}");

		assert(data->GetPointers().size() == 1);
		assert(data->GetPointsTo(data->GetPointers()[0]).size() == 1);
		assert(data->GetPointsTo(data->GetPointers()[0])[0].nameofidentifier == "x");
	}
	{
		srcPtrData* data = Analyze("int main() {\nint x = 0;\nint& y;\ny = x;\ny = 12;\nint& y2 = x;\ny2 = 12;\nint* z;\nz = &x;\nint* z2 = &x;\n}");

		assert(data->GetPointers().size() == 4);

		assert(data->GetPointsTo(data->GetPointers()[0]).size() == 1);
		assert(data->GetPointsTo(data->GetPointers()[1]).size() == 1);
		assert(data->GetPointsTo(data->GetPointers()[2]).size() == 1);
		assert(data->GetPointsTo(data->GetPointers()[3]).size() == 1);

		assert(data->GetPointsTo(data->GetPointers()[0])[0].nameofidentifier == "x");
		assert(data->GetPointsTo(data->GetPointers()[1])[0].nameofidentifier == "x");
		assert(data->GetPointsTo(data->GetPointers()[2])[0].nameofidentifier == "x");
		assert(data->GetPointsTo(data->GetPointers()[3])[0].nameofidentifier == "x");
	}
	{
		srcPtrData* data = Analyze("void func(int *x) {\n   *x = 12;\nint *a = x;\n}\n\nint main() {\nint *y;\nint z;\ny = &z;\nfunc(y);\n}");

		assert(data->GetPointers().size() == 3);

		assert(data->GetPointers()[0].nameofidentifier == "a");
		assert(data->GetPointers()[1].nameofidentifier == "x");
		assert(data->GetPointers()[2].nameofidentifier == "y");

		assert(data->GetPointsTo(data->GetPointers()[0]).size() == 1);
		assert(data->GetPointsTo(data->GetPointers()[1]).size() == 1);
		assert(data->GetPointsTo(data->GetPointers()[2]).size() == 1);

		assert(data->GetPointsTo(data->GetPointers()[0])[0].nameofidentifier == "x");
		assert(data->GetPointsTo(data->GetPointers()[1])[0].nameofidentifier == "y");
		assert(data->GetPointsTo(data->GetPointers()[2])[0].nameofidentifier == "z");
	}
}

int main() {
	RunTests();
	return 0;
}
