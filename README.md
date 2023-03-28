# JumpingSquare MainServer
게임 서버-클라이언트 구조를 공부/구현하기 위해 개발한 멀티플레이 점프맵 게임의 서버 프로젝트입니다.

[Game Server Project]
[GitHub: JumpingSquare Client](https://github.com/DecisionDisorder/JumpingSquare "[GitHub: JumpingSquare Client]")  

[Developer]
- 김현종 (guswhd990507@naver.com)

## 프로젝트 소개
### [게임 서버 역할]
- 플레이어들 간의 위치와 회전 정보를 동기화합니다.
- 플레이어의 리스폰 위치를 저장합니다.
- 플레이어의 사망, 리스폰, 클리어를 판정합니다.

### [서버 아키텍처]
<img src="/Image/JumpingSquare_TCP-UDP.png"/>

### [데모 영상]
- https://

## 기술 관련 소개
###  [개발환경 및 기술 스택]
- Visual Studio 2022 & C++ (Visual C++ 2022)
- Windows Socket API
- Rapidjson
- Thread & Mutex
- VCS: GitHub

### [개발 이슈]
**TCP/UDP 병행 통신**
- 처음 서버를 개발할 당시에는 안정적인 TCP를 기반으로 플레이어의 위치와 메시지 모두 한번에 송수신 하는 형태로 개발했습니다.
- 하지만, 응답성이 중요한데 일부 종류의 메시지가 소실되어도 큰 문제가 없는 게임 서버 특성상, UDP가 많이 사용되는 것으로 알고 있기에, UDP를 이용한 개발도 경험해봐야겠다고 생각했습니다.
- 그러나 UDP로 중요한 메시지까지 보내기에는 안정성이 보장되지 못하므로, TCP를 병행하여 사용하기로 하였습니다.
- TCP 송수신과 UDP 송수신을 동시에 수행해야 하기 때문에 TCP 전용, UDP 전용 스레드를 따로 생성하여 독립적으로 돌아가지만, 서로 메시지를 주고받을 창구를 만들어서 유기적인 환경을 만들도록 구현했습니다.
- TCP는 클라이언트로부터 메시지를 주기적으로 받지 않기 때문에 Non-blocking 소켓으로 설정하여 UDP 전용 스레드에서 온 메시지와 TCP로 수신된 메시지가 있는지 확인하는 것을 번갈아 수행하도록 구현했습니다.


**멀티 쓰레드**
- TCP 스레드에서는 UDP로부터 온 (playerName, message) 쌍으로 메시지 Queue를 읽고, UDP 스레드에서는 TCP 스레드로부터 온 (playerName, message) 쌍의 Hash Map(unordered_map)으로 메시지를 읽습니다.
- UDP  스레드에서 Hash Map을 사용하는 이유는 특정 플레이어로부터 수신받은 데이터에 TCP에서 해당 플레이어에게 적용해야하는 데이터를 바로 찾아서 적용하기 위함입니다. 반면에 TCP는 메시지가 쌓인 순서대로 해당 플레이어에게 메시지를 전송해도 되기 때문에 Queue를 선택했습니다.
- 메시지 Queue와 Hash Map은 모두 스레드들 간에 공유되는 공유 변수입니다. 멀티 스레드 환경에서 공유 변수는 동기화 문제가 발생할 수 있기 때문에 mutex로 critical section을 보호해야 하기 때문에 mutex.lock과 unlock을 이용하여 일관성이 깨지지 않도록 구현하였습니다.
