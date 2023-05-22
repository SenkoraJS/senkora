#include <cstdio>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <iterator>
#include <js/CharacterEncoding.h>
#include <js/Class.h>
#include <js/CompileOptions.h>
#include <js/Context.h>
#include <js/Modules.h>
#include <js/PropertyAndElement.h>
#include <js/RootingAPI.h>
#include <js/String.h>
#include <js/TypeDecls.h>
#include <js/Utility.h>
#include <js/Value.h>
#include <jsapi.h>
#include <jsfriendapi.h>
#include <js/CompilationAndEvaluation.h>
#include <js/SourceText.h>
#include <js/Conversions.h>
#include <js/Object.h>
#include <mozilla/Utf8.h>
#include <Senkora.hpp>
#include <ostream>
#include <stack>
#include <string>
#include <unistd.h>

#include "boilerplate.hpp"
#include "moduleResolver.hpp"

#define DEBUG(str) printf("%s\n", str);

namespace fs = std::filesystem;

/* Global variables */
std::stack<std::string> dirs;

static bool executeCode(JSContext *ctx, const char* code, const char* fileName) {
    JS::CompileOptions options(ctx);

    JS::RootedObject mod(ctx, Senkora::CompileModule(ctx, fileName, code));
    if (!mod) {
        boilerplate::ReportAndClearException(ctx);
        return false;
    }

    return true;
}

void PrintNonPrivateProperties(JSContext* cx, JSObject *obj) {
    JS::IdVector idprops(cx);
    JS::MutableHandleIdVector props = idprops;
    JS::HandleObject ob(obj);

    js::GetPropertyKeys(cx, ob, JSITER_SYMBOLS, &idprops);
}

static bool print(JSContext* ctx, unsigned argc, JS::Value* vp) {
  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::HandleValue val = args.get(0);
  if (val.isString()) {
      JS::RootedString str(ctx, val.toString());
      JS::UniqueChars chars(JS_EncodeStringToUTF8(ctx, str));
      std::cout << chars.get() << std::endl;
  } else if (val.isNumber()) {
      std::cout << val.toNumber() << std::endl;
  } else if (val.isBoolean()) {
      std::cout << val.toBoolean() << std::endl;
  } else if (val.isNullOrUndefined()) {
      if (val.isNull()) std::cout << "null" << std::endl;
      else std::cout << "undefined" << std::endl;
  } else if (val.isObject()) {
      PrintNonPrivateProperties(ctx, &val.toObject());
  }
  args.rval().setNull();
  return true;
}

static bool run(JSContext *ctx, int argc, const char **argv) {
    if (argc == 1) return false;
    const char *fileName = argv[1];

    JS::RootedObject global(ctx, boilerplate::CreateGlobal(ctx));
    if (!global) return false;

    JSAutoRealm ar(ctx, global);

    JS::SetModuleResolveHook(JS_GetRuntime(ctx), resolveHook);
    JS::SetModuleMetadataHook(JS_GetRuntime(ctx), metadataHook);
    JS::RootedObject privateMod(ctx, JS_NewPlainObject(ctx));
    JS::RootedValue v(ctx);

    v.setObject(*privateMod);
    const JSClass *obj = JS::GetClass(&v.getObjectPayload());

    JS_DefineFunction(ctx, privateMod, "print", &print, 1, 0);

    JS_SetProperty(ctx, global, "__PRIVATE_CUZ_FF_STUPID", v);

    std::string currentPath = fs::current_path();
    std::string filePath = fileName;
    if (fileName[0] != '/') {
        filePath = fs::path(currentPath + "/" + fileName).lexically_normal();
    }
    std::string code = Senkora::readFile(filePath);
    if (code.length() == 0) return false;

    return executeCode(ctx, code.c_str(), filePath.c_str());
}

int main(int argc, const char* argv[]) {
    if (!boilerplate::Run(run, true, argc, argv)) {
        return 1;
    }
    return 0;
}