#ifndef done_h
#define done_h

#include <v8.h>
#include <libplatform/libplatform.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <uv.h>

uv_loop_t *loop;

// samples/shell.c
// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}


// Don't think this is good. But my C++ isnt that sharp
void on_new_connection(uv_stream_t *server, int status) {
    uv_tcp_t *client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);

    if(uv_accept(server, (uv_stream_t*)client) == 0) {
        printf("Nova conexão: %d\n", status);
    }

    return;
}
int startServer(int port) {
    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in bind_addr;
    uv_ip4_addr("localhost", port, &bind_addr);
    uv_tcp_bind(&server, (struct sockaddr *)&bind_addr, 0);
    int r = uv_listen((uv_stream_t*) &server, 128, on_new_connection);
    printf("Server iniciado\n");
    if(r) {
        printf("Erro ao ouvir\n");
    }
    return uv_run(loop, UV_RUN_DEFAULT);

}

class Done {
    private:
        v8::Isolate *isolate;
        v8::Global<v8::Context> context_;
        std::unique_ptr<v8::Platform> platform;


        static void Print(const v8::FunctionCallbackInfo<v8::Value>& info) {
            v8::HandleScope handle_scope(info.GetIsolate());
            for(int i = 0; i < info.Length(); i++) {
                if(i != 0) printf(" ");
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

            if(!info[0]->IsNumber()) {
                info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(info.GetIsolate(), "Fist parameter needs to be a string or number").ToLocalChecked());
                return;
            }
            // get port 
            auto port = info[0]->Int32Value(info.GetIsolate()->GetCurrentContext()).ToChecked();

            if(!info[1]->IsFunction()) {
                info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(info.GetIsolate(), "Second parameter needs to be a callback").ToLocalChecked());
                return; 
            }
            v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(info[1]);
            int result = startServer(port);
        
            callback->Call(info.GetIsolate()->GetCurrentContext(), v8::Undefined(info.GetIsolate()), 0, nullptr).ToLocalChecked();
        }

        static void Input(const v8::FunctionCallbackInfo<v8::Value>& info) {
            v8::HandleScope handle_scope(info.GetIsolate());
            v8::String::Utf8Value str(info.GetIsolate(), info[0]);
            const char *c = ToCString(str);
            std::string input;
            std::cout << c;
            std::cin >> input;
            auto response = v8::String::NewFromUtf8(info.GetIsolate(), input.c_str(), v8::NewStringType::kNormal).ToLocalChecked(); 
            info.GetReturnValue().Set(response);
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
            loop = uv_default_loop();
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
            global->Set(GetIsolate(), "input", v8::FunctionTemplate::New(GetIsolate(), this->Input));
            
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