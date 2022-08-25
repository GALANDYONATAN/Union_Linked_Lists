#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "md5.h" //using void md5String(const char* input, char* output);
#include "md4.h" //using void md4String(const char* input, char* output);

#define MD4_HASH 0
#define MD5_HASH 1
#define True 1
#define False 0
typedef unsigned char HashType;
typedef unsigned char Byte;
typedef char Bool;

typedef struct msgWithHash
{
	unsigned int id;
	char* text;
	union
	{
		Byte md4[16];
		Byte md5[16];
	} hashDigest;
	HashType type;
}msgWithHash;

typedef struct msgListNode msgListNode;
struct msgListNode
{
	msgWithHash* data;
	msgListNode* next;
};

void print_hash(const unsigned char* p);
msgWithHash* createMessage(unsigned int id, const char* text, HashType type);
void printMessage(const msgWithHash* message);
msgListNode* addMessage(msgListNode* head, msgWithHash* data);
int findMsgByHashIterative(const msgListNode* head, Byte hash[16]);
int findMsgByHashRecursive(const msgListNode* head, Byte hash[16]);
msgListNode* deleteMessage(msgListNode* head, unsigned int id);
Bool verifyIntegrity(msgListNode* head, unsigned int id, Byte compareHash[16]);
msgListNode* reverseMsgList(msgListNode* head);
void freeMsgList(msgListNode* head);





int main() {

	msgWithHash* msg1 = createMessage(87651233, "", MD4_HASH);
	msgWithHash* msg2 = createMessage(3, "banana1", MD5_HASH);
	msgWithHash* msg3 = createMessage(275, "banana2!", MD4_HASH);
	msgWithHash* msg4 = createMessage(342234, "Hello World", MD4_HASH);

	msgListNode* head = NULL;
	head = addMessage(head, msg4);
	head = addMessage(head, msg2);
	head = addMessage(head, msg3);
	head = addMessage(head, msg1);

	msgListNode* temp = head;
	printf("Sorted list by ID: \n\n");
	while (temp != NULL) {
		printMessage(temp->data);
		temp = temp->next;
	}

	int z = findMsgByHashIterative(head, head->next->data->hashDigest.md4);
	int y = findMsgByHashRecursive(head, (Byte*)(head->next->data->hashDigest.md4));

	switch (y) {
	case -1:
		printf("There`s no such hash in the list\n");
		break;
	case -2:
		printf("Error");
		break;
	default:
		printf("The item location is %d\n\n", z);
	}

	head = deleteMessage(head, 3);

	z = verifyIntegrity(head, head->data->id, head->next->data->hashDigest.md4);
	switch (z) {
	case -1:
		printf("There`s no such id in the list\n");
		break;
	case 0:
		printf("Hash not verified\n");
		break;
	case 1:
		printf("Hash verified\n");
	}

	head = reverseMsgList(head);
	temp = head;
	printf("Reversed sorted list by ID: \n\n");
	while (temp != NULL) {
		printMessage(temp->data);
		temp = temp->next;
	}
	freeMsgList(head);
	return 0;
}

msgWithHash* createMessage(unsigned int id, const char* text, HashType type) {

	msgWithHash* newmsg = (msgWithHash*)malloc(sizeof(msgWithHash));
	if (!newmsg)
	{
		printf("Allocation failed!");
		return NULL;
	}
	newmsg->text = (char*)malloc((strlen(text) + 1));
	if (newmsg == NULL)
	{
		printf("Allocation failed!");
		return NULL;
	}

	newmsg->id = id;
	strcpy(newmsg->text, text);
	newmsg->type = type;
	if (newmsg->type == MD4_HASH)
		md4String(newmsg->text, newmsg->hashDigest.md4);
	else if (newmsg->type == MD5_HASH)
		md5String(newmsg->text, newmsg->hashDigest.md5);
	else
	{
		printf("Error - type must be MD4 or MD5");
		return NULL;
	}
	return newmsg;
}

void printMessage(const msgWithHash* message) {

	if (message == NULL)
	{
		printf("Error! - message NULL");
		return;
	}

	printf("Message ID: %d\n", message->id);
	printf("Message Text: %s\n", message->text);

	if (message->type == MD4_HASH) {
		printf("Md4 Hash: ");
		print_hash(message->hashDigest.md4);
	}
	else if (message->type == MD5_HASH) {
		printf("Md5 Hash: ");
		print_hash(message->hashDigest.md5);
	}
	else
		printf("Error   define type");
}

msgListNode* addMessage(msgListNode* head, msgWithHash* data) {

	msgListNode* newmsg = (msgListNode*)malloc(sizeof(msgListNode));
	if (!newmsg)
	{
		printf("Allocation failed!");
		return head;
	}

	newmsg->data = data;
	newmsg->next = NULL;

	if (head == NULL)
		return newmsg;

	msgListNode* temp = head;
	msgListNode* ptr = NULL;

	while (temp != NULL && temp->data->id < newmsg->data->id) {
		ptr = temp;
		temp = temp->next;
	}

	newmsg->next = temp;

	if (temp == head)
		head = newmsg;
	else
		ptr->next = newmsg;

	return head;
}

int findMsgByHashIterative(const msgListNode* head, Byte hash[16]) {

	if (head == NULL) {
		printf("Error! - no items appeared");
		return -2;
	}

	const msgListNode* temp = head;
	int anothernum = 1;

	while (temp != NULL) {
		if (hash == temp->data->hashDigest.md4)
			return anothernum;
		temp = temp->next;
		anothernum++;
	}

	return -1;

}

int findMsgByHashRecursive(const msgListNode* head, Byte hash[16]) {

	const msgListNode* temp = head;
	static int counter = 1;

	if (temp == NULL)
		return -1;

	if (hash == temp->data->hashDigest.md4)
		return counter;

	counter++;

	return findMsgByHashRecursive(temp->next, hash);

}

msgListNode* deleteMessage(msgListNode* head, unsigned int id) {

	if (head == NULL) {
		printf("The list is empty!");
		return head;
	}

	msgListNode* temp = head;
	msgListNode* ptr = NULL;

	while (temp != NULL && id != temp->data->id) {
		ptr = temp;
		temp = temp->next;
	}

	if (temp == NULL) {
		printf("There`s no such file with this id");
		return head;
	}

	if (temp == head)
		head = temp->next;
	else
		ptr->next = temp->next;

	free(temp->data);
	free(temp);
	return head;
}

Bool verifyIntegrity(msgListNode* head, unsigned int id, Byte compareHash[16]) {

	msgListNode* temp = head;

	while (temp != NULL && id != temp->data->id) {
		temp = temp->next;
	}

	if (temp == NULL)
		return -1;

	if (temp->data->hashDigest.md4 == compareHash)
		return 1;
	else
		return 0;
}

msgListNode* reverseMsgList(msgListNode* head) {

	msgListNode* prev = NULL, * cur = head, * tmp;
	while (cur) {
		tmp = cur->next;
		cur->next = prev;
		prev = cur;
		cur = tmp;
	}
	return prev;
}

void freeMsgList(msgListNode* head) {

	if (head == NULL)
		return;

	msgListNode* value = head->next;

	while (value != NULL) {
		free(head->data);
		free(head);
		head = value;
		value = value->next;
	}
	free(head->data);
	free(head);

}

void print_hash(const unsigned char* p) {
	for (unsigned int i = 0; i < 16; ++i) {
		printf("%02x", p[i]);
	}
	printf("\n");
}