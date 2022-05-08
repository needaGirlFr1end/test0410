class User* userArray[MAX_USER_NUMBER] = { nullptr };

class User
{
	int fdNumber;
	string name = "<NULL>";

	//이건 waitSend가 부족할 때! 쓸 거에요!
	std::queue<char*> waitQueue;

	//아직 전달되지 않은 대기중인 메시지들!
	char waitSend[MAX_BUFFER_SIZE] = { 0 };
	//기다리고 있는 메시지의 길이!
	int waitLength = 0;

	//로그인이 되어있는가?
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
		//메시지 총 길이를 넘었을 때! 큐에다가 넣어주면 됩니다!
		if (length + waitLength >= MAX_BUFFER_SIZE)
		{

		}
		else //아직 메시지에 들어갈 만 해요!
		{
			//     비어있는 메시지 공간     메시지   길이만큼
			memcpy(waitSend + waitLength, message, length);

			//그 만큼 길이가 늘어났으니까!
			waitLength += length;
		};
	}

	//메시지 큐에 있는 내용을 전달하는 거에요!
	void Send()
	{
		//보내야 하는 길이가 아직 남아있다고 했을 때!
		if (waitLength > 0)
		{
			//write는요! 데이터를 보냈을 때! 보낸 길이를 다시 저희한테 알려줍니다!
			//0보다 크다고 한다면 메시지가 잘 보내졌다고 볼 수 있지 않을까요?
			if (write(pollFDArray[fdNumber].fd, waitSend, waitLength) > 0)
			{
				//보낼 내용도 0으로 초기화!
				memset(waitSend, 0, waitLength);
				//초기화!
				waitLength = 0;
			};
			//if문에 들어가지 못했다는 거는 보내지 못했다는 거니까! 초기화는 하지 않습니다!
		};
	}

	bool LogIn(string wantName)
	{
		//여기에서는 저희가 중복 로그인이라고 하는 것을 방지해줄 필요가 있습니다!
		//저희가 같은 이름의 사람이 들어온다거나 이 사람이 이미 로그인 되어있다고 한 상태에서
		//다시 로그인을 시도했을 때 문제가 생겼다고 알려줄 수 있겠죠!
		if (isLogin) return false;

		//이름 설정에 실패했습니다!
		if (!SetName(wantName)) return false;

		return true;
	};

	//해당 이름이 유효한지 체크하기!
	bool CheckNameValidate(string wantName)
	{
		//이름이 너무 깁니다!
		if (wantName.length() > 16) return false;

		int leftUser = currentUserNumber;
		for (int i = 0; i < MAX_USER_NUMBER; i++)
		{
			if (userArray[i] != nullptr)
			{
				//얘 저랑 이름 똑같아요!
				if (userArray[i]->name == wantName) return false;
				//체크한 녀석 하나 제거!
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