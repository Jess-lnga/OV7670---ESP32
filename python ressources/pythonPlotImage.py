import serial
import numpy as np
import matplotlib.pyplot as plt

# paramètres
width, height = 160, 120
frame_size = width * height * 2   # RGB565 = 2 octets par pixel
port = "COM4"
baud = 921600

ser = serial.Serial(port, baud, timeout=2)

# figure interactive
plt.ion()
fig, ax = plt.subplots()
img_display = ax.imshow(np.zeros((height, width, 3), dtype=np.uint8))
plt.show()

def read_frame():
    """Attend FRAME_START, lit exactement frame_size octets, puis attend FRAME_END"""
    # attendre FRAME_START
    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if line == "FRAME_START":
            break

    # lire les données binaires de l'image
    frame_data = bytearray()
    while len(frame_data) < frame_size:
        chunk = ser.read(frame_size - len(frame_data))
        if not chunk:
            raise TimeoutError("Timeout en lisant l'image")
        frame_data.extend(chunk)

    # attendre FRAME_END
    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if line == "FRAME_END":
            break

    return bytes(frame_data)

while True:
    try:
        frame_data = read_frame()

        # convertir en numpy (RGB565 -> RGB888)
        pixels = np.frombuffer(frame_data, dtype=np.uint16).reshape((height, width))
        r = ((pixels >> 11) & 0x1F) << 3
        g = ((pixels >> 5) & 0x3F) << 2
        b = (pixels & 0x1F) << 3
        rgb = np.dstack((r, g, b)).astype(np.uint8)

        # mise à jour de l’image
        img_display.set_data(rgb)
        fig.canvas.flush_events()
        plt.pause(0.001)

    except Exception as e:
        print("Erreur:", e)
