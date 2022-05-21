//����! �̶�� �ϴ� ������!
//Struct�� ����ϰ� ���� �� �ֽ��ϴ�!
//�������� �����̳�? �ϳ��� �޸𸮸� �������� �ڷ����� �����ؿ�!

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

//�׻� Ŭ���̾�Ʈ�� enum���� ��ġ�����ּ���!
enum class MessageType
{
	EndOfLine, //���ڿ��� ���� �Ǵ��� �� \0�̶�� �ϴ� �� �ľ��߽��ϴ�! �޽����� �ڿ� �� ���� ��� �ϴ� �� EndOfLine���� ǥ��!
	LogIn,
	LogOut,
	Chat,
	Input,

	Unknown,  //���� ���� �𸣰ڴ� ����!
	Length
	//���� ���� �޽��� Ÿ���� �������� �� ���� ������ ������ ����!
};

enum class InputType
{
	Button0,
	Button1,
	Button2,
};

class MessageInfo
{
public :
	MessageType type;
	int length;
	int userIndex;
};

class MessageInfo_Login : public MessageInfo
{
public :
	string name;
	MessageInfo_Login(char* message, int targetUser)
	{
		type = MessageType::LogIn;

		userIndex = targetUser;
		//�޽����� 4��°���� ������ �־��ֵ��� �սô�!
		name = &(message[4]);
	}
};

class MessageInfo_Chat : public MessageInfo
{
public:
	string value;

	MessageInfo_Chat(char* message, int targetUser)
	{
		type = MessageType::Chat;

		userIndex = targetUser;
		//�޽����� 4��°���� ������ �־��ֵ��� �սô�!
		value = &(message[4]);
	}
};

class MessageInfo_Input : public MessageInfo
{
public:
	InputType currentType;

	MessageInfo_Input(InputType wantType, int targetUser)
	{
		type = MessageType::Input;

		currentType = wantType;
		userIndex = targetUser;
	}
};