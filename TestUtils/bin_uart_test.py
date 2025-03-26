import serial
import threading
import struct

START_FLAG = 42
END_FLAG = 69
ESCAPE_FLAG = 123

streaming_data_enabled = False

def decode_payload(content: bytes):
    global streaming_data_enabled

    # The last byte in content is the transmitted checksum.
    received_checksum = content[-1]
    header_payload = content[:-1]
    # Compute checksum: sum of all bytes in header and payload (mod 256)
    computed_checksum = sum(header_payload) % 256
    if computed_checksum != received_checksum:
        print("Checksum error: received 0x{:02X} but computed 0x{:02X}".format(received_checksum, computed_checksum))
        return

    # Decode the header.
    frame_type_byte = header_payload[0]
    # Extract frame category (top two bits) and frame ID (lower six bits).
    category = (frame_type_byte >> 6) & 0x03
    frame_id = frame_type_byte & 0x3F

    index = 1  # pointer into header_payload after the frame type byte
    seq = None
    if category in (0, 1):
        # Request and reply frames include a sequence number.
        seq = header_payload[index]
        index += 1

    # Now decode based on frame type.
    if category == 0 and frame_id == 1:
        # Robot speed request frame: 3 floats (4 bytes each = 12 bytes).
        if len(header_payload) - index != 12:
            print("Unexpected payload length for speed request.")
            return
        x, y, w = struct.unpack("<fff", bytes(header_payload[index:]))
        print("Robot Speed Request Frame:")
        print("  Sequence Number:", seq)
        # print("  X speed:", x)
        # print("  Y speed:", y)
        # print("  W speed:", w)
        # print the components with .3f precision
        print("  X speed: {:.3f}".format(x))
        print("  Y speed: {:.3f}".format(y))
        print("  W speed: {:.3f}".format(w))
    elif category == 1 and frame_id == 1:
        # Robot speed reply frame: no payload.
        print("Robot Speed Reply Frame:")
        print("  Sequence Number:", seq)
    elif category == 2 and frame_id == 2:
        if not streaming_data_enabled:
            return
        # Robot speed data frame: 3 floats (4 bytes each = 12 bytes).
        if len(header_payload) - index != 12:
            print("Unexpected payload length for speed data.")
            return
        x, y, w = struct.unpack("<fff", bytes(header_payload[index:]))
        print("Robot Speed Data Frame (Streaming):")
        print("  X speed: {:.3f}".format(x))
        print("  Y speed: {:.3f}".format(y))
        print("  W speed: {:.3f}".format(w))
    elif category == 2 and frame_id == 3:
        if not streaming_data_enabled:
            return
        # Robot status data frame: 1 float.
        if len(header_payload) - index != 4:
            print("Unexpected payload length for status data.")
            return
        (battery_voltage,) = struct.unpack("<f", bytes(header_payload[index:]))
        print("Robot Status Data Frame (Streaming):")
        print("  Battery Voltage:", battery_voltage)
    else:
        print("Unknown frame type (Category: {}, ID: {})".format(category, frame_id))

sequence_number = 0

def create_set_speed_payload(x: float, y: float, w: float):
    global sequence_number
    # Frame type for robot speed request: category=0, ID=1 → 0b00_000001 = 0x01.
    frame_type = 0x01
    seq = sequence_number & 0xFF
    sequence_number = (sequence_number + 1) % 256

    # Pack the payload: three doubles in little-endian format.
    payload = struct.pack("<fff", x, y, w)
    # Build header: frame type, sequence number, then payload.
    header_payload = bytes([frame_type, seq]) + payload
    # Compute checksum: sum of header and payload bytes mod 256.
    checksum = sum(header_payload) % 256

    # Build final frame with SOF, header_payload, checksum, and EOF.
    frame = header_payload + bytes([checksum])
    return frame

def encode_frame(payload: bytes) -> bytes:
    encoded = bytearray()
    # Append the SOF at the beginning.
    encoded.append(START_FLAG)
    
    # Process each byte in the payload.
    for b in payload:
        if b == START_FLAG:
            encoded.append(ESCAPE_FLAG)
            encoded.append(1)
        elif b == ESCAPE_FLAG:
            encoded.append(ESCAPE_FLAG)
            encoded.append(2)
        elif b == END_FLAG:
            encoded.append(ESCAPE_FLAG)
            encoded.append(3)
        else:
            encoded.append(b)
    
    # Append the EOF at the end.
    encoded.append(END_FLAG)
    
    return bytes(encoded)


def decode_frame_payload(frame: bytes) -> bytes:
    if not frame or frame[0] != START_FLAG or frame[-1] != END_FLAG:
        raise ValueError("Invalid frame: Missing SOF and/or EOF markers.")
    
    # Remove the SOF and EOF.
    # Note: frame[0] is SOF, frame[-1] is EOF, so payload is in between.
    inner = frame[1:-1]
    decoded = bytearray()
    
    i = 0
    while i < len(inner):
        b = inner[i]
        if b == ESCAPE_FLAG:
            # Ensure there is a following byte.
            if i + 1 >= len(inner):
                raise ValueError("Invalid escape sequence at end of frame.")
            next_byte = inner[i + 1]
            if next_byte == 1:
                decoded.append(START_FLAG)
            elif next_byte == 2:
                decoded.append(ESCAPE_FLAG)
            elif next_byte == 3:
                decoded.append(END_FLAG)
            else:
                raise ValueError(f"Invalid escape code: {next_byte}")
            i += 2  # Skip the escape and its code.
        else:
            decoded.append(b)
            i += 1
    
    return bytes(decoded)


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
                decode_payload(msg)
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
    global streaming_data_enabled

    try:
        ser = serial.Serial(com_port, baud_rate)
        print(f"Opened {com_port} at {baud_rate} baud rate.")
        
        read_thread = threading.Thread(target=read_from_port, args=(ser,))
        read_thread.daemon = True
        read_thread.start()

        while True:
            if streaming_data_enabled:
                user_input = input("Enter s to stop streaming...")
                if user_input == "s":
                    streaming_data_enabled = False
                    print("Streaming stopped.")
                    continue
            else:
                user_input = input("Enter the speeds on all three axes (separated by ,) OR enter s to see the streaming data: ")
                if user_input == "s":
                    streaming_data_enabled = True
                    print("Streaming started.")
                    continue
                sets = user_input.split(",")
                payload = create_set_speed_payload(float(sets[0]), float(sets[1]), float(sets[2]))
                frame = encode_frame(payload)
                ser.write(frame)

    except serial.SerialException as e:
        print(f"Error opening {com_port}: {e}")

if __name__ == "__main__":
    main()