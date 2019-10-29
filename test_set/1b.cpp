#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <unistd.h>
#include <getopt.h>


#include "llvm/Support/Host.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Parse/Parser.h"
#include "clang/Lex/Token.h"

using namespace std;

using clang::CompilerInstance;
using clang::TargetOptions;
using clang::TargetInfo;
using clang::FileEntry;
using clang::Token;
using clang::Parser;
using clang::SourceLocation;
using clang::DiagnosticOptions;
using clang::TextDiagnosticPrinter;

static int DEBUG_OPT = 0;
static int HELP_OPT = 0;
static int MIN_MATCH = 9;
static shared_ptr<CompilerInstance> ci1;
static shared_ptr<CompilerInstance> ci2;

class Student {
	char *name;
	int sid;
};

class Match {
    public:
	  int index1, index2, length;
      Match(int a, int b, int l) { index1 = a; index2 = b; length = l; }
	  bool operator==(const Match &m) {
		return this->index1 == m.index1 && this->index2 == m.index2 && this->length == m.length;
	  }
      bool operator<(const Match &m) {
		return this->index1 < m.index1 || (this->index1 == m.index1 && this->length < m.length);
	  }
};

vector<Token> getTokensFromFile(string &file_name, int id) {
    shared_ptr<CompilerInstance> ci(new CompilerInstance);
    unique_ptr<DiagnosticOptions> diagnosticOptions(new DiagnosticOptions);
    ci->createDiagnostics();

    std::shared_ptr<clang::TargetOptions> pto = std::make_shared<clang::TargetOptions>();
    pto->Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *pti = TargetInfo::CreateTargetInfo(ci->getDiagnostics(), pto);
    ci->setTarget(pti);
	ci->getHeaderSearchOpts().AddPath("/usr/include/c++/8", clang::frontend::Angled, false, false);
	ci->getHeaderSearchOpts().AddPath("/usr/include/x86_64-linux-gnu/c++/8", clang::frontend::Angled, false, false);
    ci->getHeaderSearchOpts().AddPath("/usr/local/include", clang::frontend::Angled, false, false);
	ci->getHeaderSearchOpts().AddPath("/usr/lib/llvm-6.0/lib/clang/6.0.0/include", clang::frontend::Angled, false, false);
	ci->getHeaderSearchOpts().AddPath("/usr/include/x86_64-linux-gnu", clang::frontend::Angled, false, false);
	ci->getHeaderSearchOpts().AddPath("/include", clang::frontend::Angled, false, false);
	ci->getHeaderSearchOpts().AddPath("/usr/include", clang::frontend::Angled, false, false);
	ci->getHeaderSearchOpts().AddPath("/usr/include/llvm-6.0", clang::frontend::Angled, false, false);
	ci->getHeaderSearchOpts().AddPath("/usr/lib/llvm-6.0/include", clang::frontend::Angled, false, false);

    ci->createFileManager();
    ci->createSourceManager(ci->getFileManager());
    ci->getLangOpts().CPlusPlus = true;
    ci->getLangOpts().CPlusPlus11 = true;
    ci->createPreprocessor(clang::TU_Complete);

    const FileEntry *pFile = ci->getFileManager().getFile(file_name.c_str());
    ci->getSourceManager().setMainFileID(ci->getSourceManager().createFileID( pFile, clang::SourceLocation(), clang::SrcMgr::C_User));
    ci->getDiagnosticClient().BeginSourceFile(ci->getLangOpts(), &ci->getPreprocessor());
    ci->getPreprocessor().EnterMainSourceFile();
    vector<Token> tokens;
	do {
	  Token token;
	  ci->getPreprocessor().Lex(token);
	  SourceLocation loc = token.getLocation();
	  auto mainFileID = ci->getSourceManager().getMainFileID();
	  auto curFileID = ci->getSourceManager().getFileID(loc);
	  if (mainFileID == curFileID)
        tokens.push_back(token);
    } while (tokens.size() == 0 || tokens[tokens.size() - 1].isNot(clang::tok::eof));
    tokens.pop_back();
	ci->getDiagnosticClient().EndSourceFile();
	if (id == 1) ci1 = ci; else ci2 = ci;
    return tokens;
}

void help() {
	printf("[usage: codesim [-v|--verbose] [-h|--help] code1 code2\n");
}

pair<string, string> parseCommand(int argc, char *argv[]) {
    int ch;
	int option_index = 0;
	static struct option long_options[] = {
	  {"help", no_argument, NULL, 'h'},
	  {"verbose", no_argument, NULL, 'v'}
	};
	pair<string, string> filePair;
    while ((ch = getopt_long(argc, argv, "hv", long_options, &option_index)) != -1) {
	  switch (ch) {
		case 'h': help(); HELP_OPT = 1; break;
		case 'v': DEBUG_OPT = 1; break;
		default: printf("Unknown option: %c\n",(char)optopt); break;
	  }
	}
    if (argc == optind + 2) {
	  filePair.first = string(argv[optind]);
	  filePair.second = string(argv[optind + 1]);
	}
	return filePair;
}

template <class T>
int length(T& arr) {
	return sizeof(arr) / sizeof(arr[0]);
}

int main(int argc, char *argv[]) {
    pair<string, string> filePair;
	filePair = parseCommand(argc, argv);
    if (filePair.first.empty() || filePair.second.empty()) {
	  if (HELP_OPT) return 0;
	  help(); return 1;
	}
	vector<Token> tokens1 = getTokensFromFile(filePair.first, 1);
	vector<Token> tokens2 = getTokensFromFile(filePair.second, 2);
    return 0;
}
