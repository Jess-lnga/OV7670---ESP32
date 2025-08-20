import serial
import numpy as np
import matplotlib.pyplot as plt

# ----------------------------
# CONFIG SERIAL
# ----------------------------
SERIAL_PORT = 'COM4'  # ou '/dev/ttyUSB0' selon ton OS
BAUD_RATE = 921600 #115200
WIDTH = 160
HEIGHT = 120
FRAME_SIZE = WIDTH * HEIGHT * 2  # 2 octets par pixel (RGB565)

# ----------------------------
# FONCTIONS
# ----------------------------
def rgb565_to_rgb888(frame_bytes):
    """
    Convertit un tableau RGB565 (2 octets par pixel) en tableau RGB888 (3 octets par pixel)
    """
    frame_rgb = np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8)
    for y in range(HEIGHT):
        for x in range(WIDTH):
            idx = (y * WIDTH + x) * 2
            pixel = (frame_bytes[idx] << 8) | frame_bytes[idx + 1]
            
            r = ((pixel >> 11) & 0x1F) << 3   # 5 bits → 8 bits
            g = ((pixel >> 5) & 0x3F) << 2    # 6 bits → 8 bits
            b = (pixel & 0x1F) << 3           # 5 bits → 8 bits
            
            frame_rgb[y, x] = [r, g, b]
    return frame_rgb

# ----------------------------
# MAIN
# ----------------------------
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

plt.ion()  # mode interactif pour mise à jour en temps réel
fig, ax = plt.subplots()
img_plot = ax.imshow(np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8))
plt.title("Image ESP32 OV7670")

while True:
    # Lire une frame complète
    frame_bytes = ser.read(FRAME_SIZE)
    if len(frame_bytes) != FRAME_SIZE:
        continue  # attendre la frame complète

    # Convertir en RGB888
    frame_rgb = rgb565_to_rgb888(frame_bytes)

    # Mettre à jour le plot
    img_plot.set_data(frame_rgb)
    plt.draw()
    plt.pause(0.001)
