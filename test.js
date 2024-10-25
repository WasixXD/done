print("Hello, Javascript!")

// const port = Number(input("Qual a porta do servidor? > "))
const port = 5000

function cb(req, res) {
    const path = req.path;
    const method = req.method;

    print(`Nova requisição ${method} para o caminho ${path}`, path === "/")
}

createHttpServer(port, () => {
    print("http://localhost:" + port)
}, cb)