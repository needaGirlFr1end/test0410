//메시지를 전달받아서 그 내용을 바이트단위로 분리해주는 녀석!
//메시지 타입이나 길이는 잘 읽어서 돌려드립니다^^
void DebugMessage(char* message)
{
	//처음 헤더를 읽어오기 시작합니다!
	for (int i = 0; i < 4; i++)
	{
		byteConvertor.character[i] = message[i];
	};

	unsigned short type = byteConvertor.uShortInteger[0];

	//타입이 0이니까! 더 메시지가 없어요!
	if (type == 0) 
	{ 
		return;
	};
	//첫 번째 두 개의 바이트는 타입으로!
	cout << "[ type : " << type << "] ";

	//그 다음 두 개의 바이트는 길이로!
	unsigned short length = byteConvertor.uShortInteger[1];
	cout << "[ leng : " << length << "] ";

	//뒤에 있는 애들을 몽땅 읽어오기!
	for (int i = 0; i < length; i++)
	{
		cout << "[" << (int)message[i + 4] << "]";
	};

	cout << endl;
}

bool SendMessage(char* message, int length, int userNumber)
{
	//서버가 무언가 보낼 때 "적어 주는 거"에요 그래서 Write라고 부르고
	//받을 때에는 Read하겠죠?
	//서버가 정보를 보내는 것은! 문제가 하나가 있습니다!
	//이미 보내고 있는 중에는 추가적인 메시지를 보내기가 힘들어요!
	//지금 이거 보내고 있어서 제가 더 처리해드리기가 힘드네요!
	//"이거 다 하고 보내드릴게요^^ 라고 하면 참 좋겠지만! 서버는 그렇게 행동하지 않아요!
	//"지금 하고 있으니까 보내지 마세요!" 라고 그냥 날려버립니다!
	//그럼 메세지를 어떻게 보내죠? 라고 생각하실 수 있죠!
	//"메세지 큐"라고 하는 것이 필요합니다! 해당 유저에게 보내야 하는 메시지들을
	//저장해놓는 공간이 필요해요! 보내지 못했을 때 담아두려고!
	//해당 유저가 존재할 경우에!
	if (userArray[userNumber] != nullptr)
	{
		//해당 유저에게 메시지를 전달해주기!
		userArray[userNumber]->MessageQueue(message, length);
	}
	//write(sendFD, message, length);
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
			//           메시지,   길이,   대상
			SendMessage(message, length,  i);

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

	DebugMessage(input);

	MessageInfo* result;
	//메시지 타입에 따라서 내용 넣어주기!
	switch ((MessageType)byteConvertor.shortInteger[0])
	{
	case MessageType::EndOfLine:return nullptr;

	case MessageType::LogIn:	result = new MessageInfo_Login(input, userIndex);
		break;
	case MessageType::Chat:		result = new MessageInfo_Chat(input, userIndex);
		break;
	case MessageType::Input:
		//4번째 칸부터 제가 입력 타입을 적어놓았습니다!
		for (int i = 0; i < 4; i++) byteConvertor.character[i] = input[i + 4];
		result = new MessageInfo_Input((InputType)byteConvertor.integer, userIndex);
		break;
	default:					result = new MessageInfo();
								result->type = MessageType::Unknown;
	}
	result->length	= byteConvertor.shortInteger[1] + 4;			//길이도 줍시다!

	return result;
}

int TranslateMessage(int fromFD, char* message, int messageLength, MessageInfo* info)
{
	//아무 것도 없는데요?  //끝까지 이동!
	if (info == nullptr) return MAX_BUFFER_SIZE;

	//전체 길이와 하나의 메시지 길이 둘 중에 작은 값으로!
	int currentLength = min(messageLength, info->length);	

	//타입에 따라 다른 행동!
	switch (info->type)
	{
	case MessageType::Chat:
	{
		MessageInfo_Chat* chatInfo = (MessageInfo_Chat*)info;
		//메모리 중에서 제가 처리해야하는 메모리까지만!
		char* sendResult = new char[currentLength + 4];
		byteConvertor.uShortInteger[0] = (short)MessageType::Chat;
		//                               전체 메시지 길이에서 헤더 길이 뺀 것!
		byteConvertor.uShortInteger[1] = currentLength - 4;

		//위쪽에서 제가 새로 만든 헤더를 넣어주는 거에요!
		for (int i = 0; i < 4; i++)
		{
			sendResult[i] = byteConvertor.character[i];
		};
		//그 다음 4byte에는 유저 번호를 입력해줍니다!
		byteConvertor.integer = fromFD;
		//4 ~ 7까지는 유저 번호가 들어갑니다!
		for (int i = 0; i < 4; i++)
		{
			sendResult[i + 4] = byteConvertor.character[i];
		};
		// 4칸을 추가를 해놓았기 때문에 4칸의 여유를 더 주도록 하겠습니다!
		memcpy(sendResult + 8, message + 4, currentLength - 4);
		//                 여기                             여기      두 개가 연관되는 개수기 때문에 맞춰줍시다!

		BroadCastMessage(sendResult, currentLength + 4, fromFD);

		cout << "Message Send From " << userArray[fromFD]->GetName() << " : " << sendResult + 8 << endl;
		delete sendResult;
		break;
	}
	case MessageType::LogIn:
	{
		MessageInfo_Login* loginInfo = (MessageInfo_Login*)info;

		cout << "Someone Try Login! Name is " << loginInfo->name << "!!" << endl;
		//           유저번호  성공여부 (로그인 성공 / 중복 로그인 / 신규 로그인)
		//[][] [][]  [][][][] []
		char sendResult[9] = { 0 };

		byteConvertor.uShortInteger[0] = (short)MessageType::LogIn;
		byteConvertor.uShortInteger[1] = 5;

		for (int i = 0; i < 4; i++)
		{
			sendResult[i] = byteConvertor.character[i];
		};

		sendResult[8] = userArray[fromFD]->LogIn(loginInfo->name);

		//로그인 정보에서 이름을 받아와서 시도해봅니다!
		switch (sendResult[8])
		{
		case 0: cout << "Login Succeed"		<< endl; break;
		case 1: cout << "Invalide Password" << endl; break;
		case 2: cout << "Already Logined"	<< endl; break;
		case 3: cout << "Non-Exist ID"		<< endl; break;
		case 4: cout << "ID Not Fit In Form"<< endl; break;
		default:cout << "Unknown Error"		<< endl; break;
		};

		//본인한테 성공 여부를 보내주는 것!
		SendMessage(sendResult, 9, fromFD);

		//로그인에 실패한 경우에는 다른 사람들한테 아무 말도 안해도 괜찮아요!
		if (sendResult[8] != 0) break;

		//다른 사람들한테 보내줄 내용!
		char* broadcastResult;

		int currentLength = 0;
		//글자 끝이 나오지 않았다!
		while (loginInfo->name[currentLength] != '\0')
		{
			++currentLength; //글자 끝이 나올 때까지 달려서 길이를 재어보기!
		};

		//일반적인 내용 [][] [][] [][][][] -> [][][][][][][][][][]
		broadcastResult = new char[currentLength + 8];
		byteConvertor.uShortInteger[0] = (short)MessageType::LogIn;
		byteConvertor.uShortInteger[1] = currentLength + 4;
		//                                            유저번호 들어갈 4칸!
		for (int i = 0; i < 4; i++) broadcastResult[i] = byteConvertor.character[i];

		//유저 번호 무엇인가요?
		byteConvertor.integer = loginInfo->userIndex;
		for (int i = 0; i < 4; i++) broadcastResult[i + 4] = byteConvertor.character[i];

		//이름까지 채워주기!
		memcpy(broadcastResult + 8, loginInfo->name.c_str(), currentLength);

		//이제 방송합시다!
		BroadCastMessage(broadcastResult, currentLength + 8);

		//잘 썼습니다 ^^
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
		return MAX_BUFFER_SIZE; //최대치까지 밀어서 그 뒤에 메시지가 더 없다고 알려줍니다!

	default:
		break;
	}

	//메시지 처리 완료!
	delete info;

	//사실 메시지같은 경우는 하나씩 보내면 조금 효율이 떨어집니다 ㅎㅎ
	//보낼 수 있을 때 여러개를 같이 보내는 게 좋습니다!
	//모아두었다가 보내는 개념!
	//전체 메시지 길이 - 지금 확인한 메시지 길이!
	//아직 뒤에 메시지가 더 있어요! 라고 하는 걸 확인할 수 있죠!
	//그래서 처리한 길이를 여기에서 확인하고 갈게요!
	return currentLength;
}
