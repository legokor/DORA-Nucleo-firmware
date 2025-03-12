# DORA Nucleo <--> Jetson UART communication

## Generic communication description
The UART communication is completely in binary. It uses a server-client topology, where the Nucleo is the server.
The Jetson might send some requests to the Nucleo, which will respond with a corresponding answer communication frame. Further more, the Nucleo also has
streaming functionality, which will stream data at a predefined frequency if enabled. All data is transmitted (i.e. doubles) in little-endian form.

## Frame format
Each frame starts with a start-of-frame (`SOF = 10'42`) character, after which there is a frame type byte. The this byte contains the following bits: **2'CCDDDDDD**, where **CC** marks the frame category (0 - request, 1 - reply, 2 - stream), and **DDDDDD** marks the ID of this frame type. In case of the _request_ and _reply_ categories, another byte follows which marks the sequence number of the request and the reply respectively.

Following this header there will be some data bytes. After these data bytes the next to last byte is the checksum byte, which contains the sum product of all bytes in the frame (excluding the `SOF` and `EOF` characters). Finally, the last byte in the frame is the end-of-frame (`EOF = 10'69`) character, which marks the end of the given frame.

### Escape character
The `SOF` and `EOF` characters must not appear anywhere else in the transmitted frame, so they are always "escaped" using the escape character (`ESC = 10'123`).
If the encoded character is the following:
- `SOF` -> `ESC` and a `1` is transmitted
- `ESC` -> `ESC` and a `2` is transmitted
- `EOF` -> `ESC` and a `3` is transmitted

## Frame types
The following are the currently supported frame types.

### Robot speed request frame (2'00000001):
This frame contains the movement commands for the robot. The payload consists of 3 floats: the `X`, `Y` and `W` movement speed (in this order).
- The reply has a frame type byte of **2'01000001** and no payload.

### Robot speed data frame (2'10000010):
This is a frame that the Nucleo streams at a certain frequency, that contains the currently measured speed of the robot in the form of 3 floats: `X`, `Y`
and `W`.
- As this is a stream frame, there is no reply.

**Dev info:** **_Currently the Nucleo streams this data by default without the need to enable it_**
