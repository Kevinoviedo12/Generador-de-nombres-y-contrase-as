const net = require('net');
const readline = require('readline');

// Configuración del cliente
const PORT = 9090;
const HOST = '127.0.0.1';

// Crear interfaz para leer la entrada del usuario
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

// Crear el cliente TCP
const client = new net.Socket();

// Conectar al servidor
client.connect(PORT, HOST, () => {
    console.log('Conectado al servidor');
    showMenu();
});

// Manejar datos recibidos del servidor
client.on('data', (data) => {
    console.log('Respuesta del servidor: ' + data.toString());
    showMenu();
});

// Manejar errores
client.on('error', (err) => {
    console.error('Error: ' + err.message);
    client.end();
});

// Manejar cierre de conexión
client.on('close', () => {
    console.log('Conexión cerrada');
    rl.close();
});

// Mostrar menú y manejar opciones del usuario
function showMenu() {
    rl.question('Menú:\n1. Generar nombre de usuario\n2. Generar contraseña\n3. Salir\nElija una opción: ', (option) => {
        if (option === '3') {
            console.log('Saliendo del programa...');
            client.end();
            return;
        }

        if (option === '1' || option === '2') {
            rl.question('Indique la longitud: ', (length) => {
                length = parseInt(length, 10);

                if (isNaN(length) || length <= 0) {
                    console.log('Longitud inválida. Debe ser un número positivo.');
                    showMenu();
                    return;
                }

                let request = '';
                if (option === '1') {
                    if (length < 5 || length > 15) {
                        console.log('Longitud inválida para nombre de usuario. Debe estar entre 5 y 15.');
                        showMenu();
                        return;
                    }
                    request = `U${length}`;
                } else if (option === '2') {
                    if (length < 8 || length >= 50) {
                        console.log('Longitud inválida para contraseña. Debe ser al menos 8 y menos de 50.');
                        showMenu();
                        return;
                    }
                    request = `P${length}`;
                }

                // Enviar solicitud al servidor
                client.write(request);
            });
        } else {
            console.log('Opción no válida.');
            showMenu();
        }
    });
}
