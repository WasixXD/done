# Done
# Toy Javascript Runtime (Nodejs Clone)
![image](https://github.com/user-attachments/assets/a593bc9d-4061-4fd7-b14d-9d8a45e29e10)

**Definitely Not Deno**

# Brief 📖
This is probably my biggest project ever. Ever since I started using Node.js, I dreamed of creating something like this—a JavaScript runtime. It sounded easy, but oh boy, this past week has been the most intense I’ve had in a while. 
The V8 documentation is pretty scarce, and my C++ skills are rusty. However, I didn’t give up, and here we are. I hope that anyone who tries to do this in the future can benefit from my mistakes.


# Challenges 🐢
- Compile V8 
- Find useful information about it on the internet 
- Use V8 magical syntax 
- Be able to make V8 and Libuv communicate
- C++

# Goals 🏆
[ x ] Runtime groups both V8 and Libuv \
[ x ] Runtime can read and execute JS files \
[ x ] Custom embed functions \
[ x ] Uses libuv to set up a simple http server \


# How it works? 💼
This is tricky. \
The main purpose of this project was to better understand the Node.js runtime, but it kind of confused me more. I’ll do my best to explain.
First, we need to initialize V8, which is pretty simple, but be aware that you'll need a unique pointer to keep it running; once it goes out of scope, it will stop. 
Then, you have to set up additional scopes because the engine has a garbage collector that works independently, so it needs to know which objects it can free to run more tasks. 
This requires an isolate scope (the isolate is an instance that runs once for each platform, basically the main of the engine), followed by a HandleScope (which acts as a container for everything that comes after). 
Finally, you need a context scope (the scope in which your script will run). Only after that can you create a new context 
that can embed custom functions. From here, it's pretty straightforward; you can Google how to add functions to the runtime and call them from your JavaScript file.


# How to install 🚀

Make sure to have at least `git` and run this from a linux environment

```
git clone https://github.com/WasixXD/done
cd done
chmod +x setup.sh
./setup.sh
```

The `setup.sh` script installs all the tooling for downloading and compiling V8 from source. \

Then you can

```
make
./done <your_script>.js
```

# ▶️ DEMO
**Remember that this is a toy runtime, don't expect much from it**

```js
// input let you receive a string from the user
const port = Number(input("Specify the port > "))

function cb(req, res) {
    // req and res are both custom objects that have only those params and methods
    const path = req.path;
    const method = req.method;
    print(`Got new req ${method} to path: ${path}`)
    if(path === "/") {
        res.write("Hello, World\n")
    } else if(path === "/about") {
        res.write("/about\n")
    } else {
        res.write("undefined route\n");
    }
}

// createHttpServer let you run a simple TCP server that have some tooling to answer HTTP requests
createHttpServer(port, () => {  print("http://localhost:" + port)  }, cb)
```

PS: Nodejs killer
