# TDOO:
1. ipv4/v6 support
6. epoll is not supported by macOS
7. deal with client closed
8. async send back in echoServer(async in TCPServer?)


# reference:
1. https://www.ibm.com/docs/en/i/7.1?topic=designs-using-poll-instead-select
2. https://stackoverflow.com/questions/2597608/c-socket-connection-timeout

# UML

## Class
![](Class.svg)

<!--

```
@startuml Class
'https://plantuml.com/class-diagram

namespace AccuEnergyTest {
interface IServer
interface IClient

IServer <|.up. TCPServer
IClient <|.up. TCPClient

EchoServer --o IServer
EchoServer --* EchoTask

SimpleMemPool --*PoolMemory

EchoTask --o PoolMemory
}

Server --o EchoServer
Server --o TCPServer
Client --o TCPClient
Server << (E,orchid)>>
Client << (E,orchid)>>

@enduml
```
-->

##  Sequence

![](SeverSequence.svg)
