class User* userArray[MAX_USER_NUMBER] = { nullptr };

class User
{
	int fdNumber;
	string name = "<NULL>";
	//�α����� �Ǿ��ִ°�?
	bool isLogin = false;

public:
	User(int targetFD)
	{
		fdNumber = targetFD;
	}

	bool LogIn(string wantName)
	{
		//���⿡���� ���� �ߺ� �α����̶�� �ϴ� ���� �������� �ʿ䰡 �ֽ��ϴ�!
		//���� ���� �̸��� ����� ���´ٰų� �� ����� �̹� �α��� �Ǿ��ִٰ� �� ���¿���
		//�ٽ� �α����� �õ����� �� ������ ����ٰ� �˷��� �� �ְ���!
		if (isLogin) return false;

		//�̸� ������ �����߽��ϴ�!
		if (!SetName(wantName)) return false;

		return true;
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