import time
import serial
import json
import serial.tools.list_ports
import random
import signal



# Liste des couleurs possibles que l'on va tirer au sort pour chaque alcôve
availableColors = [
    "#ff0000",
    "#00ff00",
    "#0000ff"
]

# Recherche des ports série disponibles
serialPort = None
ports = serial.tools.list_ports.comports()
for port in ports:    
    if 'IOUSBHostDevice' in port.description or 'CP2102' in port.description:
        serialPort = port.device
        print('Port série de l\'Arduino trouvé :', port.device)

if serialPort is None:
    print('Aucun port série Arduino trouvé')
    exit(1)

# Si l'Arduino est trouvé, on peut envoyer des données json
arduino = serial.Serial(serialPort, 9600)
arduino.reset_input_buffer()

# Lire les données du port série
response = arduino.readline().decode("latin-1")
print("Response from Arduino: ", response)


def keyboard_interrupt_handler(signal, frame):
    print("Keyboard interrupt detected. Closing Arduino connection...")
    arduino.close()
    exit(0)

# Register the signal handler
signal.signal(signal.SIGINT, keyboard_interrupt_handler)

while True:
    try:
        # Tirage au sort de 6 couleurs aléatoires dans availableColors
        colors = []
        for i in range(0, 6):
            colors.append(random.choice(availableColors))

        # Créer le dictionnaire JSON
        data = {
            "alcove1": {"colors": [colors[0], colors[1]]},
            "alcove2": {"colors": [colors[2], colors[3]]},
            "alcove3": {"colors": [colors[4], colors[5]]},
        }

        # Convertir le dictionnaire en JSON
        json_data = json.dumps(data)

        # Envoyer le JSON à l'Arduino
        arduino.write(json_data.encode())        

        # Lire les données du port série
        response = arduino.readline().decode("latin-1")
        print("Response from Arduino: ", response)

        # Attendre 5 secondes
        time.sleep(5)

    except KeyboardInterrupt:
        keyboard_interrupt_handler(signal.SIGINT, None)
