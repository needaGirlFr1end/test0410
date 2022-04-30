bool SendMessage(char* message, int length, int sendFD)
{
	//서버가 무언가 보낼 때 "적어 주는 거"에요 그래서 Write라고 부르고
	//받을 때에는 Read하겠죠?
	write(sendFD, message, length);
	return true;
}

//                                              본인에게 보내기는 기본적으로 true에요~!
//												그럼 뭐.. 체크안해도 되겠죠!
//												체크하려면 FD가 설정되어야 되니까!
//												sendSelf를 false로 하고 싶으면 sendFD도
//												꼭 정해야 할 겁니다!
void BroadCastMessage(char* message, int length, int sendFD = -1, bool sendSelf = true)
{
	//저희는 유저의 인원수를 알고 있습니다! 그 말은 곧,
	//끝까지 다 돌지 않아도 유저한테 다 전달을 했다면
	//굳이 뭐.. 더 돌 필요 있나!
	//보낼 때마다 send를 1씩 늘려주고, 유저수만큼 보냈다면 끝내기!

	//sendSelf가 false면 -> 본인한테 보내주는 게 아니면! 보내야 하는 유저 개수를
	//수정해줘야 하겠죠! 본인한테 보내준 셈 칩시다!
	//반복문이 한 명 덜 줬는데.. 누구지..? 하면서 끝까지 갈 거에요! 그거 막아주기!
	int send = sendSelf ? 0 : 1;
	//			 조건   ? true : false
	//     본인한테 보냄?  아무한테도 아직 안보냈다! : 1명 보내놨다!
	
	// 0번은 리슨 소켓!   최대치까지 갔거나, 또는 현재 유저 수만큼 보냈다면!
	for (int i = 1; i < MAX_USER_NUMBER; i++)
	{
		//본인한테 안 보낼거임! 이라고 할 때 받아온 정보가 있다면 넘어가기!
		if (!sendSelf && i == sendFD) continue;

		//대상이 없는데 보낼 순 없겠죠?
		if (pollFDArray[i].fd != -1)
		{
			//           메시지,   길이,   대상의 소켓
			SendMessage(message, length, pollFDArray[i].fd);

			//보냈다!   그랬더니 다 보냄! 이라고 했을 때 돌려주기!
			if (++send >= currentUserNumber) break;
		};
	};
}

//메시지를 구분하는 용도                  이걸 준 유저
MessageInfo* ProcessMessage(char* input, int userIndex)
{
	for (int i = 0; i < 4; i++)
	{
		byteConvertor.character[i] = input[i];
	};
	//메시지타입		길이
	//[][]			[][]

	MessageInfo* result;
	//메시지 타입에 따라서 내용 넣어주기!
	switch ((MessageType)byteConvertor.shortInteger[0])
	{
	case MessageType::LogIn:
		result = new MessageInfo_Login(input, userIndex);
		break;
	default:
		result = new MessageInfo();
		result->type = MessageType::Chat;	//타입 돌려주기!
		break;
	}
	result->length	= byteConvertor.shortInteger[1] + 4;			//길이도 줍시다!

	return result;
}

int TranslateMessage(int fromFD, char* message, int messageLength, MessageInfo* info)
{
	//전체 길이와 하나의 메시지 길이 둘 중에 작은 값으로!
	int currentLength = min(messageLength, info->length);
	//메모리 중에서 제가 처리해야하는 메모리까지만!
	char* target = new char[currentLength];
	memcpy(target, message, currentLength);

	//타입에 따라 다른 행동!
	switch (info->type)
	{
	case MessageType::Chat:
		BroadCastMessage(target, currentLength, fromFD);
		cout << "Message Send To " << send << "User : " << target + 4 << endl;
		break;
	case MessageType::LogIn:
	{
		MessageInfo_Login* loginInfo = (MessageInfo_Login*)info;
		//로그인 정보에서 이름을 받아와서 시도해봅니다!
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
	//사실 메시지같은 경우는 하나씩 보내면 조금 효율이 떨어집니다 ㅎㅎ
	//보낼 수 있을 때 여러개를 같이 보내는 게 좋습니다!
	//모아두었다가 보내는 개념!
	//전체 메시지 길이 - 지금 확인한 메시지 길이!
	//아직 뒤에 메시지가 더 있어요! 라고 하는 걸 확인할 수 있죠!
	//그래서 처리한 길이를 여기에서 확인하고 갈게요!
	return currentLength;
}
