
// �׻� Ŭ���̾�Ʈ�� eunm���� ��ġ���Ѿ���
enum class MessageType
{
	LogIn,
	LogOut,
	Chat,



	Length // ���� ���� �޽��� Ÿ���� �������� ������ ����!


};

// struct�� ����ϰ� ��밡��
// �ϳ��� �޸𸮸� �������� �ڷ����� ��������

// 0 0 0 10 
// int  10
// char \n
// �ϳ��� �޸𸮸� ���� �ڷ������� ����

union ConvertionBase {
	
	// ����ũ�� �ڷ������� ���
	unsigned int uInteger;
	int integer;
	
	float floating;

	char character[4];

	short shortInteger[2];
	unsigned short uShortInteger[2];
};

ConvertionBase byteConvertor;

struct MessageInfo {
	MessageType type;
	int length;
};

//												���ο��� ������� true 
//												sendSelf true�� �����ϱ����� 
//												sendFD�� �ʱ�ȭ�ؾ���
void BroadCastMessage(char* message, int length, int sendFD = -1, bool sendSelf = true) {
	
	// ������ �ο� ���� �˰�
	// ������ ã���ʾƵ� �������� �� �����ߴٸ� ��
	// ���������� send�� �÷��ְ� ��������ŭ ���� �� ������ 
	// sendSelf�� false�� �������׺����ִ°� �ƴϸ� ������ �ϴ� ����������
	// ���� �ؾ��Ѵ�

	// �ݺ����� �� �Ѹ� �� ã�°� ���
	//		�����̶�� 0 �ƴϸ� 1
	int send = sendSelf ? 0 : 1;
	
	
	// 0���� �������� i�� �ִ�ġ���� ���ų� ���� ���� ����ŭ ���´ٸ�
	for (int i = 1; i < MAX_USER_NUMBER ; i++) {

		// || send >= currentUserNumber
		// ���ο��� �Ⱥ������� �޾ƿ� ������ �ִٸ� �Ѿ��
		if (!sendSelf && i == sendFD) continue;

		// ����� ���µ� ���� �� ���� ����
		if (pollFDArray[i].fd != -1) {

			// ������ ���� ������  "�޸� �ϱ�" 
			// ������ Read()
			//		����� �Ǵ� ����	���� �޼���	����
			if (write(pollFDArray[i].fd, message, length)) {
				
				
				// ������!  �ٺ������� �������ֱ�
				if (++send >= currentUserNumber) return;
			};

		};
	};

}

// �޽����� �����ϴ� �뵵                     ���� ���� int�ּ���
MessageInfo ProcessMessage(char input[4]) {

	for (int i = 0; i < 4; i++) {
		byteConvertor.character[i] = input[i];
	};


	// �޼��� Ÿ�� ����
	// [][]       [][]
	
	MessageInfo result;
	
	result.type = byteConvertor.shortInteger[0];		// Ÿ�� �����ֱ�
	result.length = byteConvertor.shortInteger[1] + 4;		// ���̱��� 4qkdlxm cnrk

	return result;
}

int TranslateMessage(int fromFD, char* message, int messageLength, MessageInfo info) {
	
	// ��ü ���̿� �ϳ��� �޼����� ������ �� ����������
	int currentLength = min(messageLength, info.length);

	// �޸��߿��� ó���ؾߵǴ� �޸𸮱�����
	char* target = new char[currentLength];
	memcpy(target, message, currentLength);
	
	// Ÿ�Կ� ���� �з�
	switch (info.type)
	{
	case MessageType::Chat:

		BroadCastMessage(target, currentLength, fromFD);
		break;

	case MessageType::LogIn:
		break;

	case MessageType::LogOut:
		break;

	default:
		break;
	};


	// �޽������ �ϳ��� ������ ȿ���� �������Ƿ�
	// �ѹ��� �������� �ϴ°� ����
	// ��ü �޼��� ���� - ���� Ȯ���� �޽��� ����
	// 
	return messageLength - info.length;
}