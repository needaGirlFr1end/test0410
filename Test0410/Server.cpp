// ������ �����Ǹ� ������ ����
// �����Ƿ� ����
// �ܺο��� ���ӽ� �ۺ� �����ǰ� �ʿ�������
// ���� ���������� ������ ����� ���� ���� �����ǰ� �ʿ���.

// ���� �����Ǹ� ����
#define SERVER_PRIVATE_IP "172.31.38.57"

// ��ǻ�Ϳ��� �������� ���α׷��� �۵�
// ��Ʈ��ũ ����
// ���� �ٸ� ���� ���� �� �� ��������� �޼����� ��Ʈ�� ������ 

// 49152 ~ 65535 �����Ӱ� ��밡��
#define SERVER_PORT "49153"

// ���������� �޼����� �������Դϴ�
// �޼��� ������ ����
// ��Ʈ��ũ ���������� �Ѱ� ����

// �ִ� ���� ������ ����
#define MAX_BUFFER_SIZE 1024

// �������� ������ ���� �� ������ �ʿ���
#define MAX_USER_NUMBER 100

#include <iostream>

// ���� �����쿡�� �ȳ��� (������ ���)
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
// Socket�� ���������� File�� �����ϱ� ������
// ���Ͽ� �ش�Ǵ� ���� ��ȣ(int) ������ ����.

// poll ������ ������ ��������
// ������ �޼����� ���������� ��������
struct pollfd pollFDArray[MAX_USER_NUMBER];
int StartServer(int currentFD);

int StartServer(int currentFD) {

	// listenFD �ʱ�ȭ
	if (currentFD == -1) {
		perror("empty Socket()");

		// ���н� ���� ����
		close(currentFD);
		
		return -1;
	}


	// ���Ͽ��ٰ� INET ���� �ʿ�
	// ���� �����Ǹ� listenSocket�� ���
	sockaddr_in address;

	// address �ʱ�ȭ
	// ���� �� ��� 0
	memset(&address, 0, sizeof(address));

	// �����̶� ������ ���߱�
	address.sin_family = AF_INET;
	// ���� ���� ������ ����
	address.sin_addr.s_addr = inet_addr(SERVER_PRIVATE_IP);
	// ���� ���� ��Ʈ ����
	address.sin_port = htons(SERVER_PORT);

	// ����ؼ� ������ �����ߵǴ�
	// ���� ������ ���� ���Ͽ� ����							-1 == fail
	if (bind(currentFD, (struct sockaddr*)&address, sizeof(address) == -1)) {
		perror("bind()");
		close(currentFD);
		return -1;
	};

	// listen socket �۵� ip 2^3 == 8
	if (listen(currentFD, 8) == -1) {
		perror("listen()");
		close(currentFD);
		return -1;
	};
	

	return 1;
}


int main() {

	// ListenFD �Ա� ����
	// 0��° ������ ������������ ���
	struct pollfd& listenFD = pollFDArray[0];

	// INET 4.3.0 = ipv4(4byte IP)
	listenFD.fd = socket(AF_INET, SOCK_STREAM, 0);
	listenFD.events = POLLIN;
	listenFD.revents = 0;

	//  0�� ��������
	for (int i = 1; i < MAX_USER_NUMBER; i++) {
		// FD�� ��ϵ������� poolFD
		// �ʱ�ȭ
		// -1 = �ش� ������ ���� ����
		pollFDArray[i].fd = -1;

	};

	// ������ ����
	// listenFD�� ������ �����Ͽ� ����
	StartServer(listenFD.fd);
	
	while (true)
	{
		// poll �޼��� ���޽� ����
		// 0������ ���� �־� �������Ͽ� ������������� ���ư����� ����
		int result = poll(pollFDArray, MAX_USER_NUMBER, -1);
		
		// ȣ�� �� 
		if (result > 0) {
			// listenFD�� ���� ��
			// ���� �õ� ��
			if (listenFD.revents == POLLIN) {
				cout << " ���� �õ� " << endl;
			};
		};
	};

	close(listenFD.fd);
	
	for (int i = 0; i < MAX_USER_NUMBER; i++) {
		// ���� ����� �ݱ�
		if (pollFDArray[i].fd != -1) close(pollFDArray[i].fd);
	};
	return -4;
}




