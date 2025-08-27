import socket
import numpy as np
import matplotlib.pyplot as plt

# Résolution et formats
WIDTH, HEIGHT = 160, 120
FRAME_HEADER = b'\xCD\xAB'   # uint16_t 0xABCD envoyé en little endian
FRAME_FOOTER = b'\xBA\xDC'   # uint16_t 0xDCBA envoyé en little endian

ESP_IP = "10.96.153.146"   # ← IP affichée par l’ESP32
ESP_PORT = 8080

print("Connexion à l'ESP32...")
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((ESP_IP, ESP_PORT))
print("✅ Connecté à l'ESP32")

# Mode interactif pour afficher les images
plt.ion()
fig, ax = plt.subplots()
img_display = ax.imshow(np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8))
plt.axis("off")

buffer = bytearray()

try:
    while True:
        # Lire un bloc de données du socket
        chunk = sock.recv(4096)
        if not chunk:
            print("Connexion fermée par l'ESP32")
            break
        buffer.extend(chunk)

        # Tant qu'il y a une frame complète dans le buffer
        while True:
            start_idx = buffer.find(FRAME_HEADER)
            if start_idx == -1:
                break

            end_idx = buffer.find(FRAME_FOOTER, start_idx + len(FRAME_HEADER))
            if end_idx == -1:
                break

            # Extraire la frame brute
            data = buffer[start_idx + len(FRAME_HEADER):end_idx]

            if len(data) == WIDTH * HEIGHT * 2:
                # Conversion RGB565 → RGB888
                pixels = np.frombuffer(data, dtype=np.uint16).reshape((HEIGHT, WIDTH))
                r = ((pixels >> 11) & 0x1F) << 3
                g = ((pixels >> 5) & 0x3F) << 2
                b = (pixels & 0x1F) << 3
                rgb = np.dstack((r, g, b)).astype(np.uint8)

                # Rotation de -90° si nécessaire
                rgb = np.rot90(rgb, k=-1)

                img_display.set_data(rgb)
                plt.pause(0.001)
            else:
                print(f"⚠️ Taille incorrecte: {len(data)} (attendu {WIDTH*HEIGHT*2})")

            # Retirer la frame du buffer
            buffer = buffer[end_idx + len(FRAME_FOOTER):]

except KeyboardInterrupt:
    print("Arrêt manuel")
finally:
    sock.close()
    plt.ioff()
    plt.show()
