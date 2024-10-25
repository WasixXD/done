print("Hello, Javascript!")

// const port = Number(input("Qual a porta do servidor? > "))
const port = 5000

function cb(req, res) {
    const path = req.path;
    const method = req.method;
    print(`Got new req ${method} to path: ${path}`)
    print(path === "/")
    if(path === "/") {
        res.write("Hello, World\n")
    } else if(path === "/about") {
        res.write("/about/\n")
    }
}

createHttpServer(port, () => {
    print("http://localhost:" + port)
}, cb)