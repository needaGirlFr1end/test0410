enum class MessageType
{
	Chat,
	Length // 제가 가진 메시지 타입의 개수보다 많으면 무시!


};

void ProcessMessage() {

}

void TranslateMessage() {

}

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
	for (int i = 1; i < MAX_USER_NUMBER || send >= currentUserNumber; i++) {


		// 본인에게 안보낼껀데 받아온 정보가 있다면 넘어가기
		if (!sendSelf && 1 == sendFD) continue;

		// 대상이 없는데 보낼 수 없는 조건
		if (pollFDArray[i].fd != -1) {

			// 서버가 무언갈 보낼때  "메모 하기" 
			// 받을떈 Read()
			//		대상이 되는 소켓	보낼 메세지	길이
			write(pollFDArray[i].fd, message, length);

			++send;
		};
	};
}