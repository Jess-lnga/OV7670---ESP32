import serial
import numpy as np
import matplotlib.pyplot as plt

# paramètres
width, height = 160, 120
port = "COM4"   # adapte pour ton PC
baud = 921600

ser = serial.Serial(port, baud, timeout=1)

# figure interactive
plt.ion()
fig, ax = plt.subplots()
img_display = ax.imshow(np.zeros((height, width, 3), dtype=np.uint8))
plt.show()

while True:
    line = ser.readline().decode(errors="ignore").strip()
    
    if line == "FRAME_START":
        # lire les pixels jusqu'à FRAME_END
        frame_data = bytearray()
        while True:
            chunk = ser.read(1024)
            if b"FRAME_END" in chunk:
                # séparer données utiles et balise
                idx = chunk.find(b"FRAME_END")
                frame_data.extend(chunk[:idx])
                break
            else:
                frame_data.extend(chunk)

        # convertir en numpy (RGB565 -> RGB888)
        pixels = np.frombuffer(frame_data, dtype=np.uint16).reshape((height, width))
        
        # conversion RGB565 vers 24 bits
        r = ((pixels >> 11) & 0x1F) << 3
        g = ((pixels >> 5) & 0x3F) << 2
        b = (pixels & 0x1F) << 3
        rgb = np.dstack((r, g, b)).astype(np.uint8)

        # mise à jour image
        img_display.set_data(rgb)
        plt.draw()
        plt.pause(0.001)
