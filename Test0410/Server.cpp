// ������ �����Ǹ� ������ ����
// �����Ƿ� ����
// �ܺο��� ���ӽ� �ۺ� �����ǰ� �ʿ�������
// ���� ���������� ������ ����� ���� ���� �����ǰ� �ʿ���.

// ���� �����Ǹ� ����
#define SERVER_PRIVATE_IP "172.31.41.33"

// ��ǻ�Ϳ��� �������� ���α׷��� �۵�
// ��Ʈ��ũ ����
// ���� �ٸ� ���� ���� �� �� ��������� �޼����� ��Ʈ�� ������ 

// 49152 ~ 65535 �����Ӱ� ��밡��
#define SERVER_PORT 49153

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

// ListenFD �Ա� ����
// 0��° ������ ������������ ���
struct pollfd& listenFD = pollFDArray[0];

// ���������� �����ϴ°���
char buffRecv[MAX_BUFFER_SIZE] = { 0 };
// ���� ������ �����ϴ� ����
char buffSend[MAX_BUFFER_SIZE] = { 0 };

// ���� ������ 
unsigned int currentUserNumber = 0;

void EndFD(struct pollfd* targetFD);
int StartServer(int currentFD);

// �� �������� ����ϱ����� ��� ��ġ�� ����� ����
#include "Message.h"


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
	if (bind(currentFD, (struct sockaddr*)&address, sizeof(address)) == -1) {
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
	
	cout << " Complete Server " << endl; 

	return 1;
}


int main() {

	

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
		

		// ������ �õ��ϰ� �;��ϴ� ������ �����غ�
		struct sockaddr_in connectSocket;
		
		socklen_t addressSize;


		// ȣ�� �� 
		if (result > 0) {
			// listenFD�� ���� ��
			// ���� �õ� ��
			if (listenFD.revents == POLLIN) {
				cout << " Someone Connected " << endl;
				// 
				int currendFD = accept(listenFD.fd, (struct sockaddr*)&connectSocket, &addressSize);

				if (currentUserNumber < MAX_USER_NUMBER - 1) {
					// 0���� ���� �����̹Ƿ�
					for (int i = 1; i < MAX_USER_NUMBER; i++) {
						// ����ִ� pollFDã��

						if (pollFDArray[i].fd == -1) {

							// ���� ������ ������ ���� FD�� �޾ƿ�
							pollFDArray[i].fd = currendFD;
							pollFDArray[i].events = POLLIN;
							pollFDArray[i].revents = 0;

							cout << "Connected" << endl;

							// ������ �߰���
							++currentUserNumber;

							break;
						};

					};
				};

			};

			// �� ������ ������ �����ϴ� �������� ����
			for (int i = 1; i < MAX_USER_NUMBER; i++) {

				// ����� �������� ����
				switch (pollFDArray[i].revents)
				{
				// ���� ����
				case 0: break;
				// ���� ����
				case POLLIN: 
					// ���� ������ �ִ��� Ȯ��
					//						�б� ����				�������� ��û
					if (read(pollFDArray[i].fd, buffRecv, MAX_BUFFER_SIZE) > 1) {
						EndFD(&pollFDArray[i]);
					};
					
					// ���ᰡ �ƴ� �ٸ��� ��Ź�Ҷ� �޼��� ó��
					BroadCastMessage(buffRecv, sizeof(buffRecv));

					// �Է¹��� �ʱ�ȭ
					memset(buffRecv, 0, sizeof(buffRecv));
					
					// �Է��ذ� �Ϸ��� �������.
					pollFDArray[i].revents = 0;

					break;

				// �߸��� ��
				default:
					EndFD(&pollFDArray[i]);
					break;
				};

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

// ��� ������ ����
void EndFD(struct pollfd* targetFD) {
	
	// �ݾ��ֱ�
	close(targetFD->fd);

	// �ݾ����ϱ� -1�� ǥ���ϱ�
	targetFD->fd = -1;
	targetFD->revents = 0;

	cout << "User Connection has Destroyed" << endl;
}


