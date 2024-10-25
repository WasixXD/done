print("Hello, Javascript!")

const port = Number(input("Qual a porta do servidor? > "))

function cb(req, res) {
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

createHttpServer(port, () => { print("http://localhost:" + port) }, cb)