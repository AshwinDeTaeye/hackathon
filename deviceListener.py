from flask import Flask
import serial
from threading import Thread
from queue import Queue

# Configuration
SERIAL_PORT = '/dev/ttyUSB0'  # Update this with your serial port
BAUD_RATE = 9600
app = Flask(__name__)
serial_data_queue = Queue(maxsize=10)  # Store the last 10 lines

def read_from_port(serial_port):
    ser = serial.Serial(serial_port, BAUD_RATE)
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            if serial_data_queue.full():
                serial_data_queue.get()  # Remove the oldest item
            serial_data_queue.put(line)

@app.route('/')
def index():
    all_lines = list(serial_data_queue.queue)
    return {'data': all_lines}

if __name__ == '__main__':
    # Start reading from the serial port in a separate thread
    thread = Thread(target=read_from_port, args=(SERIAL_PORT,))
    thread.daemon = True  # This thread dies when the main thread dies
    thread.start()

    # Start the Flask app
    app.run(debug=True)