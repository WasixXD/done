#ifndef done_h
#define done_h

#include <v8.h>
#include <libplatform/libplatform.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

// samples/shell.c
// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

class Done {
    private:
        v8::Isolate *isolate;
        v8::Global<v8::Context> context_;
        std::unique_ptr<v8::Platform> platform;

        static void Print(const v8::FunctionCallbackInfo<v8::Value>& info) {
            v8::HandleScope handle_scope(info.GetIsolate());
            for(int i = 0; i < info.Length(); i++) {
                v8::String::Utf8Value str(info.GetIsolate(), info[i]);
                const char *c = ToCString(str);
                printf("%s", c);
            }
            printf("\n");
            fflush(stdout);
        }
        static void httpServer(const v8::FunctionCallbackInfo<v8::Value>& info) {
            v8::HandleScope handle_scope(info.GetIsolate());
            if(info.Length() < 2) {
                info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(info.GetIsolate(), "This function expects two parameters").ToLocalChecked());
                return;
            }

            if(!info[0]->IsString() || info[0]->IsNumber()) {
                info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(info.GetIsolate(), "Fist parameter needs to be a string or number").ToLocalChecked());
                return;
            }
            // get port 
            v8::String::Utf8Value arg0(info.GetIsolate(), info[0]);
            const char *port = ToCString(arg0);

            if(!info[1]->IsFunction()) {
                info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(info.GetIsolate(), "Second parameter needs to be a callback").ToLocalChecked());
                return; 
            }

            v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(info[1]);
            callback->Call(info.GetIsolate()->GetCurrentContext(), v8::Undefined(info.GetIsolate()), 0, nullptr).ToLocalChecked();
        }


        v8::MaybeLocal<v8::String> readFile(const char* file_path) {

            std::ifstream file(file_path);

            if(!file.is_open()) { 
                std::__throw_runtime_error("ERROR: file does not exists\n");
                return v8::MaybeLocal<v8::String>();
            }

            // not the best
            std::string text;
            std::ostringstream s;
            s << file.rdbuf();
            text = s.str();

            file.close();
            return v8::MaybeLocal<v8::String>(v8::String::NewFromUtf8(this->isolate, text.c_str()));
        }

    public: 
        Done() {
            // doing this so the platform don't die after exiting the scope
            this->platform = v8::platform::NewDefaultPlatform();
            v8::V8::InitializePlatform(this->platform.get());
            v8::V8::Initialize();

            v8::Isolate::CreateParams createParams;
            createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

            // create the unique Isolate instance
            this->isolate = v8::Isolate::New(createParams);
        }

        v8::Isolate *GetIsolate() {
            return this->isolate;
        }

        void execute_script(const char* filename) {
            // Add one scope for the isolate
            v8::Isolate::Scope isolate_scope(GetIsolate());
            // One scope for this scope
            v8::HandleScope handle_scope(GetIsolate());

            // Set up a global interface that can have embed functions
            v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(GetIsolate());
            global->Set(GetIsolate(), "print", v8::FunctionTemplate::New(GetIsolate(), this->Print));
            global->Set(GetIsolate(), "createHttpServer", v8::FunctionTemplate::New(GetIsolate(), this->httpServer));
            
            // Gen a new context (new script -> new context) and change the global one
            v8::Local<v8::Context> context = v8::Context::New(GetIsolate(), nullptr, global);
            this->context_.Reset(GetIsolate(), context);

            // Scope for this context
            v8::Context::Scope context_scope(context);
            v8::Local<v8::String> source = this->readFile(filename).ToLocalChecked();
            
            v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();

            v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
        }
};
#endif