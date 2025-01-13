import serial
import threading

START_FLAG = 42
END_FLAG = 69
ESCAPE_FLAG = 123

def read_from_port(ser):
    msg = []
    frameIsOpen = False
    escapeIsActive = False

    while True:
        byte = ser.read(1)[0]

        if byte == START_FLAG:
            frameIsOpen = True
            escapeIsActive = False
            msg = []
        elif frameIsOpen:
            if byte == END_FLAG:
                frameIsOpen = False
                print("\nData received: " + " ".join([str(b) for b in msg]))
            elif byte == ESCAPE_FLAG:
                if(escapeIsActive):
                    print("Error: Double escape flag")
                escapeIsActive = True
            elif escapeIsActive:
                if byte == 1:
                    msg.append(START_FLAG)
                elif byte == 2:
                    msg.append(ESCAPE_FLAG)
                elif byte == 3:
                    msg.append(END_FLAG)
                else:
                    print("Error: Invalid escape flag")
                escapeIsActive = False
            else:
                msg.append(byte)


def main():
    com_port = input("Enter COM port (e.g., COM3): ")
    baud_rate = 115200

    try:
        ser = serial.Serial(com_port, baud_rate)
        print(f"Opened {com_port} at {baud_rate} baud rate.")
        
        read_thread = threading.Thread(target=read_from_port, args=(ser,))
        read_thread.daemon = True
        read_thread.start()

        while True:
            user_input = input("Enter numbers separated by spaces (and sets separated by |): ")
            sets = user_input.split("|")

            for set in sets:
                numbers = [int(num) for num in set.split()]
                frame = [START_FLAG]
                for num in numbers:
                    if num == START_FLAG:
                        frame.extend([ESCAPE_FLAG, 1])
                    elif num == ESCAPE_FLAG:
                        frame.extend([ESCAPE_FLAG, 2])
                    elif num == END_FLAG:
                        frame.extend([ESCAPE_FLAG, 3])
                    else:
                        frame.append(num)
                frame.append(END_FLAG)

                ser.write(bytearray(frame))

    except serial.SerialException as e:
        print(f"Error opening {com_port}: {e}")

if __name__ == "__main__":
    main()