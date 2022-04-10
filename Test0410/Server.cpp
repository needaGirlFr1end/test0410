// 서버는 아이피를 가지고 시작
// 아이피로 접속
// 외부에서 접속시 퍼블릭 아이피가 필요하지만
// 내부 공유기한테 서버를 실행시 내부 개인 아이피가 필요함.

// 내부 아이피를 정의
#define SERVER_PRIVATE_IP "172.31.38.57"

// 컴퓨터에는 여러개의 프로그램이 작동
// 네트워크 사용시
// 각각 다른 게임 켰을 시 각 서버연결된 메세지를 포트로 구분함 

// 49152 ~ 65535 자유롭게 사용가능
#define SERVER_PORT "49153"

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
int StartServer(int currentFD);

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
	if (bind(currentFD, (struct sockaddr*)&address, sizeof(address) == -1)) {
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
	

	return 1;
}


int main() {

	// ListenFD 입구 역할
	// 0번째 유저를 리슨소켓으로 사용
	struct pollfd& listenFD = pollFDArray[0];

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
		
		// 호출 시 
		if (result > 0) {
			// listenFD에 반응 시
			// 접속 시도 시
			if (listenFD.revents == POLLIN) {
				cout << " 접속 시도 " << endl;
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




