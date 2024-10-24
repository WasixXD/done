#ifndef done_h
#define done_h

#include <v8.h>
#include <libplatform/libplatform.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Done {
    private:
        v8::Isolate *isolate;
        v8::Global<v8::Context> context_;
        std::unique_ptr<v8::Platform> platform;
    public: 
        Done() {
            this->platform = v8::platform::NewDefaultPlatform();
            v8::V8::InitializePlatform(this->platform.get());
            v8::V8::Initialize();

            v8::Isolate::CreateParams createParams;
            createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

            this->isolate = v8::Isolate::New(createParams);
        }

        v8::Isolate *GetIsolate() {
            return this->isolate;
        }


        void execute_script(const char* filename) {
            v8::Isolate::Scope isolate_scope(GetIsolate());
            v8::HandleScope handle_scope(GetIsolate());

            v8::Local<v8::Context> context = v8::Context::New(GetIsolate());
            this->context_.Reset(GetIsolate(), context);

            v8::Context::Scope context_scope(context);

            v8::Local<v8::String> source = v8::String::NewFromUtf8Literal(GetIsolate(), "'Hello' + ', World!'");
            
            // Compile the source code.
            v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();

            // Run the script to get the result.
            v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

            // Convert the result to an UTF8 string and print it.
            v8::String::Utf8Value utf8(GetIsolate(), result);
            printf("%s\n", *utf8);
        }
};
#endif