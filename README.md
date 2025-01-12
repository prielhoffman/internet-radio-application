# Internet Radio Application

## Overview
The **Internet Radio Application** is a custom implementation of a radio streaming system using **TCP** and **UDP** protocols. The application enables streaming of audio over a **multicast** network, allowing users to switch between different radio stations and upload new songs.

This project demonstrates advanced use of **socket programming**, **multithreaded programming**, and the implementation of a **server-client protocol** to facilitate communication between a client and a server for streaming radio.

## Features
- **Multithreaded Programming**: Efficiently handles multiple connections with both **TCP** and **UDP** sockets.
- **Protocol Implementation**: A custom server-client protocol ensures smooth communication between the client and the server.
- **Audio Streaming**: Streams audio over **UDP** multicast groups to enable radio functionality.
- **Station Switching**: Users can change between different multicast streams (radio stations).
- **Song Upload**: Users can upload new songs to the server to be added to the station’s playlist.

## Client
The client is responsible for interacting with the server to stream music and manage stations. It features:
- **Two Sockets**: One for **TCP** communication and one for **UDP** audio streaming.
- **User Interface**: A simple interface for users to interact with the server, select songs, change stations, and upload new songs.
- **Handshake**: The client establishes a connection with the server and retrieves the multicast address for audio streaming.
- **Audio Playback**: Receives audio packets from the server and plays music to the user.

## Server
The server manages multiple client connections and facilitates radio streaming. It includes:
- **Multiple Sockets**: The server handles many **TCP** connections for control and **UDP** streams for broadcasting audio to clients.
- **Station Management**: It supports multiple radio stations, each transmitted via a separate multicast group.
- **Stateful Server**: The server maintains the state of each client and ensures smooth communication through the server-client protocol.
- **Notifications**: The server notifies clients about new stations and song changes.

## Protocol
The server and client communicate using a simple, custom protocol. The messages exchanged follow a specific format to ensure synchronization and error-free communication.

### Example Messages
- **Hello**: Initializes the connection.
- **Welcome**: The server responds to the client with available stations and multicast details.
- **AskSong**: The client requests a song.
- **Announce**: The server announces a new song or station.

### Error Handling
The system handles various errors, such as invalid commands or timeouts. In case of errors, the server or client will disconnect, ensuring the integrity of the system.

## Finite State Machine (FSM)
The system employs a **Finite State Machine (FSM)** for both the client and server to manage the different states in communication.

### Client States
- **Off**: The default state.
- **WelcomeWait**: The client is waiting for the server's response.
- **Established**: The connection is established, and the client is actively interacting with the server.
- **NewStations**: The client is notified about new stations available.

### Server States
- **Connected**: The server is connected to a client and actively transmitting data.
- **Disconnected**: The server is not connected to any clients.
- **Streaming**: The server is actively streaming audio to clients.

### Example Client State Machine
```c
int handleMsgFromServer(int currState, char* msg) {
    int newState;
    switch(currState) {
        case OFF: newState = offStateMsgHandler(msg); break;
        case WelcomeWait: newState = WelcomeWaitMsgHandler(msg); break;
        case Established: newState = EstablishedMsgHandler(msg); break;
        default: {terminate(); newState = OFF;}
    }
    return newState;
}
```

### State Transition for Welcome Message
```c
int WelcomeWaitMsgHandler(char* msg) {
    int newState, msgType = msg[0];
    switch (msgType) {
        case WELCOME: { handleWelcome(msg); newState = ESTABLISHED; } break;
        default: { newState = OFF; } break;
    }
    return newState;
}
```

### Setup Instructions
1. Clone the repository to your local machine.
2. Build the client and server programs in your preferred language (C, C++, or Python).
3. Set up a multicast network or use GNS3 to simulate the network environment.
4. Run the server and start the client.
5. Use the client interface to interact with the radio stations and upload songs.

### Requirements
* GNS3 for multicast network simulation.
* Python, C, or C++ for building the server and client applications.
* Access to audio playback hardware for the streaming functionality.
