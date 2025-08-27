import serial
import threading
import numpy as np
import matplotlib.pyplot as plt
import time
from matplotlib.widgets import Button

PORT = "COM4"
BAUD = 921600
WIDTH, HEIGHT = 160, 120

FRAME_HEADER = b'\xAA\xBB\xCC\xDD'
FRAME_FOOTER = b'\xDD\xCC\xBB\xAA'

ser = None
running = False
frame_data = bytearray()
lock = threading.Lock()
thread = None
debug = 0

def serial_thread():
    global frame_data, running, ser, count, debug

    buffer = bytearray()
    while running:
        start = time.time()
        if debug:
            print(f"----------------------------------------------------")
            print(f"In Thread serial_thread: It is supposed to read data")
        if ser.in_waiting > 0:
            if debug:
                print(f"There ara data waiting to be read")
            
            chunk = ser.read(1024) 
            #chunk = ser.read(ser.in_waiting)
            buffer.extend(chunk)
            if debug:
                print(f"Size of the read data {len(buffer)}")

            while True:
                if debug:
                    print(f"----->Trying to find the headers")
                start_idx = buffer.find(FRAME_HEADER)

                if start_idx != -1:
                    if debug:
                        print(f"----->Found the starting header at {start_idx}")
                    end_idx = buffer.find(FRAME_FOOTER, start_idx + len(FRAME_HEADER))
                    
                    if (end_idx != -1):#&(end_idx > start_idx):
                        if debug:
                            print(f"----->Found the stopping header at {end_idx}")
                    else:
                        if debug:
                            print(f"----->Did not found the stopping header")

                else:
                    if debug:    
                        print(f"----->Did not found the starting header")
                
                
                if start_idx != -1 and end_idx != -1 and end_idx > start_idx:
                    if debug:    
                        print(f"The data have been read and contains useful infos")
                    data = buffer[start_idx + len(FRAME_HEADER):end_idx]

                    if debug:    
                        print(f"After retriving data, it has a size of{len(data)}")
                    with lock:
                        frame_data = data
                    buffer = buffer[end_idx + len(FRAME_FOOTER):]
                    end = time.time()
                    print(f"Elapsed time: {end-start:.6f} s" )
                else:
                    if debug:    
                        print(f"----->Was not able to treat data")
                    break

def toggle(event):
    global running, thread, ser
    if not running:
        try:
            ser = serial.Serial(PORT, BAUD, timeout=0.1)
        except Exception as e:
            print("Erreur ouverture COM:", e)
            return
        running = True
        thread = threading.Thread(target=serial_thread, daemon=True)
        thread.start()
        btn.label.set_text("Stop")
    else:
        running = False
        btn.label.set_text("Start")
        if thread is not None:
            thread.join()
        if ser is not None:
            ser.close()
            ser = None

plt.ion()
fig, ax = plt.subplots()
img_display = ax.imshow(np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8))
plt.axis("off")
plt.subplots_adjust(bottom=0.2)

axbtn = plt.axes([0.4, 0.05, 0.2, 0.075])
btn = Button(axbtn, "Start")
btn.on_clicked(toggle)

try:
    while plt.fignum_exists(fig.number):
        with lock:
            #print("In the drawing function: About to analyse frame_data")
            #print(f"frame _data is of size: {len(frame_data)}")
            if len(frame_data) > 0:
                # On prend les données reçues
                data = frame_data[:WIDTH*HEIGHT*2]  

                # Si la taille est trop petite, on complète avec des zéros (pixels noirs)
                if len(data) < WIDTH*HEIGHT*2:
                    data = data + bytes(WIDTH*HEIGHT*2 - len(data))

                # Conversion en pixels RGB565
                pixels = np.frombuffer(data, dtype=np.uint16).reshape((HEIGHT, WIDTH))
                r = ((pixels >> 11) & 0x1F) << 3
                g = ((pixels >> 5) & 0x3F) << 2
                b = (pixels & 0x1F) << 3
                rgb = np.dstack((r, g, b)).astype(np.uint8)

                # Rotation de -90°
                rgb = np.rot90(rgb, k=-1)

                img_display.set_data(rgb)
                frame_data = bytearray()

        plt.pause(0.01)
finally:
    running = False
    if thread is not None:
        thread.join()
    if ser is not None:
        ser.close()


        #with lock:
            #if len(frame_data) == WIDTH*HEIGHT*2:  # RGB565
                #pixels = np.frombuffer(frame_data, dtype=np.uint16).reshape((HEIGHT, WIDTH))
                #r = ((pixels >> 11) & 0x1F) << 3
                #g = ((pixels >> 5) & 0x3F) << 2
                #b = (pixels & 0x1F) << 3
                #rgb = np.dstack((r, g, b)).astype(np.uint8)
                #img_display.set_data(rgb)
                #frame_data = bytearray()