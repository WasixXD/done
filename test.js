print("Hello, Javascript!")

const port = Number(input("Qual a porta do servidor? > "))

createHttpServer(port, () => {
    print("Server iniciado")
})