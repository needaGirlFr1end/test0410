class User* userArray[MAX_USER_NUMBER] = { nullptr };

class User
{
	int fdNumber;
	string name = "<NULL>";

	//�̰� waitSend�� ������ ��! �� �ſ���!
	std::queue<char*> waitQueue;

	//���� ���޵��� ���� ������� �޽�����!
	char waitSend[MAX_BUFFER_SIZE] = { 0 };
	//��ٸ��� �ִ� �޽����� ����!
	int waitLength = 0;

	//�α����� �Ǿ��ִ°�?
	bool isLogin = false;

public:
	string GetName()
	{
		return name;
	};

	User(int targetFD)
	{
		fdNumber = targetFD;
	}

	void MessageQueue(char* message, int length)
	{
		//�޽��� �� ���̸� �Ѿ��� ��! ť���ٰ� �־��ָ� �˴ϴ�!
		if (length + waitLength >= MAX_BUFFER_SIZE)
		{

		}
		else //���� �޽����� �� �� �ؿ�!
		{
			//     ����ִ� �޽��� ����     �޽���   ���̸�ŭ
			memcpy(waitSend + waitLength, message, length);

			//�� ��ŭ ���̰� �þ���ϱ�!
			waitLength += length;
		};
	}

	//�޽��� ť�� �ִ� ������ �����ϴ� �ſ���!
	void Send()
	{
		//������ �ϴ� ���̰� ���� �����ִٰ� ���� ��!
		if (waitLength > 0)
		{
			//write�¿�! �����͸� ������ ��! ���� ���̸� �ٽ� �������� �˷��ݴϴ�!
			//0���� ũ�ٰ� �Ѵٸ� �޽����� �� �������ٰ� �� �� ���� �������?
			if (write(pollFDArray[fdNumber].fd, waitSend, waitLength) > 0)
			{
				//���� ���뵵 0���� �ʱ�ȭ!
				memset(waitSend, 0, waitLength);
				//�ʱ�ȭ!
				waitLength = 0;
			};
			//if���� ���� ���ߴٴ� �Ŵ� ������ ���ߴٴ� �Ŵϱ�! �ʱ�ȭ�� ���� �ʽ��ϴ�!
		};
	}

	int LogIn(string wantName)
	{
		//���⿡���� ���� �ߺ� �α����̶�� �ϴ� ���� �������� �ʿ䰡 �ֽ��ϴ�!
		//���� ���� �̸��� ����� ���´ٰų� �� ����� �̹� �α��� �Ǿ��ִٰ� �� ���¿���
		//�ٽ� �α����� �õ����� �� ������ ����ٰ� �˷��� �� �ְ���!
		if (isLogin) return 2;

		//�̸� ������ �����߽��ϴ�!
		if (!SetName(wantName)) return 4;

		return 0;
	};

	//�ش� �̸��� ��ȿ���� üũ�ϱ�!
	bool CheckNameValidate(string wantName)
	{
		//�̸��� �ʹ� ��ϴ�!
		if (wantName.length() > 16) return false;

		int leftUser = currentUserNumber;
		for (int i = 0; i < MAX_USER_NUMBER; i++)
		{
			if (userArray[i] != nullptr)
			{
				//�� ���� �̸� �Ȱ��ƿ�!
				if (userArray[i]->name == wantName) return false;
				//üũ�� �༮ �ϳ� ����!
				--leftUser;
			};
		};

		return true;
	}

	bool SetName(string wantName) 
	{ 
		if (!CheckNameValidate(wantName)) return false;
		name = wantName; 
		return true;
	};
};