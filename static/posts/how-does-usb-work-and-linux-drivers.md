# USB 2.0 and 1.1

USB as its name would suggest is a serial bus. It uses 4 shielded wires of which two are power (+5v & GND). The remaining two are twisted pair differential data signals. It uses a NRZI (Non Return to Zero Invert) encoding scheme to send data with a sync field to synchronise the host and receiver clocks.

## Connectors

All devices have an upstream connection to the host and all hosts have a downstream connection to the device. Upstream and downstream connectors are not mechanically interchangeable, thus eliminating illegal loopback connections at hubs such as a downstream port connected to a downstream port. There are commonly two types of connectors, called type A and type B which are shown below.

**Type A USB Connector**

![contypea.gif](USB%202%200%20and%201%201%20e47a8c8a104a43c9a2eed1fb75fa0a55/contypea.gif)

**Type B USB Connector**

![Untitled](USB%202%200%20and%201%201%20e47a8c8a104a43c9a2eed1fb75fa0a55/Untitled.png)

| Pin Number | Cable Colour | Function |
| --- | --- | --- |
| 1 | Red | VBUS (5 volts) |
| 2 | White | D- |
| 3 | Green | D+ |
| 4 | Black | Ground |

Type A plugs always face upstream. Type A sockets will typically find themselves on hosts and hubs. For example type A sockets are common on computer main boards and hubs. Type B plugs are always connected downstream and consequently type B sockets are found on devices.

## **Speed Identification**

A USB device must indicate its speed by pulling either the D+ or D- line high to 3.3 volts. A full speed device, pictured below will use a pull up resistor attached to D+ to specify itself as a full speed device.

**Note**: Without a pull up resistor, USB assumes there is nothing connected to the bus. Some devices have this resistor built into its silicon, which can be turned on and off under firmware control, others require an external resistor.

![Untitled](USB%202%200%20and%201%201%20e47a8c8a104a43c9a2eed1fb75fa0a55/Untitled%201.png)

Low Speed devices will use a pull up resistor attached to D- to specify itself as a Low Speed Device 😇

![Untitled](USB%202%200%20and%201%201%20e47a8c8a104a43c9a2eed1fb75fa0a55/Untitled%202.png)