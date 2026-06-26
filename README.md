*This project has been created as part of the 42 curriculum by rroundi, mbelkass, aben-moh.*

# ft_irc

## Description

**ft_irc** is a custom implementation of an Internet Relay Chat (IRC) server written in C++, following the RFC standards of the IRC protocol.

The goal of this project is to build a fully functional IRC server capable of handling multiple clients simultaneously, managing authentication, private messaging, and channel-based communication.

The project is divided into three main components, each handled by a team member:

* **Server Core (aben-moh)**
  Responsible for the server architecture, including:

  * Socket creation and management
  * Handling multiple client connections (poll)
  * Server lifecycle and signal handling
  * Communication between server components

* **Client Commands & Authentication (mbelkass)**
  Responsible for implementing core IRC commands:

  * `PASS`, `NICK`, `USER` (authentication flow)
  * `PRIVMSG` (private messaging)
  * `PING` / `PONG` (connection keep-alive)
  * `QUIT` (client disconnection)

* **Channel Management (rroundi)**
  Responsible for channel related features:

  * `JOIN`, `PART`, `KICK`
  * `TOPIC`, `INVITE`
  * `MODE` (channel modes and permissions)

Together, these components form a modular and scalable IRC server that follows real world networking principles.

---

## Instructions

### Compilation

To compile the project, run:

```bash
make
```

This will generate the executable:

```bash
./ircserv
```

---

### Usage

Run the server with:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 mypassword
```

---

### Connecting to the Server

You can connect using any IRC client such as:

* `irssi`
* `weechat`
* `netcat` (for testing)

Example with `nc`:

```bash
nc -c localhost 6667
```

---

### Authentication Flow

Once connected, the client must authenticate using:

```bash
PASS <password>
NICK <nickname>
USER <username> 0 * :realname
```

---

### Features

* Multiple client handling (non-blocking I/O)
* Authentication system
* Private messaging
* Channel creation and management
* Operator privileges and channel modes
* Clean client disconnection handling

---

## Resources

### Classic References & Documentation

* RFC 1459 - Internet Relay Chat Protocol
* RFC 2812 - IRC Client Protocol
* Beej’s Guide to Network Programming
* C++ Socket Programming tutorials
* poll/select system call documentation (man pages)

---

### AI Usage

AI tools were used during the development of this project for:

* Understanding IRC protocol behavior and edge cases
* Clarifying networking concepts (sockets, poll, non-blocking I/O)
* Debugging specific issues and improving code structure
* Generating explanations for better understanding of complex parts (e.g., signal handling, command parsing)

All core implementation, design decisions, and debugging were carried out by the team members.


