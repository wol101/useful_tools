// SocketMessages.h - defines for socket messages

#ifndef SocketMessages_h
#define SocketMessages_h

const int kSocketMaxMessageLength = 64; // includes terminating 0
const char * kSocketRequestTaskMessage = "SM_RequestTask";
const char * kSocketSendingCommandLine = "SM_CommandLine";
const char * kSocketSendingExtraData = "SM_ExtraData";
const char * kSocketSendingGenome = "SM_Genome";
const char * kSocketSendingScore = "SM_Score";

#endif
