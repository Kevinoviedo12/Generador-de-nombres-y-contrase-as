const net = require('net');
const readline = require('readline');

// Configuración del cliente
const PUERTO = 9090;
const HOST = '127.0.0.1';

// Crear interfaz para leer la entrada del usuario
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

// Crear el cliente TCP
const cliente = new net.Socket();

// Conectar al servidor
cliente.connect(PUERTO, HOST, () => {
    console.log('Conectado al servidor');
    mostrarMenu();
});

// Manejar datos recibidos del servidor
cliente.on('data', (datos) => {
    console.log('Respuesta del servidor: ' + datos.toString());
    mostrarMenu();
});

// Manejar errores
cliente.on('error', (error) => {
    console.error('Error: ' + error.message);
    cliente.end();
});

// Manejar cierre de conexión
cliente.on('close', () => {
    console.log('Conexión cerrada');
    rl.close();
});

// Mostrar menú y manejar opciones del usuario
function mostrarMenu() {
    rl.question('Menú:\n1. Generar nombre de usuario\n2. Generar contraseña\n3. Salir\nElija una opción: ', (opcion) => {
        if (opcion === '3') {
            console.log('Saliendo del programa...');
            cliente.end();
            return;
        }

        if (opcion === '1' || opcion === '2') {
            rl.question('Indique la longitud: ', (longitud) => {
                longitud = parseInt(longitud, 10);

                if (isNaN(longitud) || longitud <= 0) {
                    console.log('Longitud inválida. Debe ser un número positivo.');
                    mostrarMenu();
                    return;
                }

                let solicitud = '';
                if (opcion === '1') {
                    if (longitud < 5 || longitud > 15) {
                        console.log('Longitud inválida para nombre de usuario. Debe estar entre 5 y 15.');
                        mostrarMenu();
                        return;
                    }
                    solicitud = `U${longitud}`;
                } else if (opcion === '2') {
                    if (longitud < 8 || longitud >= 50) {
                        console.log('Longitud inválida para contraseña. Debe ser al menos 8 y menos de 50.');
                        mostrarMenu();
                        return;
                    }
                    solicitud = `P${longitud}`;
                }

                // Enviar solicitud al servidor
                cliente.write(solicitud);
            });
        } else {
            console.log('Opción no válida.');
            mostrarMenu();
        }
    });
}