
// 항상 클라이언트의 eunm값을 일치시켜야함
enum class MessageType
{
	LogIn,
	LogOut,
	Chat,



	Length // 제가 가진 메시지 타입의 개수보다 많으면 무시!


};

// struct랑 비슷하게 사용가능
// 하나의 메모리를 여러가지 자료형이 공유가능

// 0 0 0 10 
// int  10
// char \n
// 하나의 메모리를 여러 자료형으로 공유

union ConvertionBase {
	
	// 같은크기 자료형으로 사용
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

//												본인에게 보내기는 true 
//												sendSelf true로 설정하기위해 
//												sendFD를 초기화해야함
void BroadCastMessage(char* message, int length, int sendFD = -1, bool sendSelf = true) {
	
	// 유저의 인원 수를 알고
	// 끝까지 찾지않아도 유저한테 다 전달했다면 끝
	// 보낼때마다 send를 늘려주고 유저수만큼 보낼 시 끝내기 
	// sendSelf가 false면 본인한테보내주는게 아니면 보내야 하는 유저개수를
	// 수정 해야한다

	// 반복문이 그 한명 을 찾는걸 대비
	//		본인이라면 0 아니면 1
	int send = sendSelf ? 0 : 1;
	
	
	// 0번은 리슨소켓 i가 최대치까지 가거나 현재 유저 수만큼 보냈다면
	for (int i = 1; i < MAX_USER_NUMBER ; i++) {

		// || send >= currentUserNumber
		// 본인에게 안보낼껀데 받아온 정보가 있다면 넘어가기
		if (!sendSelf && i == sendFD) continue;

		// 대상이 없는데 보낼 수 없는 조건
		if (pollFDArray[i].fd != -1) {

			// 서버가 무언갈 보낼때  "메모 하기" 
			// 받을떈 Read()
			//		대상이 되는 소켓	보낼 메세지	길이
			if (write(pollFDArray[i].fd, message, length)) {
				
				
				// 보냇따!  다보냈을때 리턴해주기
				if (++send >= currentUserNumber) return;
			};

		};
	};

}

// 메시지를 구분하는 용도                     길이 받을 int주세용
MessageInfo ProcessMessage(char input[4]) {

	for (int i = 0; i < 4; i++) {
		byteConvertor.character[i] = input[i];
	};


	// 메세지 타입 길이
	// [][]       [][]
	
	MessageInfo result;
	
	result.type = byteConvertor.shortInteger[0];		// 타입 돌려주기
	result.length = byteConvertor.shortInteger[1] + 4;		// 길이까지 4qkdlxm cnrk

	return result;
}

int TranslateMessage(int fromFD, char* message, int messageLength, MessageInfo info) {
	
	// 전체 길이와 하나의 메세지의 길이중 더 작은값으로
	int currentLength = min(messageLength, info.length);

	// 메모리중에서 처리해야되는 메모리까지만
	char* target = new char[currentLength];
	memcpy(target, message, currentLength);
	
	// 타입에 따라 분류
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


	// 메시지경우 하나씩 보내면 효율이 떨어지므로
	// 한번에 보내도록 하는게 좋음
	// 전체 메세지 길이 - 지금 확인한 메시지 길이
	// 
	return messageLength - info.length;
}