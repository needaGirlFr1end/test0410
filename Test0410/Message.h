enum class MessageType
{
	Chat,
	Length // ���� ���� �޽��� Ÿ���� �������� ������ ����!


};

void ProcessMessage() {

}

void TranslateMessage() {

}

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