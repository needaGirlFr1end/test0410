//������ �����Ǹ� ������ ������ �մϴ�!
//�����Ƿ� ������ �ϴ� ������?
//�ܺο��� ������ �� ������ �ۺ�IP�� �ʿ�������, ������ �� ������
//���� ���������� ����IP�� �� �ſ���! ��� �̾߱��� �ʿ䰡 �ֽ��ϴ�!
//���� IP�� ���⿡�ٰ� �Է����ֽø� �˴ϴ�!
#define SERVER_PRIVATE_IP "172.31.41.33"

//��ǻ�Ϳ��� ���ÿ� �������� ���α׷��� �۵��ϰ� �ֽ��ϴ�!
//���縵�� �ϰ� �־����! ��Ʈ��ũ�� ����ϰ� ����!
//�������� ���� �ѵ׽��ϴ�! �����Ǹ� ������ ��ȭ�� �õ��ϸ��! ���� �޽����� ���縵 ������, ������ ������ ���� �� ���� �����!
//"��Ʈ"��� �ϴ� ���� ���� �޽������� ������ �� �ְ� �����!
//�� �� ��Ʈ�� �ָ� �� ���α׷��� �ٰԿ�^^ ��� �ϴ� ����!
//49152 ~ 65535 �� �����Ӱ� ����� �� �ִ� "���� ��Ʈ"�ϱ� �� ���̿� �ִ� ������ �������ٰԿ�!
#define SERVER_PORT 55123

//���������� �޽����� ���� �̴ϴ�!
//�޽��� ������ ���� ���� �����! ��Ʈ��ũ �������� ���������� �ִµ� �������� �Ѱ谡 ������ ���ۿ� ����!
//���� �׷��Ա��� ū �޽����� ���� ������ ���� �Ŵϱ� ������ �ִ� ���ۻ���� �������ֵ��� �սô�!
#define MAX_BUFFER_SIZE 1024

//�ִ밪�� ���ؾ��ϴ� �ٸ� ���� ���� �ſ���!
#define MAX_USER_NUMBER 100

//�޽����� �����µ� ������ ������ �ΰ� �����ϴ�!
#define SEND_TICK_RATE 30

//1�ʿ� �󸶳� ��������!
#define SEND_PER_SECONDS 1000 / SEND_TICK_RATE

#include <iostream>

//���� �����쿡�� �ȳ��ɴϴ�! ���������� ����� ����� �帮�� �ִ� �ſ���!
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <queue>

using namespace std;

//poll�̶�� �ϴ� ���� ������ ������ ���־��� ��! �ν��ؼ� �����ִ� �����̿���!
//������ ������ �޽����� �������� ��! ���� ������ ���ư�����!
//FD�� ������? File Descripter ������ �������ִ� �༮�̿�?
//������ ����ȭ�� ���ε�! �������� �����̶�� �ϴ� �͵� ������ ���� ���·� ������ �մϴ�!
//���� ��ȣ�� ������ �� �ִ� ������ �� �ſ���! ���Ͽ� �ش�Ǵ� ���� ����!
struct pollfd pollFDArray[MAX_USER_NUMBER];
//��⸸ �ϴ� fd�� �ʿ��ؿ�!
//��� �̻��� �ְ� �ͼ� �̾߱⸦ �ϸ� �ȵ��߰���!
//��ǻ�ʹ� ó�� ���� IP�� �ͼ� �̾߱��ϸ� "��ȭ �߸��ż̾��!" ��� �ϸ鼭 ���������!
//ListenFD��� �ϴ� �ִ� �� ����� �̴ϴ�!
//����! �� ���� ���� ������ �ϰ� �����Ű� ���ϴ�!
//�ٸ� FD���� ���ο� ������ �˷��ִ� ���ҷ� �� �ſ���! (�Ա� ������ �ϴ� �ſ���!)
//0��° ������ ������������ ����� �̴ϴ�!
struct pollfd& ListenFD = pollFDArray[0];

//���� ������ �����ϴ� ����(����)
char buffRecv[MAX_BUFFER_SIZE] = { 0 };
//���� ������ �����ϴ� ����(����)
char buffSend[MAX_BUFFER_SIZE] = { 0 };

//������ �κ�!
pthread_t sendThread;
pthread_t commandThread;

//���� ���� ��
unsigned int currentUserNumber = 0;

void EndFD(struct pollfd* targetFD);
int StartServer(int currentFD);

//�� #include�� ���⿡ �ֳ���?
//����� ���� �ٿ��ֱ�� ���⿡ �־�� ���� �ִ� �������� ����� �� �־ ���⿡�� �׾��!
#include "User.h"
#include "MessageInfo.h"
#include "Message.h"
#include "SQL.h"

//�������� �޽����� ������ �������Դϴ�!
void* SendThread(void* data)
{
	int checkNumber;
	while (true)
	{
		checkNumber = 0;
		//���� ��ü �����ֱ�!
		for (int i = 1; i < MAX_USER_NUMBER; i++)
		{
			//���� �ֳ�!
			if (userArray[i] != nullptr)
			{
				//��������!
				userArray[i]->Send();

				//üũ �߽��ϴ�!
				++checkNumber;
				//üũ ���߳׿�!
				if (checkNumber >= currentUserNumber) break;
			};
		};
	};
}

int main()
{
	              //IPv4(4����Ʈ¥�� IP)
	ListenFD.fd = socket(AF_INET, SOCK_STREAM, 0);
	ListenFD.events = POLLIN;
	ListenFD.revents = 0;

	//     0�� ���������̴ϱ�!
	for (int i = 1; i < MAX_USER_NUMBER; i++)
	{
		//File Descripter�� ��ϵ��� ���� pollFD�¿�! ������ �� �� ������!
		//�ش�Ǵ� ������ ���ٴ� ���Դϴ�!
		pollFDArray[i].fd = -1;
	};

	//���⼭ FD�� �غ� �Ǿ���! ������ �������ô�!
	//���� ������ ������ �������ָ鼭 ������ ������ �ſ���!
	StartServer(ListenFD.fd);

	while (true)
	{
		//poll�� ���ؼ� ������ �帱 ��! ������ ������ �޽����� �������� �� ����!
		//0�������� ���� �־ ���� ���Ͽ� ����� ���� ������ �� �� �ְ� ������ ����������!
		int result = poll(pollFDArray, MAX_USER_NUMBER, -1);

		//������ �õ��ϰ� �;��ϴ� ������ ���� �غ��մϴ�! ���ο� ������ ����� ���� ����?
		struct sockaddr_in connectSocket;
		//�����ϰ��� �ϴ� ������ �ּ� ������!
		socklen_t addressSize;

		//���� �θ�������? 0�̸� �ƹ��� �����ߴ�! 15��� �ϸ�, 15���� �θ���!
		if (result > 0)
		{
			//���� ���Ͽ� ���� Ȯ��!
			//������ ������ �õ��ϰ� �ֽ��ϴ�!
			if (ListenFD.revents == POLLIN)
			{
				cout << "Someone Connected!" << endl;
				//����� ������! �����¿� ������ ������ �ʹٸ� ��� �����ǰ� �ƴ϶�� �Ѵٸ��.. �� �޾���� �մϴ�!
				int currentFD = accept(ListenFD.fd, (struct sockaddr*)&connectSocket, &addressSize);

				//0���� ���� �������� ���� �־ ��ü ���� ���� -1�� ���¿��� ���ҰԿ�!
				if (currentUserNumber < MAX_USER_NUMBER - 1)
				{
					//0�� ���� �����̴ϱ�! 1���� �����ؼ� ���� ������!
					for (int i = 1; i < MAX_USER_NUMBER; i++)
					{
						//����ִ� pollFD�� ã�� �ſ���!
						if (pollFDArray[i].fd == -1)
						{
							//�̰� ����? ���� �ֳ�?
							if (userArray[i] != nullptr)
							{
								//������ Ȥ�� �������� ����� �� �־! ���������� ���Կ�!
								delete userArray[i];
							};

							//���� ������ ������ File Descriptor�� �޾ƿ���!
							pollFDArray[i].fd = currentFD;
							pollFDArray[i].events = POLLIN;
							pollFDArray[i].revents = 0;

							//������ ���� ������ֵ��� �սô�!
							userArray[i] = new User(i);

							cout << "Connected : " << i << endl;

							//���� �� �� �߰���!
							++currentUserNumber;
							//�� �� �����ϱ�! �����!
							break;
						};
					};
				};
			};

			//�� ������ ���� �����ϴ� ���� ���� �����̾�����
			//�Ʒ������� �ٸ� �Ϲ� �������� �����ϴ� �κ��� �ʿ��� �ſ���!
			for (int i = 1; i < MAX_USER_NUMBER; i++)
			{
				//����� �������� ����
				switch (pollFDArray[i].revents)
				{
					//���� ����!
				case 0:
				{
					break;
				}//Switch�� �ȿ� ������ �׷� �� ���� ���� �߰�ȣ�� �޾��ּž� �ؿ�!

					//���� ����!
				case POLLIN:
				{
					//���� �����̾������� Ȯ���غ��߰���!
					//                         �б� ����            ���� ���� ��û!
					if (read(pollFDArray[i].fd, buffRecv, MAX_BUFFER_SIZE) < 1)
					{
						//���޶�µ� �� ��
						EndFD(&pollFDArray[i]);
						break;
					};

					//���޶�� �ϴ� �� �ƴϰ� �ٸ� �� ��Ź���� �� ���⿡�� �޽����� ó���� �ʿ䰡 �ֱ���!
					//BroadCastMessage(buffRecv, sizeof(buffRecv));
					int leftSize = sizeof(buffRecv);
					int checkSize = 0;
					while (leftSize > 0)
					{
						//					�����̸鼭 ���� ������!
						int currentSize = TranslateMessage(i, buffRecv + checkSize, leftSize, ProcessMessage(buffRecv + checkSize, i));

						checkSize += currentSize;
						leftSize -= currentSize;
					};

					//�Է� ���� �ʱ�ȭ!
					memset(buffRecv, 0, sizeof(buffRecv));
					//�Է� �ذ��������ϱ� �� ������ ����!
					pollFDArray[i].revents = 0;
					break;
				}
				//�̻��� ���� ���� ����!
				default:
					EndFD(&pollFDArray[i]);
					break;
				};
			};
		};
	};

	//���� ���� �ݰ�
	close(ListenFD.fd);

	for (int i = 0; i < MAX_USER_NUMBER; i++)
	{
		//��.. ���� �־�? �ݾ�!
		if (pollFDArray[i].fd != -1) close(pollFDArray[i].fd);
	};
	return -4;
}

int StartServer(int currentFD)
{
	//������ �����Ϸ��� �ϴµ�.. �ƴ�!
	//���� ������ �����־��!
	if (currentFD == -1)
	{
		//���϶����� ������ �����!
		perror("socket()");
		//Ȥ�� �𸣴ϱ� ���� ���ֱ�!
		close(currentFD);
		return -1;
	};

	//���Ͽ��ٰ� INET�̶�� ������ �־���µ���
	//�׷�.. 4����Ʈ¥�� IP�� ���� �˰ڴµ�.. �׷��� IP�� ����?
	//�׷��� ����� ������ IP�� ���� ���Ͽ��ٰ� ����� ���־�� �մϴ�!
	sockaddr_in address;

	//Ȥ�� �𸣴ϱ� ���� �ʱ�ȭ �س��� ������ �����Կ�!
	//address ���θ� ���� 0���� ���߱�!
	memset(&address, 0, sizeof(address));

	//�����̶� ������ ������ �����ֵ��� �ҰԿ�!
	address.sin_family = AF_INET;
	//���� ���� IP�� ���⿡�� �־��ֵ��� �սô�!
	address.sin_addr.s_addr = inet_addr(SERVER_PRIVATE_IP);
	//��Ʈ���� �����־�� �߾���?
	address.sin_port = htons(SERVER_PORT);

	//�ּҰ� ���⿡�� �� �����ٸ� ������ ������ �ȵ� �ſ���! ���常 �� ������!
	//����ؼ� ������ ������ �� �Ŵϱ�!
	//���� ������ �ּҸ� ���Ͽ��ٰ� "����" �� �ſ���!                 ���д�!
	if (bind(currentFD, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		perror("bind()");
		close(currentFD);
		return -1;
	};

	//������ ���� ������ �۵��� ���Ѻ��ϴ�!
	//�׷� ��¥�� ������ ���ư��� �Ű���?
	if (listen(currentFD, 8) == -1)
	{
		perror("listen()");
		close(currentFD);
		return -1;
	};

	//�����带 �����ϴ�!
	if (pthread_create(&sendThread, NULL, SendThread, NULL) != 0)
	{
		cout << "Cannot Create Send Thread" << endl;
		return -1;
	};

	//SQL������� �õ��غ��ô�!
	if (SQLConnect() == -1)
	{
		//SQL������ ���ʿ��� �� �ȵǾ����� �̾߱����ݴϴ�! cout�� ���ҰԿ�!
		return -1;
	};

	cout << "Server is On the way" << endl;

	//����� ��� �÷��� �Ǹ��ϰ� �̰ܳ��̽��ϴ�
	return 1;
}

//��� ������ �����ϵ��� �սô�!
void EndFD(struct pollfd* targetFD)
{
	//�ݾ��ֱ�!
	close(targetFD->fd);

	//�ݾ����ϱ� -1�� ǥ���ϱ�~!
	targetFD->fd = -1;
	targetFD->revents = 0;

	//�������ϱ� ���� �� �ٿ��ֱ�!
	--currentUserNumber;

	cout << "User Connection has Destroyed" << endl;
}