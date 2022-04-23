// 서버는 아이피를 가지고 시작
// 아이피로 접속
// 외부에서 접속시 퍼블릭 아이피가 필요하지만
// 내부 공유기한테 서버를 실행시 내부 개인 아이피가 필요함.

// 내부 아이피를 정의
#define SERVER_PRIVATE_IP "172.31.41.33"

// 컴퓨터에는 여러개의 프로그램이 작동
// 네트워크 사용시
// 각각 다른 게임 켰을 시 각 서버연결된 메세지를 포트로 구분함 

// 49152 ~ 65535 자유롭게 사용가능
#define SERVER_PORT 49153

// 서버에서는 메세지를 보낼것입니다
// 메세지 갯수는 제한
// 네트워크 물리계층의 한계 존재

// 최대 버퍼 사이즈 조정
#define MAX_BUFFER_SIZE 1024

// 동시접속 가능한 유저 수 제한이 필요함
#define MAX_USER_NUMBER 100

#include <iostream>

// 지금 윈도우에선 안나옴 (리눅스 헤더)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>


using namespace std;


// FD File Descripter
// Socket을 리눅스에선 File로 관리하기 때문에
// 소켓에 해당되는 파일 번호(int) 정보를 저장.

// poll 상대방이 반응을 해줬을때
// 누군가 메세지를 전해줬을대 서버실행
struct pollfd pollFDArray[MAX_USER_NUMBER];

// ListenFD 입구 역할
// 0번째 유저를 리슨소켓으로 사용
struct pollfd& listenFD = pollFDArray[0];

// 받을내용을 저장하는공간
char buffRecv[MAX_BUFFER_SIZE] = { 0 };
// 보낼 내용을 저장하는 공간
char buffSend[MAX_BUFFER_SIZE] = { 0 };

// 현재 유저수 
unsigned int currentUserNumber = 0;

void EndFD(struct pollfd* targetFD);
int StartServer(int currentFD);

// 윗 변수들을 사용하기위해 헤더 위치를 여기로 조정
#include "Message.h"


int StartServer(int currentFD) {

	// listenFD 초기화
	if (currentFD == -1) {
		perror("empty Socket()");

		// 실패시 소켓 종료
		close(currentFD);
		
		return -1;
	}

	// 소켓에다가 INET 정보 필요
	// 서버 아이피를 listenSocket에 등록
	sockaddr_in address;

	// address 초기화
	// 내부 값 모드 0
	memset(&address, 0, sizeof(address));

	// 소켓이랑 이이피 맞추기
	address.sin_family = AF_INET;
	// 실제 서버 아이피 삽입
	address.sin_addr.s_addr = inet_addr(SERVER_PRIVATE_IP);
	// 실제 서버 포트 삽입
	address.sin_port = htons(SERVER_PORT);

	// 사용해서 서버를 만들어야되니
	// 현재 설정된 서버 소켓에 묶기							-1 == fail
	if (bind(currentFD, (struct sockaddr*)&address, sizeof(address)) == -1) {
		perror("bind()");
		close(currentFD);
		return -1;
	};

	// listen socket 작동 ip 2^3 == 8
	if (listen(currentFD, 8) == -1) {
		perror("listen()");
		close(currentFD);
		return -1;
	};
	
	cout << " Complete Server " << endl; 

	return 1;
}


int main() {

	

	// INET 4.3.0 = ipv4(4byte IP)
	listenFD.fd = socket(AF_INET, SOCK_STREAM, 0);
	listenFD.events = POLLIN;
	listenFD.revents = 0;

	//  0은 리슨소켓
	for (int i = 1; i < MAX_USER_NUMBER; i++) {
		// FD가 등록되지않은 poolFD
		// 초기화
		// -1 = 해당 소켓이 없는 상태
		pollFDArray[i].fd = -1;

	};

	// 서버를 실행
	// listenFD에 정보를 전달하여 실행
	StartServer(listenFD.fd);
	
	while (true)
	{
		// poll 메세지 전달시 실행
		// 0번까지 폴에 넣어 리슨소켓에 대답있을때에도 돌아가도록 설정
		int result = poll(pollFDArray, MAX_USER_NUMBER, -1);
		

		// 연결을 시도하고 싶어하는 소켓을 새로준비
		struct sockaddr_in connectSocket;
		
		socklen_t addressSize;


		// 호출 시 
		if (result > 0) {
			// listenFD에 반응 시
			// 접속 시도 시
			if (listenFD.revents == POLLIN) {
				cout << " Someone Connected " << endl;
				// 
				int currendFD = accept(listenFD.fd, (struct sockaddr*)&connectSocket, &addressSize);

				if (currentUserNumber < MAX_USER_NUMBER - 1) {
					// 0번은 리슨 소켓이므로
					for (int i = 1; i < MAX_USER_NUMBER; i++) {
						// 비어있는 pollFD찾기

						if (pollFDArray[i].fd == -1) {

							// 지금 연결한 소켓의 파일 FD를 받아옴
							pollFDArray[i].fd = currendFD;
							pollFDArray[i].events = POLLIN;
							pollFDArray[i].revents = 0;

							cout << "Connected" << endl;

							// 유저수 추가후
							++currentUserNumber;

							break;
						};

					};
				};

			};

			// 이 위쪽은 접속을 관리하는 리슨소켓 내용
			for (int i = 1; i < MAX_USER_NUMBER; i++) {

				// 대상이 전달해준 반응
				switch (pollFDArray[i].revents)
				{
				// 반응 없음
				case 0: break;
				// 반응 있음
				case POLLIN: 
					// 무슨 반응이 있는지 확인
					//						읽기 버퍼				연결해제 요청
					if (read(pollFDArray[i].fd, buffRecv, MAX_BUFFER_SIZE) > 1) {
						EndFD(&pollFDArray[i]);
					};
					
					// 종료가 아닌 다른걸 부탁할때 메세지 처리
					BroadCastMessage(buffRecv, sizeof(buffRecv));

					// 입력버퍼 초기화
					memset(buffRecv, 0, sizeof(buffRecv));
					
					// 입력해결 완료후 내용없다.
					pollFDArray[i].revents = 0;

					break;

				// 잘못된 값
				default:
					EndFD(&pollFDArray[i]);
					break;
				};

			};
		};
	};

	close(listenFD.fd);
	
	for (int i = 0; i < MAX_USER_NUMBER; i++) {
		// 유저 존재시 닫기
		if (pollFDArray[i].fd != -1) close(pollFDArray[i].fd);
	};
	return -4;
}

// 대상 소켓을 정리
void EndFD(struct pollfd* targetFD) {
	
	// 닫아주기
	close(targetFD->fd);

	// 닫았으니깐 -1로 표시하기
	targetFD->fd = -1;
	targetFD->revents = 0;

	cout << "User Connection has Destroyed" << endl;
}


