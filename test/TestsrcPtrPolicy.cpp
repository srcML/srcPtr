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
#include <vector>
#include <utility> 

#include <srcml.h>

class srcPtrTestAlgorithm {
public:
	~srcPtrTestAlgorithm() { }; 

	void AddPointsToRelationship(Variable lhs, Variable rhs) { pointsToRelationships.push_back(std::pair<Variable, Variable>(lhs, rhs)); }

	void AddAssignmentRelationship(Variable lhs, Variable rhs) { assignmentRelationships.push_back(std::pair<Variable, Variable>(lhs, rhs)); }

	void Print() { }

	void PrintGraphViz() { }

	std::vector<Variable> GetPointsTo(Variable ptr) { return std::vector<Variable>(); }

	std::vector<Variable> GetPointers() { return std::vector<Variable>(); }

	srcPtrTestAlgorithm *Clone() const { return (new srcPtrTestAlgorithm(*this)); }

	std::vector<std::pair<Variable, Variable>> pointsToRelationships;
	std::vector<std::pair<Variable, Variable>> assignmentRelationships;

};

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

srcPtrTestAlgorithm * Analyze(std::string codestr) {
	try {
		std::string srcmlstr = StringToSrcML(codestr);

		// First Run
		srcPtrDeclPolicy *declpolicy = new srcPtrDeclPolicy();
		srcSAXController control(srcmlstr);
		srcSAXEventDispatch::srcSAXEventDispatcher<> handler{declpolicy};
		control.parse(&handler);

		// Second Run
		srcPtrTestAlgorithm* algorithm;
		srcPtrPolicy<srcPtrTestAlgorithm> *policy = new srcPtrPolicy<srcPtrTestAlgorithm>(declpolicy->GetData());
		srcSAXController control2(srcmlstr);
		srcSAXEventDispatch::srcSAXEventDispatcher<> handler2{policy};
		control2.parse(&handler2);
 
		srcPtrTestAlgorithm *data = policy->GetData()->Clone();

		return data;
	} catch(SAXError e) {
		std::cerr << e.message;
	}
}

void TestAssignments() {
	{
		srcPtrTestAlgorithm* data = Analyze("int main() { int x; int * y; y = &x; }");

		assert(data->assignmentRelationships.size() == 0);
		assert(data->pointsToRelationships[0].first.nameofidentifier == "y");
		assert(data->pointsToRelationships[0].second.nameofidentifier == "x");

		delete data;
	}
	{
		srcPtrTestAlgorithm* data = Analyze("int main() {int var; int * ptr1; int ** ptr2; ptr1 = &var; ptr2 = &ptr1; }");

		assert(data->assignmentRelationships.size() == 0);

		assert(data->pointsToRelationships[0].first.nameofidentifier == "ptr1");
		assert(data->pointsToRelationships[0].second.nameofidentifier == "var");

		assert(data->pointsToRelationships[1].first.nameofidentifier == "ptr2");
		assert(data->pointsToRelationships[1].second.nameofidentifier == "ptr1");

		delete data;
	}
	{
		srcPtrTestAlgorithm* data = Analyze("int main() {   int var ; int            *    ptr1;   int          ** ptr2; ptr1 =    &    var; ptr2   =   &   ptr1; }");

		assert(data->assignmentRelationships.size() == 0);

		assert(data->pointsToRelationships[0].first.nameofidentifier == "ptr1");
		assert(data->pointsToRelationships[0].second.nameofidentifier == "var");

		assert(data->pointsToRelationships[1].first.nameofidentifier == "ptr2");
		assert(data->pointsToRelationships[1].second.nameofidentifier == "ptr1");

		delete data;
	}
	{
		srcPtrTestAlgorithm* data = Analyze("int main() {\nint * x;\nint * y;\ny = x;\n}");

		assert(data->pointsToRelationships.size() == 0);
		assert(data->assignmentRelationships[0].first.nameofidentifier == "y");
		assert(data->assignmentRelationships[0].second.nameofidentifier == "x");

		delete data;
	}
	{
		srcPtrTestAlgorithm* data = Analyze("int f() { return 2; } int main(){int* x; int* y=x;}");
		
		assert(data->pointsToRelationships.size() == 0);
		assert(data->assignmentRelationships[0].first.nameofidentifier == "y");
		assert(data->assignmentRelationships[0].second.nameofidentifier == "x");

		delete data;
	}
	{
		srcPtrTestAlgorithm* data = Analyze("int main(){int var;int x; int* y=&x;}");
		
		assert(data->assignmentRelationships.size() == 0);
		assert(data->pointsToRelationships[0].first.nameofidentifier == "y");
		assert(data->pointsToRelationships[0].second.nameofidentifier == "x");

		delete data;
	}
}

void TestFunctions () {
	{
		srcPtrTestAlgorithm* data = Analyze("void f(int* x) {int y; x = &y;}\n int main() {int*ptr; f(ptr); }");

		assert(data->pointsToRelationships[0].first.nameofidentifier == "x");
		assert(data->pointsToRelationships[0].first.nameoftype == "int");
		assert(data->pointsToRelationships[0].second.nameofidentifier == "y");

		assert(data->assignmentRelationships[0].first.nameofidentifier == "x");
		assert(data->assignmentRelationships[0].second.nameofidentifier == "ptr");
	}
}

int main() {
	TestAssignments();
	TestFunctions();
	std::cout << std::endl << "Finished testing srcPtrPolicy" << std::endl;
	return 0;
}
