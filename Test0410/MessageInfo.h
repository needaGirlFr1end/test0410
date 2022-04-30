//통합! 이라고 하는 것이죠!
//Struct랑 비슷하게 쓰실 수 있습니다!
//차이점이 무엇이냐? 하나의 메모리를 여러가지 자료형이 공유해요!

//[0][0][0][10]
//          10  int
//          \n  char[3]
union ConvertionBase
{
	unsigned int uInteger;
	int integer;
	float floating;
	char character[4];
	short shortInteger[2];
	unsigned short uShortInteger[2];
};
ConvertionBase byteConvertor;

//항상 클라이언트의 enum값과 일치시켜주세요!
enum class MessageType
{
	LogIn,
	LogOut,
	Chat,

	Length
	//제가 가진 메시지 타입의 개수보다 더 많은 내용이 들어오면 무시!
};

class MessageInfo
{
public :
	MessageType type;
	int length;
};

class MessageInfo_Login : public MessageInfo
{
public :
	int userIndex;
	string name;
	MessageInfo_Login(char* message, int targetUser)
	{
		type = MessageType::LogIn;

		userIndex = targetUser;
		//메시지의 4번째부터 내용을 넣어주도록 합시다!
		name = &(message[4]);
	}
};