bool SendMessage(char* message, int length, int sendFD)
{
	//������ ���� ���� �� "���� �ִ� ��"���� �׷��� Write��� �θ���
	//���� ������ Read�ϰ���?
	write(sendFD, message, length);
	return true;
}

//                                              ���ο��� ������� �⺻������ true����~!
//												�׷� ��.. üũ���ص� �ǰ���!
//												üũ�Ϸ��� FD�� �����Ǿ�� �Ǵϱ�!
//												sendSelf�� false�� �ϰ� ������ sendFD��
//												�� ���ؾ� �� �̴ϴ�!
void BroadCastMessage(char* message, int length, int sendFD = -1, bool sendSelf = true)
{
	//����� ������ �ο����� �˰� �ֽ��ϴ�! �� ���� ��,
	//������ �� ���� �ʾƵ� �������� �� ������ �ߴٸ�
	//���� ��.. �� �� �ʿ� �ֳ�!
	//���� ������ send�� 1�� �÷��ְ�, ��������ŭ ���´ٸ� ������!

	//sendSelf�� false�� -> �������� �����ִ� �� �ƴϸ�! ������ �ϴ� ���� ������
	//��������� �ϰ���! �������� ������ �� Ĩ�ô�!
	//�ݺ����� �� �� �� ��µ�.. ������..? �ϸ鼭 ������ �� �ſ���! �װ� �����ֱ�!
	int send = sendSelf ? 0 : 1;
	//			 ����   ? true : false
	//     �������� ����?  �ƹ����׵� ���� �Ⱥ��´�! : 1�� ��������!
	
	// 0���� ���� ����!   �ִ�ġ���� ���ų�, �Ǵ� ���� ���� ����ŭ ���´ٸ�!
	for (int i = 1; i < MAX_USER_NUMBER; i++)
	{
		//�������� �� ��������! �̶�� �� �� �޾ƿ� ������ �ִٸ� �Ѿ��!
		if (!sendSelf && i == sendFD) continue;

		//����� ���µ� ���� �� ������?
		if (pollFDArray[i].fd != -1)
		{
			//           �޽���,   ����,   ����� ����
			SendMessage(message, length, pollFDArray[i].fd);

			//���´�!   �׷����� �� ����! �̶�� ���� �� �����ֱ�!
			if (++send >= currentUserNumber) break;
		};
	};
}

//�޽����� �����ϴ� �뵵                  �̰� �� ����
MessageInfo* ProcessMessage(char* input, int userIndex)
{
	for (int i = 0; i < 4; i++)
	{
		byteConvertor.character[i] = input[i];
	};
	//�޽���Ÿ��		����
	//[][]			[][]

	MessageInfo* result;
	//�޽��� Ÿ�Կ� ���� ���� �־��ֱ�!
	switch ((MessageType)byteConvertor.shortInteger[0])
	{
	case MessageType::LogIn:
		result = new MessageInfo_Login(input, userIndex);
		break;
	default:
		result = new MessageInfo();
		result->type = MessageType::Chat;	//Ÿ�� �����ֱ�!
		break;
	}
	result->length	= byteConvertor.shortInteger[1] + 4;			//���̵� �ݽô�!

	return result;
}

int TranslateMessage(int fromFD, char* message, int messageLength, MessageInfo* info)
{
	//��ü ���̿� �ϳ��� �޽��� ���� �� �߿� ���� ������!
	int currentLength = min(messageLength, info->length);
	//�޸� �߿��� ���� ó���ؾ��ϴ� �޸𸮱�����!
	char* target = new char[currentLength];
	memcpy(target, message, currentLength);

	//Ÿ�Կ� ���� �ٸ� �ൿ!
	switch (info->type)
	{
	case MessageType::Chat:
		BroadCastMessage(target, currentLength, fromFD);
		cout << "Message Send To " << send << "User : " << target + 4 << endl;
		break;
	case MessageType::LogIn:
	{
		MessageInfo_Login* loginInfo = (MessageInfo_Login*)info;
		//�α��� �������� �̸��� �޾ƿͼ� �õ��غ��ϴ�!
		if (userArray[fromFD]->LogIn(loginInfo->name))
		{
			BroadCastMessage(target, currentLength, fromFD);
		};
		cout << "Someone Try Login! Name is " << loginInfo->name << "!!" << endl;
		break;
	}
	case MessageType::LogOut:
		break;
	default:break;
	}
	//��� �޽������� ���� �ϳ��� ������ ���� ȿ���� �������ϴ� ����
	//���� �� ���� �� �������� ���� ������ �� �����ϴ�!
	//��Ƶξ��ٰ� ������ ����!
	//��ü �޽��� ���� - ���� Ȯ���� �޽��� ����!
	//���� �ڿ� �޽����� �� �־��! ��� �ϴ� �� Ȯ���� �� ����!
	//�׷��� ó���� ���̸� ���⿡�� Ȯ���ϰ� ���Կ�!
	return currentLength;
}
