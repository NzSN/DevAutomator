#include "gtest/gtest.h"
#include <fstream>
#include <string>
#include <memory>
#include <filesystem>
#include "utility.hpp"
#include "interpreter.hpp"
#include "testCase.hpp"

namespace fs = std::filesystem;
using std::string, std::ofstream, std::unique_ptr;

string machineDef =
    "boxProperties = [\n"
    "    core.Property('contain', ['10'])\n"
    "]\n\n"
    "boxOpSpec = ["
    "    core.OpSpec('open', [], ('N/A', core.DType))"
    "]\n"
    "class BoxMachine(core.Machine):\n"
    "    def __init__(self) -> None:\n"
    "        core.Machine.__init__(self, \"Box\", boxProperties, boxOpSpec)\n"
    "    @core.Machine.operation(\"BoxMachine\", boxOpSpec)\n"
    "    def open(self) -> core.DNone:\n"
    "       return self.operate(\n"
    "            core.Operation(\n"
    "                \"core\",\n"
    "                \"Box\",\n"
    "                core.opTuple(\"open\", [])\n"
    "            )\n"
    "        )";

class OperInstInterpret_Fixture : public ::testing::Test {
protected:

    void SetUp() override {

        ofstream file;

        // Create Test Directory
        fs::create_directory(testPath);

        // Create DAL TestCase
        file.open(testPath+testCase+".py");

        file <<
            "import DevAuto.Translator.translator as trans\n"
            "import DevAuto.Core as core\n"
            "import DevAuto as DA\n\n";
        file << machineDef + "\n";
        file <<
            "@DA.function(globals())\n"
            "def testCase():\n"
            "    box = BoxMachine()\n"
            "    box.open()\n"
            "    return True\n"
            "\n\n"
            "def main():\n"
            "    tr = trans.Translator()\n"
            "    return tr.trans(testCase)\n";

        file.close();

        itp = std::make_unique<Interpreter>(searchPath);
    }

    void TearDown() override {
        fs::remove_all(testPath);
    }

    string testPath = "/tmp/InterpreterTest/";
    string searchPath = testPath + ":" + "/home/nzsn/WorkSpaces/Codes/DevAutomator/src/lang";
    string testCase = "tc";
    unique_ptr<Interpreter> itp;
};


TEST_F(OperInstInterpret_Fixture, Eval) {
    TestCase tc {"tc", "default", testPath+testCase+".py"};
    InstructionSet insts = itp->interpret(tc);

    // There should only one Instruction
    EXPECT_EQ(insts.size(), 1);

    InstPtr ptr = *insts.begin();
    EXPECT_EQ(ptr->code(), OPER_INST);
}
