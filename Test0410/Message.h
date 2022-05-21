//�޽����� ���޹޾Ƽ� �� ������ ����Ʈ������ �и����ִ� �༮!
//�޽��� Ÿ���̳� ���̴� �� �о �����帳�ϴ�^^
void DebugMessage(char* message)
{
	//ó�� ����� �о���� �����մϴ�!
	for (int i = 0; i < 4; i++)
	{
		byteConvertor.character[i] = message[i];
	};

	unsigned short type = byteConvertor.uShortInteger[0];

	//Ÿ���� 0�̴ϱ�! �� �޽����� �����!
	if (type == 0) 
	{ 
		return;
	};
	//ù ��° �� ���� ����Ʈ�� Ÿ������!
	cout << "[ type : " << type << "] ";

	//�� ���� �� ���� ����Ʈ�� ���̷�!
	unsigned short length = byteConvertor.uShortInteger[1];
	cout << "[ leng : " << length << "] ";

	//�ڿ� �ִ� �ֵ��� ���� �о����!
	for (int i = 0; i < length; i++)
	{
		cout << "[" << (int)message[i + 4] << "]";
	};

	cout << endl;
}

bool SendMessage(char* message, int length, int userNumber)
{
	//������ ���� ���� �� "���� �ִ� ��"���� �׷��� Write��� �θ���
	//���� ������ Read�ϰ���?
	//������ ������ ������ ����! ������ �ϳ��� �ֽ��ϴ�!
	//�̹� ������ �ִ� �߿��� �߰����� �޽����� �����Ⱑ ������!
	//���� �̰� ������ �־ ���� �� ó���ص帮�Ⱑ ����׿�!
	//"�̰� �� �ϰ� �����帱�Կ�^^ ��� �ϸ� �� ��������! ������ �׷��� �ൿ���� �ʾƿ�!
	//"���� �ϰ� �����ϱ� ������ ������!" ��� �׳� ���������ϴ�!
	//�׷� �޼����� ��� ������? ��� �����Ͻ� �� ����!
	//"�޼��� ť"��� �ϴ� ���� �ʿ��մϴ�! �ش� �������� ������ �ϴ� �޽�������
	//�����س��� ������ �ʿ��ؿ�! ������ ������ �� ��Ƶη���!
	//�ش� ������ ������ ��쿡!
	if (userArray[userNumber] != nullptr)
	{
		//�ش� �������� �޽����� �������ֱ�!
		userArray[userNumber]->MessageQueue(message, length);
	}
	//write(sendFD, message, length);
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
			//           �޽���,   ����,   ���
			SendMessage(message, length,  i);

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

	DebugMessage(input);

	MessageInfo* result;
	//�޽��� Ÿ�Կ� ���� ���� �־��ֱ�!
	switch ((MessageType)byteConvertor.shortInteger[0])
	{
	case MessageType::EndOfLine:return nullptr;

	case MessageType::LogIn:	result = new MessageInfo_Login(input, userIndex);
		break;
	case MessageType::Chat:		result = new MessageInfo_Chat(input, userIndex);
		break;
	case MessageType::Input:
		//4��° ĭ���� ���� �Է� Ÿ���� ������ҽ��ϴ�!
		for (int i = 0; i < 4; i++) byteConvertor.character[i] = input[i + 4];
		result = new MessageInfo_Input((InputType)byteConvertor.integer, userIndex);
		break;
	default:					result = new MessageInfo();
								result->type = MessageType::Unknown;
	}
	result->length	= byteConvertor.shortInteger[1] + 4;			//���̵� �ݽô�!

	return result;
}

int TranslateMessage(int fromFD, char* message, int messageLength, MessageInfo* info)
{
	//�ƹ� �͵� ���µ���?  //������ �̵�!
	if (info == nullptr) return MAX_BUFFER_SIZE;

	//��ü ���̿� �ϳ��� �޽��� ���� �� �߿� ���� ������!
	int currentLength = min(messageLength, info->length);	

	//Ÿ�Կ� ���� �ٸ� �ൿ!
	switch (info->type)
	{
	case MessageType::Chat:
	{
		MessageInfo_Chat* chatInfo = (MessageInfo_Chat*)info;
		//�޸� �߿��� ���� ó���ؾ��ϴ� �޸𸮱�����!
		char* sendResult = new char[currentLength + 4];
		byteConvertor.uShortInteger[0] = (short)MessageType::Chat;
		//                               ��ü �޽��� ���̿��� ��� ���� �� ��!
		byteConvertor.uShortInteger[1] = currentLength - 4;

		//���ʿ��� ���� ���� ���� ����� �־��ִ� �ſ���!
		for (int i = 0; i < 4; i++)
		{
			sendResult[i] = byteConvertor.character[i];
		};
		//�� ���� 4byte���� ���� ��ȣ�� �Է����ݴϴ�!
		byteConvertor.integer = fromFD;
		//4 ~ 7������ ���� ��ȣ�� ���ϴ�!
		for (int i = 0; i < 4; i++)
		{
			sendResult[i + 4] = byteConvertor.character[i];
		};
		// 4ĭ�� �߰��� �س��ұ� ������ 4ĭ�� ������ �� �ֵ��� �ϰڽ��ϴ�!
		memcpy(sendResult + 8, message + 4, currentLength - 4);
		//                 ����                             ����      �� ���� �����Ǵ� ������ ������ �����ݽô�!

		BroadCastMessage(sendResult, currentLength + 4, fromFD);

		cout << "Message Send From " << userArray[fromFD]->GetName() << " : " << sendResult + 8 << endl;
		delete sendResult;
		break;
	}
	case MessageType::LogIn:
	{
		MessageInfo_Login* loginInfo = (MessageInfo_Login*)info;

		cout << "Someone Try Login! Name is " << loginInfo->name << "!!" << endl;
		//           ������ȣ  �������� (�α��� ���� / �ߺ� �α��� / �ű� �α���)
		//[][] [][]  [][][][] []
		char sendResult[9] = { 0 };

		byteConvertor.uShortInteger[0] = (short)MessageType::LogIn;
		byteConvertor.uShortInteger[1] = 5;

		for (int i = 0; i < 4; i++)
		{
			sendResult[i] = byteConvertor.character[i];
		};

		sendResult[8] = userArray[fromFD]->LogIn(loginInfo->name);

		//�α��� �������� �̸��� �޾ƿͼ� �õ��غ��ϴ�!
		switch (sendResult[8])
		{
		case 0: cout << "Login Succeed"		<< endl; break;
		case 1: cout << "Invalide Password" << endl; break;
		case 2: cout << "Already Logined"	<< endl; break;
		case 3: cout << "Non-Exist ID"		<< endl; break;
		case 4: cout << "ID Not Fit In Form"<< endl; break;
		default:cout << "Unknown Error"		<< endl; break;
		};

		//�������� ���� ���θ� �����ִ� ��!
		SendMessage(sendResult, 9, fromFD);

		//�α��ο� ������ ��쿡�� �ٸ� ��������� �ƹ� ���� ���ص� �����ƿ�!
		if (sendResult[8] != 0) break;

		//�ٸ� ��������� ������ ����!
		char* broadcastResult;

		int currentLength = 0;
		//���� ���� ������ �ʾҴ�!
		while (loginInfo->name[currentLength] != '\0')
		{
			++currentLength; //���� ���� ���� ������ �޷��� ���̸� ����!
		};

		//�Ϲ����� ���� [][] [][] [][][][] -> [][][][][][][][][][]
		broadcastResult = new char[currentLength + 8];
		byteConvertor.uShortInteger[0] = (short)MessageType::LogIn;
		byteConvertor.uShortInteger[1] = currentLength + 4;
		//                                            ������ȣ �� 4ĭ!
		for (int i = 0; i < 4; i++) broadcastResult[i] = byteConvertor.character[i];

		//���� ��ȣ �����ΰ���?
		byteConvertor.integer = loginInfo->userIndex;
		for (int i = 0; i < 4; i++) broadcastResult[i + 4] = byteConvertor.character[i];

		//�̸����� ä���ֱ�!
		memcpy(broadcastResult + 8, loginInfo->name.c_str(), currentLength);

		//���� ����սô�!
		BroadCastMessage(broadcastResult, currentLength + 8);

		//�� ����ϴ� ^^
		delete broadcastResult;
		break;
	}
	case MessageType::LogOut:
		break;
	case MessageType::Input:
	{
		currentLength += 4;
		MessageInfo_Input* inputInfo = (MessageInfo_Input*)info;
		char* broadcastResult = new char[12];

		byteConvertor.uShortInteger[0] = (short)MessageType::Input;
		byteConvertor.uShortInteger[1] = 8;
		for (int i = 0; i < 4; i++) broadcastResult[i] = byteConvertor.character[i];

		byteConvertor.integer = inputInfo->userIndex;
		for (int i = 0; i < 4; i++) broadcastResult[i + 4] = byteConvertor.character[i];

		byteConvertor.integer = (int)inputInfo->currentType;
		for (int i = 0; i < 4; i++) broadcastResult[i + 8] = byteConvertor.character[i];

		BroadCastMessage(broadcastResult, 12);
		delete[] broadcastResult;
		break;
	}
	case MessageType::EndOfLine:
		return MAX_BUFFER_SIZE; //�ִ�ġ���� �о �� �ڿ� �޽����� �� ���ٰ� �˷��ݴϴ�!

	default:
		break;
	}

	//�޽��� ó�� �Ϸ�!
	delete info;

	//��� �޽������� ���� �ϳ��� ������ ���� ȿ���� �������ϴ� ����
	//���� �� ���� �� �������� ���� ������ �� �����ϴ�!
	//��Ƶξ��ٰ� ������ ����!
	//��ü �޽��� ���� - ���� Ȯ���� �޽��� ����!
	//���� �ڿ� �޽����� �� �־��! ��� �ϴ� �� Ȯ���� �� ����!
	//�׷��� ó���� ���̸� ���⿡�� Ȯ���ϰ� ���Կ�!
	return currentLength;
}
