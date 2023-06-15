// PVMMessages.h - a set of constants used for PVM message IDs

#ifndef VMMessages_h
#define VMMessages_h

// integer length, followed by a list of doubles
const int kMessageDoubleArray = 1;

// a single double
const int kMessageDouble = 2;

// integer length (not including null), followed by a null terminated string
const int kMessageString = 3;

// host failure notification message
const int kMessageHostFailure = 4;

// acknowledge message
const int kMessageAcknowledge = 5;

#endif // VMMessages_h

